# Firmware

Sketch (`src/`) que corre en una placa **ESP32 dev board**: la máquina de estados de la rutina autónoma, el modo manual vía interfaz web, y el reparto de control entre ambos modos. Controla los 3 motores reales del brazo (ruedas X, servo de orientación de línea, brazo Z) — ver [mecanismo de apertura](../docs/mecanismo-apertura.md). El mapeo de pines y los datos del Access Point están en [pines.md](pines.md).

## Organización de módulos

- `src/src.ino` — `setup()` / `loop()`, orquesta todo. (El archivo se llama `src.ino` porque Arduino IDE/CLI exige que el `.ino` principal tenga el mismo nombre que su carpeta contenedora, `src/`.)
- `src/pines.h` — mapeo de pines (placeholder, ajustar al cableado real — ver [pines.md](pines.md)).
- `src/motores.h` / `.cpp` — control real de los 3 motores: `motores_moverX(Sentido)` y `motores_moverZ(Sentido)` (dirección vía driver L298N), `motores_orientarLinea(int linea)` (servo, librería `ESP32Servo`).
- `src/sensores.h` / `.cpp` — lectura con antirrebote (no bloqueante) del sensor óptico de columna y de los botones de pedido/emergencia.
- `src/seguridad.h` / `.cpp` — chequeo de emergencia con prioridad sobre el resto de la máquina de estados; corta los motores y enciende el LED de error.
- `src/modo.h` / `.cpp` — modo de operación a nivel de todo el sistema (`MODO_MANUAL` / `MODO_AUTONOMO`), separado del estado interno de la rutina autónoma.
- `src/maquina_estados.h` / `.cpp` — la máquina de estados de la rutina autónoma (ver [diagrama-flujo-estados.md](../docs/diagrama-flujo-estados.md)). Expone `maquinaEstados_solicitarPedido(columna, linea)`, el punto de entrada único usado tanto por el botón físico como por `/pedido` de la web.
- `src/servidor_web.h` / `.cpp` — Access Point propio (`CajonesBot`) + página del modo manual + endpoint `/pedido?columna=N&linea=M` (ver [modo-manual.md](../docs/modo-manual.md)).

## Dependencia externa: ESP32Servo

El servo de orientación de línea usa la librería `ESP32Servo` (no viene con el core de Arduino). Instalarla antes de compilar:

```
arduino-cli lib install "ESP32Servo"
```

## Cómo se reparte el loop entre modo manual y modo autónomo

`src.ino` llama siempre a `servidorWeb_actualizar()` y `sensores_actualizar()`. Después:

- Si el sistema está en `MODO_AUTONOMO`, o en `ESTADO_ERROR_SEGURO` (para poder entrar/salir de la parada de emergencia sin importar el modo), corre `maquinaEstados_actualizar()`.
- Si no, el sistema está en modo manual idle: los motores se controlan directo desde los handlers de `servidor_web.cpp` (`/manual/x`, `/manual/z`, `/manual/linea`), y solo se escucha el botón físico de pedido para arrancar una rutina (siempre columna 0 / línea 0; para otra combinación hay que usar la web).

Al terminar la rutina autónoma (o al resetear una emergencia), la máquina de estados vuelve sola a `MODO_MANUAL` — no hace falta ninguna acción extra del operador.

El botón físico de pedido se lee **por flanco**, no por nivel: como resetear una emergencia exige mantenerlo presionado (junto con soltar la emergencia), leerlo por nivel haría que el robot arrancara una rutina autónoma solo, apenas se resetea la emergencia. El flanco se rastrea en cada vuelta del loop (no solo cuando el sistema está en modo manual) para que quede consumido mientras el botón se mantuvo presionado durante la emergencia.

## Nota de diseño: conteo de marcas en X, fases restantes por tiempo

A diferencia del diseño anterior (riel fijo + fin de carrera por columna), acá el robot se mueve libremente sobre ruedas: saber "a qué columna llegó" requiere contar flancos del sensor óptico de columna mientras se mueve en X, no solo leer un booleano (ver [sensores-actuadores.md](../docs/sensores-actuadores.md)). Las demás fases (orientación de línea, extensión, apertura, sidestep, vuelta a home) se resuelven por **tiempo**, consistente con el mínimo de sensores confirmado y con no inventar hardware que la cátedra no pidió. Si el grupo agrega sensores dedicados más adelante, alcanza con reemplazar esas comparaciones de tiempo por lecturas de `sensores.*` — la lógica del resto de la máquina de estados no cambia.

## Por qué la parada de emergencia no está en la web

A propósito: un E-stop no debería depender de que el WiFi o el servidor web estén funcionando. Es el botón físico `PIN_BTN_EMERGENCIA`, leído directamente por el microcontrolador con prioridad sobre cualquier otra lógica, en cualquier modo.

## Cómo probarlo en el banco

1. Cablear según [pines.md](pines.md): driver(s) L298N para los motores X y Z, servo de orientación, sensor óptico de columna, botones de pedido/emergencia, LED de error.
2. Instalar la librería `ESP32Servo` (ver arriba).
3. Abrir `src/src.ino` en Arduino IDE (o compilar con `arduino-cli compile --fqbn esp32:esp32:esp32 firmware/src`), cargar a la placa.
4. Abrir el monitor serie (115200 baudios): imprime cada cambio de estado y de modo.
5. Conectarse desde el celular/notebook a la red WiFi `CajonesBot` (ver clave en [pines.md](pines.md)) y abrir `http://192.168.4.1` — ahí se puede mover X/Z/línea manualmente y pedir una columna/línea.
6. Probar también con los botones físicos: `PIN_BTN_PEDIDO` arranca la rutina (columna 0/línea 0), y `PIN_BTN_EMERGENCIA` prueba la parada segura (se resetea con `PIN_BTN_PEDIDO` mientras la emergencia no esté presionada).

## Tests automáticos (sin placa)

Además de la prueba física de arriba, `../test/` tiene tests que corren en la compu, sin ESP32 ni servo real, compilando los `.cpp` de producción tal cual con mocks de las primitivas de Arduino y de `ESP32Servo`. Sirven para validar la lógica de la FSM (secuencia completa, conteo de marcas en X, prioridad de la emergencia, antirrebote, pedidos ignorados mientras hay una rutina en curso) antes de tocar la placa. Correrlos con:

```
bash test/run_tests.sh
```

Ver [test/README.md](../test/README.md) para el detalle.

# Firmware

Sketch de banco (`src/`) que corre en una placa **ESP32 dev board** real: la máquina de estados de la rutina autónoma, el modo manual vía interfaz web, y el reparto de control entre ambos modos. Simula los motores con LEDs y los fines de carrera con botones — para validar la lógica antes de tener el brazo físico armado. El mapeo de pines y los datos del Access Point están en [pines.md](pines.md).

## Organización de módulos

- `src/src.ino` — `setup()` / `loop()`, orquesta todo. (El archivo se llama `src.ino` porque Arduino IDE/CLI exige que el `.ino` principal tenga el mismo nombre que su carpeta contenedora, `src/`.)
- `src/pines.h` — mapeo de pines del sketch de banco.
- `src/motores.h` / `.cpp` — hoy son 2 LEDs (`motores_moverX()`, `motores_moverVastago()`, `motores_pararTodos()`). Se reemplaza por el control real del driver de cada motor cuando exista la electrónica, sin tocar `maquina_estados.cpp` ni `servidor_web.cpp`.
- `src/sensores.h` / `.cpp` — lectura con antirrebote (no bloqueante) de los 2 fines de carrera confirmados como mínimo (`docs/sensores-actuadores.md`) y de los botones de pedido/emergencia.
- `src/seguridad.h` / `.cpp` — chequeo de emergencia con prioridad sobre el resto de la máquina de estados; hoy corta los LEDs, se reemplaza por el corte real (relé) cuando exista la electrónica.
- `src/modo.h` / `.cpp` — modo de operación a nivel de todo el sistema (`MODO_MANUAL` / `MODO_AUTONOMO`), separado del estado interno de la rutina autónoma.
- `src/maquina_estados.h` / `.cpp` — la máquina de estados de 8 estados de la rutina autónoma (ver [diagrama-flujo-estados.md](../docs/diagrama-flujo-estados.md)). Expone `maquinaEstados_solicitarPedido(columna)`, el punto de entrada único usado tanto por el botón físico como por `/pedido` de la web.
- `src/servidor_web.h` / `.cpp` — Access Point propio (`CajonesBot`) + página del modo manual + endpoint `/pedido?columna=N` (ver [modo-manual.md](../docs/modo-manual.md)).

## Cómo se reparte el loop entre modo manual y modo autónomo

`src.ino` llama siempre a `servidorWeb_actualizar()` y `sensores_actualizar()`. Después:

- Si el sistema está en `MODO_AUTONOMO`, o en `ESTADO_ERROR_SEGURO` (para poder entrar/salir de la parada de emergencia sin importar el modo), corre `maquinaEstados_actualizar()`.
- Si no, el sistema está en modo manual idle: los motores se controlan directo desde los handlers de `servidor_web.cpp` (`/manual/x`, `/manual/z`), y solo se escucha el botón físico de pedido para arrancar una rutina.

Al terminar la rutina autónoma (o al resetear una emergencia), la máquina de estados vuelve sola a `MODO_MANUAL` — no hace falta ninguna acción extra del operador.

## Nota de diseño: sensores reutilizados en el sketch de banco

El diseño documentado en `docs/diagrama-flujo-estados.md` menciona un "fin de carrera de reposo en Z" para `RETRAYENDO_Z` y una confirmación de posición "home" para `VOLVIENDO_HOME`, pero `docs/sensores-actuadores.md` solo confirma 2 sensores como mínimo (fin de carrera X y fin de carrera de la transición de la ranura). Para no inventar hardware que la cátedra no pidió, el sketch de banco resuelve estas dos fases por **tiempo** (consistente con el fail-safe por gravedad ya documentado para volver a reposo) en vez de por sensor. Si el grupo termina agregando sensores dedicados en el diseño eléctrico final, alcanza con reemplazar esas comparaciones de tiempo por lecturas de `sensores.*` — la lógica del resto de la máquina de estados no cambia.

## Por qué la parada de emergencia no está en la web

A propósito: un E-stop no debería depender de que el WiFi o el servidor web estén funcionando. Es el botón físico `BTN_EMERGENCIA`, leído directamente por el microcontrolador con prioridad sobre cualquier otra lógica, en cualquier modo.

## Cómo probarlo en el banco (con la placa)

1. Cablear en protoboard según [pines.md](pines.md): 3 LEDs (motor X, motor vástago, error) + 4 botones (fin de carrera X, fin de carrera transición, pedido, emergencia).
2. Abrir `src/src.ino` en Arduino IDE (o compilar con `arduino-cli compile --fqbn esp32:esp32:esp32 firmware/src`), cargar a la placa.
3. Abrir el monitor serie (115200 baudios): imprime cada cambio de estado y de modo.
4. Conectarse desde el celular/notebook a la red WiFi `CajonesBot` (ver clave en [pines.md](pines.md)) y abrir `http://192.168.4.1` — ahí se puede mover X/vástago manualmente y pedir una columna.
5. Probar también con los botones físicos: `BTN_PEDIDO` arranca la rutina, `BTN_FIN_CARRERA_X` y `BTN_FIN_CARRERA_TRANSICION` avanzan las fases correspondientes, y `BTN_EMERGENCIA` prueba la parada segura (se resetea con `BTN_PEDIDO` mientras la emergencia no esté presionada).

## Tests automáticos (sin placa)

Además de la prueba física de arriba, `../test/` tiene tests que corren en la compu, sin ESP32, compilando los `.cpp` de producción tal cual con un mock de las primitivas de Arduino. Sirven para validar la lógica de la FSM (secuencia completa, prioridad de la emergencia, antirrebote, pedidos ignorados mientras hay una rutina en curso) antes de tocar la placa. Correrlos con:

```
bash test/run_tests.sh
```

Ver [test/README.md](../test/README.md) para el detalle.

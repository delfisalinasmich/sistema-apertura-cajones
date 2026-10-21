# Firmware

Proyecto **PlatformIO** (framework Arduino, ESP32 NodeMCU-32S) construido sobre el hardware real que armó Coty — ver su guía en [`referencias/`](../referencias/) y el mecanismo real en [mecanismo-apertura.md](../docs/mecanismo-apertura.md). El mapeo de pines y los datos del Access Point están en [pines.md](pines.md).

## Entornos (`platformio.ini`)

| Entorno | Archivo | Para qué |
|---|---|---|
| `nodemcu-32s` | `src/main.cpp` | **Programa principal**: máquina de estados de la rutina autónoma + modo manual vía web |
| `calibracion` | `src/calibracion.cpp` | Calibrar el sensor de línea (comandos `w`, `b`, `r`, `l`, `s`) |
| `servo_test` | `src/servo_test.cpp` | Prueba suelta del servo en GPIO25 |
| `prueba` | `src/prueba.cpp` | Prueba de la mecánica aislada: base + servo + imán (sin ruedas ni sensor) |

Cada entorno compila un solo `.cpp`, elegido con `build_src_filter` — **si agregan un entorno nuevo, hay que excluir su archivo en los demás**. Subir siempre **un solo entorno con `-e`**: `pio run -e nodemcu-32s -t upload`, nunca sin `-e` (sube los cuatro, uno detrás del otro).

## Organización de módulos (entorno `nodemcu-32s`)

- `src/main.cpp` — `setup()` / `loop()`, orquesta todo.
- `include/pines.h` — mapeo de pines + `motorPinesSeguros()` (deja los motores en LOW apenas arranca).
- `include/sensor_linea.h` — lectura y calibración (en flash) del sensor de línea. Es de Coty, no se tocó.
- `include/ruedas.h` / `src/ruedas.cpp` — control continuo de las ruedas (traslación en X) vía L298N.
- `include/base.h` / `src/base.cpp` — control no bloqueante de la base giratoria (orienta el brazo a un lado), por trenes de pulsos con freno activo — misma lógica ya probada en `src/prueba.cpp`.
- `include/servo_brazo.h` / `src/servo_brazo.cpp` — movimiento gradual no bloqueante del servo del brazo (el que abre el cajón) — misma lógica ya probada en `src/prueba.cpp`.
- `include/modo.h` / `src/modo.cpp` — modo de operación a nivel de todo el sistema (`MODO_MANUAL` / `MODO_AUTONOMO`), separado del estado interno de la rutina autónoma.
- `include/maquina_estados.h` / `src/maquina_estados.cpp` — la máquina de estados de la rutina autónoma (ver [diagrama-flujo-estados.md](../docs/diagrama-flujo-estados.md)). Expone la API pública: `abrirCajon(lado, n)`, `girarBase(lado)`, `irAHome()`, `parar()`, `estado()`.
- `include/servidor_web.h` / `src/servidor_web.cpp` — Access Point propio (`CajonesBot`) + página del modo manual + endpoint `/pedido?columna=N&lado=izq|der` (ver [modo-manual.md](../docs/modo-manual.md)).

## Cómo se reparte el loop entre modo manual y modo autónomo

`main.cpp` llama siempre a `servidorWeb_actualizar()`, `base_actualizar()` y `servoBrazo_actualizar()` — estas dos últimas tienen que correr siempre, sin importar el modo, porque si no un pulso manual de la base o un movimiento manual del servo se quedarían a mitad de camino. Después:

- Si el sistema está en `MODO_AUTONOMO`, o en `ESTADO_ERROR_SEGURO` (para poder salir de la parada de emergencia sin importar el modo), corre `maquinaEstados_actualizar()`.
- Si no, el sistema está en modo manual idle: ruedas/base/servo se controlan directo desde los handlers de `servidor_web.cpp` (`/manual/ruedas`, `/manual/base`, `/manual/servo`).

Al terminar la rutina autónoma, la máquina de estados vuelve sola a `MODO_MANUAL` — no hace falta ninguna acción extra del operador.

## Parada de emergencia

**No hay botón físico dedicado todavía** (ver [pendientes.md](../docs/pendientes.md)) — por ahora es el comando Serial `x`, igual que en `src/prueba.cpp`, procesado sin esperar Enter para que sea inmediato. A propósito **no está expuesta en la web**: un mecanismo de parada segura no debería depender de que el WiFi o el software estén funcionando. `irAHome()` hace de reset manual después de una parada (no hay botón dedicado para eso tampoco).

## Nota de diseño: conteo de marcas en X, fases restantes por tiempo

El robot se mueve libremente sobre ruedas, así que saber "a qué columna llegó" requiere contar flancos del sensor de línea mientras se mueve (`MOVIENDO_X`), no leer un booleano una sola vez — y lo mismo para la vuelta (`VOLVIENDO_HOME`), contando en sentido inverso hasta 0. Las demás fases (orientar la base, extender, abrir, sidestep) se resuelven por **tiempo fijo**, con las constantes ya calibradas a mano en `src/prueba.cpp` — ver [diagrama-flujo-estados.md](../docs/diagrama-flujo-estados.md).

## Cómo probarlo en el banco

1. Cablear según [pines.md](pines.md).
2. `pio run -e nodemcu-32s -t upload --upload-port <puerto>`, cargar a la placa.
3. `pio device monitor -e nodemcu-32s` (115200 baudios): imprime cada cambio de estado y de modo.
4. Conectarse desde el celular/notebook a la red WiFi `CajonesBot` (ver clave en [pines.md](pines.md)) y abrir `http://192.168.4.1` — ahí se puede mover ruedas/base/servo manualmente y pedir una columna/lado.
5. Comando `x` por Serial: parada de emergencia.

Problemas conocidos al subir/grabar (cable USB, modo BOOT, servo que reinicia la placa, etc.): ver la guía de Coty en [`referencias/`](../referencias/), siguen aplicando igual acá.

## Tests automáticos (sin placa)

Además de la prueba física de arriba, `../test/` tiene tests que corren en la compu, sin ESP32 ni servo real, compilando los `.cpp` de producción tal cual con mocks de las primitivas de Arduino, de `ESP32Servo` y de `Preferences`. Sirven para validar la lógica de la FSM (secuencia completa, conteo de marcas, prioridad de la emergencia, control manual de la base, pedidos ignorados mientras hay una rutina en curso) antes de tocar la placa. Correrlos con:

```
bash test/run_tests.sh
```

Ver [test/README.md](../test/README.md) para el detalle.

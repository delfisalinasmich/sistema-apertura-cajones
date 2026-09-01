# Firmware

Todavía sin implementar. Esta carpeta va a contener el código del ESP32 una vez que esté definida la electrónica (ver [pendientes.md](../docs/pendientes.md)).

## Organización de módulos prevista

- `main.ino` — `setup()` / `loop()`, arranca todo.
- `motores.h` / `.cpp` — control de los 2 motores (riel X, vástago), con límites de seguridad.
- `sensores.h` / `.cpp` — lectura con antirrebote de los fines de carrera (y opcionalmente del sensor del imán).
- `maquina_estados.h` / `.cpp` — la máquina de estados de la rutina autónoma (ver [diagrama-flujo-estados.md](../docs/diagrama-flujo-estados.md)).
- `servidor_web.h` / `.cpp` — página del modo manual + endpoint `/pedido?columna=N` (ver [modo-manual.md](../docs/modo-manual.md)).
- `seguridad.h` / `.cpp` — botón de emergencia por interrupción de hardware, corta motores por relé además de por software.

## Por qué se puede empezar antes de tener el hardware armado

La máquina de estados se puede programar y probar ya mismo, sin el brazo físico: reemplazando los fines de carrera por botones de prueba y los motores por LEDs. Cuando llegue el hardware real, solo cambian las funciones de `motores.*` y `sensores.*`, no la lógica de `maquina_estados.*`.

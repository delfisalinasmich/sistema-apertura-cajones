# Firmware

Sketch de banco (`src/`) que corre la máquina de estados de la rutina autónoma en una placa **ESP32 dev board** real, simulando los motores con LEDs y los fines de carrera con botones — para validar la lógica antes de tener el brazo físico armado. El mapeo de pines está en [pines.md](pines.md).

`servidor_web` (modo manual + endpoint `/pedido`) todavía no está implementado — ver [pendientes.md](../docs/pendientes.md).

## Organización de módulos

- `src/src.ino` — `setup()` / `loop()`, arranca todo. (El archivo se llama `src.ino` porque Arduino IDE/CLI exige que el `.ino` principal tenga el mismo nombre que su carpeta contenedora, `src/`.)
- `src/pines.h` — mapeo de pines del sketch de banco.
- `src/motores.h` / `.cpp` — hoy son 2 LEDs (`motores_moverX()`, `motores_moverVastago()`, `motores_pararTodos()`). Se reemplaza por el control real del driver de cada motor cuando exista la electrónica, sin tocar `maquina_estados.cpp`.
- `src/sensores.h` / `.cpp` — lectura con antirrebote (no bloqueante) de los 2 fines de carrera confirmados como mínimo (`docs/sensores-actuadores.md`) y de los botones de pedido/emergencia.
- `src/maquina_estados.h` / `.cpp` — la máquina de estados de 8 estados de la rutina autónoma (ver [diagrama-flujo-estados.md](../docs/diagrama-flujo-estados.md)).
- `src/seguridad.h` / `.cpp` — chequeo de emergencia con prioridad sobre el resto de la máquina de estados; hoy corta los LEDs, se reemplaza por el corte real (relé) cuando exista la electrónica.
- `src/servidor_web.h` / `.cpp` — **pendiente**: página del modo manual + endpoint `/pedido?columna=N` (ver [modo-manual.md](../docs/modo-manual.md)). Hoy ese pedido se simula con un botón físico (`BTN_PEDIDO`, ver `pines.md`).

## Nota de diseño: sensores reutilizados en el sketch de banco

El diseño documentado en `docs/diagrama-flujo-estados.md` menciona un "fin de carrera de reposo en Z" para `RETRAYENDO_Z` y una confirmación de posición "home" para `VOLVIENDO_HOME`, pero `docs/sensores-actuadores.md` solo confirma 2 sensores como mínimo (fin de carrera X y fin de carrera de la transición de la ranura). Para no inventar hardware que la cátedra no pidió, el sketch de banco resuelve estas dos fases por **tiempo** (consistente con el fail-safe por gravedad ya documentado para volver a reposo) en vez de por sensor. Si el grupo termina agregando sensores dedicados en el diseño eléctrico final, alcanza con reemplazar esas comparaciones de tiempo por lecturas de `sensores.*` — la lógica del resto de la máquina de estados no cambia.

## Cómo probarlo

1. Cablear en protoboard según [pines.md](pines.md): 3 LEDs (motor X, motor vástago, error) + 4 botones (fin de carrera X, fin de carrera transición, pedido, emergencia).
2. Abrir `src/src.ino` en Arduino IDE (o compilar con `arduino-cli compile --fqbn esp32:esp32:esp32 firmware/src`), cargar a la placa.
3. Abrir el monitor serie (115200 baudios): cada cambio de estado se imprime por consola.
4. Apretar `BTN_PEDIDO` para arrancar la rutina, `BTN_FIN_CARRERA_X` y `BTN_FIN_CARRERA_TRANSICION` para avanzar las fases correspondientes, y `BTN_EMERGENCIA` para probar la parada segura (se sale con `BTN_PEDIDO` mientras no esté presionada la emergencia).

# Pendientes

Lista de lo que falta decidir antes de poder cerrar la propuesta inicial y empezar la construcción.

## Grupo / cátedra

- [ ] Nombres de los integrantes del grupo.
- [ ] Confirmar el microcontrolador (ESP32 propuesto).
- [ ] Lista de materiales y presupuesto.
- [ ] Restricciones de tamaño, tiempo de impresión y materiales indicadas por la cátedra para la pieza 3D.

## Diseño mecánico

- [ ] Dimensionamiento del imán y radio de la excéntrica (a calibrar con prototipo).
- [ ] Tipo de motor a usar en cada eje (paso a paso, DC + reductora, o servo) — condiciona el driver y la fuente.
- [ ] Diseño detallado de la ranura bayoneta (tolerancias) — es el punto crítico de fabricación, ver [mecanismo-trabas.md](mecanismo-trabas.md).

## Electrónica

- [ ] Elección del driver de motor para cada eje.
- [ ] Dimensionamiento de la fuente de alimentación.
- [ ] Diagrama de conexiones eléctricas definitivo (falta elegir componentes primero).

## Software (a cargo de Delfina)

- [x] Diseño visual/UX de la interfaz web del modo manual — versión funcional simple, ver [modo-manual.md](modo-manual.md).
- [x] Definir si el ESP32 actúa como access point o se conecta a una red existente — Access Point propio (`CajonesBot`).
- [x] Máquina de estados de la rutina autónoma + modo manual vía web, simulados con LEDs/botones sobre ESP32 (`firmware/src/`), compilado y verificado con `arduino-cli`.
- [ ] Contraseña definitiva del AP antes de la demo (hoy es un valor de prueba en `firmware/src/servidor_web.cpp`).
- [ ] Reemplazar `motores.*` y `sensores.*` por el control/lectura real cuando exista la electrónica (drivers de motor, fines de carrera reales) — la máquina de estados y el servidor web no deberían necesitar cambios.

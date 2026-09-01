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

- [ ] Diseño visual/UX de la interfaz web del modo manual.
- [ ] Definir si el ESP32 actúa como access point o se conecta a una red existente.
- [ ] Implementación real del firmware (`firmware/`) una vez elegidos los componentes de electrónica.

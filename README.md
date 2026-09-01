# Sistema de apertura automática de cajones

Proyecto final — Seminario de Mecatrónica.
Adaptación del sistema pick-to-light: dado un pedido por software de una pieza almacenada en una cajonera de una sola fila, un brazo se traslada hasta la columna correspondiente y abre el cajón lo suficiente para que la persona lo termine de extraer a mano.

## Estado actual

En etapa de diseño / pre-construcción. La propuesta mecánica y la arquitectura de software están definidas; falta aprobación de la cátedra, fabricación de la pieza crítica (ranura de trabas) y desarrollo del firmware.

## Integrantes

- TODO: completar nombres del grupo

## Resumen técnico

- **3 grados de libertad**: X (traslación del carro sobre riel), Z (extensión/retracción del vástago), rotación (apertura del cajón).
- **2 actuadores**: motor del riel X, motor del vástago (combina extensión y rotación mediante mecanismo de trabas por ranura bayoneta).
- **Sensores**: fin de carrera en X, fin de carrera en la transición extensión→rotación de la ranura, opcional sensor de contacto/corriente en el electroimán.
- **Efector final**: imán de neodimio descentrado (excéntrica) que engancha una placa metálica en el cajón.
- **Microcontrolador**: ESP32 (propuesto, no confirmado por la cátedra).
- **Modo manual**: interfaz web servida por el ESP32.
- **Modo autónomo**: máquina de estados que ejecuta la secuencia reposo → posicionamiento → apertura → despeje → retracción.

## Documentación

- [Propuesta inicial](docs/propuesta-inicial.md)
- [Esquema general y grados de libertad](docs/esquema-general.md)
- [Sensores y actuadores](docs/sensores-actuadores.md)
- [Mecanismo de trabas](docs/mecanismo-trabas.md)
- [Diagrama de bloques](docs/diagrama-bloques.md)
- [Diagrama de flujo / estados](docs/diagrama-flujo-estados.md)
- [Modo manual](docs/modo-manual.md)
- [Pieza impresa en 3D](docs/pieza-3d.md)
- [Pendientes](docs/pendientes.md)

## Estructura del repo

```
docs/         documentación del proyecto
firmware/     código del microcontrolador (todavía sin implementar)
cad/          archivos CAD/STL de la pieza impresa
referencias/  consigna de la cátedra y primer diseño del grupo
```

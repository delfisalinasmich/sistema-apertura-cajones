# Sistema de apertura automática de cajones

Proyecto final — Seminario de Mecatrónica.
Adaptación del sistema pick-to-light: dado un pedido por software de una pieza almacenada en una cajonera (varias líneas y columnas), un robot se traslada hasta la columna correspondiente, se orienta hacia la línea correspondiente, y abre el cajón lo suficiente para que la persona lo termine de extraer a mano.

## Estado actual

En armado/integración. El diseño mecánico y el firmware (máquina de estados, modo manual vía web, tests automáticos) están implementados y compilan/corren en verde; falta aprobación de la cátedra, terminar el cableado real de la electrónica y calibrar en el banco (ver [pendientes.md](docs/pendientes.md)).

## Integrantes

- TODO: completar nombres del grupo

## Resumen técnico

- **3 grados de libertad**: X (traslación horizontal del robot sobre ruedas), rotación de línea (servo que orienta el brazo hacia una de las líneas de cajones), Z (extensión/retracción del brazo adelante-atrás).
- **3 actuadores**: motor X (ruedas, vía driver L298N), servo de orientación de línea, motor Z (brazo, vía driver L298N) — el mismo motor Z abre el cajón al retroceder.
- **Sensores**: óptico de columna (IR), que el firmware usa contando marcas para saber a qué columna llegó el robot; opcional sensor de contacto/corriente en el imán.
- **Efector final**: imán de neodimio en la punta del brazo, que engancha una placa metálica en el cajón.
- **Microcontrolador**: ESP32 (propuesto, no confirmado por la cátedra).
- **Modo manual**: interfaz web servida por el ESP32 (Access Point propio `CajonesBot`).
- **Modo autónomo**: máquina de estados que ejecuta la secuencia reposo → traslado en X → orientación de línea → extensión → apertura → sidestep → vuelta a home.

## Documentación

- [Propuesta inicial](docs/propuesta-inicial.md)
- [Esquema general y grados de libertad](docs/esquema-general.md)
- [Sensores y actuadores](docs/sensores-actuadores.md)
- [Mecanismo de apertura y cambio de línea](docs/mecanismo-apertura.md)
- [Diagrama de bloques](docs/diagrama-bloques.md)
- [Diagrama de flujo / estados](docs/diagrama-flujo-estados.md)
- [Modo manual](docs/modo-manual.md)
- [Pieza impresa en 3D](docs/pieza-3d.md)
- [Pendientes](docs/pendientes.md)

## Estructura del repo

```
docs/         documentación del proyecto
firmware/     código del microcontrolador (máquina de estados, modo manual vía web)
test/         tests automáticos de la máquina de estados, sin necesidad de placa
cad/          archivos CAD/STL de la pieza impresa
referencias/  consigna de la cátedra y primer diseño del grupo
```

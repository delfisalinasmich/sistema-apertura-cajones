# Sistema de apertura automática de cajones

Proyecto final — Seminario de Mecatrónica.
Adaptación del sistema pick-to-light: dado un pedido por software de una pieza almacenada en una cajonera de 3×3 cajones, un robot se traslada hasta la columna correspondiente, orienta el brazo hacia el lado (izquierda/derecha) donde está el cajón pedido, y lo abre lo suficiente para que la persona lo termine de extraer a mano.

## Estado actual

El hardware está armado y parcialmente probado (Coty armó el robot y una secuencia mecánica funcional — base + servo + imán — ver [`referencias/`](referencias/)). El firmware está migrado a PlatformIO sobre ese hardware real: máquina de estados no bloqueante con API pública, modo manual vía web, y tests automáticos — los 4 entornos compilan limpio y los tests nativos están en verde. Falta aprobación de la cátedra, resolver pendientes de hardware (resistencia del sensor, asimetría de la base, el imán no se suelta solo) y calibrar la secuencia completa en el banco (ver [pendientes.md](docs/pendientes.md)).

## Integrantes

- TODO: completar nombres del grupo (incluye a Coty, que armó el hardware y el firmware base)

## Resumen técnico

- **3 grados de libertad**: X (traslación horizontal del robot sobre ruedas), lado (base giratoria que orienta el brazo hacia izquierda o derecha), Z (avance/retroceso del brazo con el servo).
- **3 actuadores**: motor X (ruedas, vía driver L298N), base giratoria (motor DC + reductora, vía L298N, por pulsos de tiempo), micro servo del brazo — el mismo servo abre el cajón al retroceder.
- **Sensor**: IR TCRT5000 analógico, calibrado en flash, que el firmware usa contando franjas para saber a qué columna llegó el robot.
- **Efector final**: imán de neodimio en la punta del brazo, que engancha una tuerca metálica en el frente de cada cajón.
- **Microcontrolador**: ESP32 NodeMCU-32S.
- **Modo manual**: interfaz web servida por el ESP32 (Access Point propio `CajonesBot`).
- **Modo autónomo**: máquina de estados que ejecuta la secuencia reposo → traslado en X → orientación de la base → extensión → apertura → vuelta de la base → sidestep → vuelta a home.

## Documentación

- [Propuesta inicial](docs/propuesta-inicial.md)
- [Esquema general y grados de libertad](docs/esquema-general.md)
- [Sensores y actuadores](docs/sensores-actuadores.md)
- [Mecanismo de apertura y orientación](docs/mecanismo-apertura.md)
- [Diagrama de bloques](docs/diagrama-bloques.md)
- [Diagrama de flujo / estados](docs/diagrama-flujo-estados.md)
- [Modo manual](docs/modo-manual.md)
- [Pieza impresa en 3D](docs/pieza-3d.md)
- [Pendientes](docs/pendientes.md)

## Estructura del repo

```
docs/         documentación del proyecto
firmware/     proyecto PlatformIO (máquina de estados, modo manual vía web, entornos de banco)
test/         tests automáticos de la máquina de estados, sin necesidad de placa
cad/          archivos CAD/STL de la pieza impresa
referencias/  consigna de la cátedra, primer diseño del grupo, guía del hardware armado por Coty
```

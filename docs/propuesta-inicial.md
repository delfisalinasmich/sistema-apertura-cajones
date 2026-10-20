# Propuesta inicial

Checklist según lo pedido por la cátedra (sección "Propuesta inicial" de la consigna) para aprobación previa a la etapa de construcción.

## Nombre del proyecto

Sistema de apertura automática de cajones

## Integrantes

TODO: completar nombres del grupo

## Descripción general

Adaptación del sistema pick-to-light: dada una solicitud por software de una pieza almacenada en una cajonera (varias líneas y columnas, piezas chicas y livianas, tipo laboratorio), el sistema traslada un brazo hasta la columna y línea correspondientes y abre el cajón lo suficiente para que la persona lo vea y lo termine de extraer a mano.

## Objetivo del robot

Recibir un pedido (columna/línea), posicionarse automáticamente frente al cajón y abrirlo mediante un mecanismo de enganche magnético, sin intervención manual continua.

## Actuadores a utilizar

- Motor X (ruedas, vía driver L298N) — traslación horizontal del robot, selección de columna.
- Servo de orientación — gira el brazo para alcanzar la línea de cajones pedida.
- Motor Z (brazo, vía driver L298N) — extensión/retracción adelante-atrás; al retroceder, abre el cajón (ver [mecanismo-apertura.md](mecanismo-apertura.md)).

## Sensores a utilizar

- Óptico de columna (IR): confirma, contando marcas, a qué columna llegó el robot.
- Opcional: sensor de contacto o de corriente en el imán (confirma el enganche antes de abrir).

## Microcontrolador seleccionado

ESP32 (propuesto, dado el trabajo previo del grupo con el sistema pick-to-light y la necesidad de WiFi para la interfaz web del modo manual). TODO: confirmar con la cátedra.

## Grados de libertad previstos

3, todos actuados:
1. **X** — traslación horizontal del robot sobre ruedas (selecciona columna).
2. **Rotación de línea** — giro del brazo con el servo (selecciona línea).
3. **Z** — extensión/retracción del brazo (abre el cajón al retroceder).

Ver detalle y justificación en [esquema-general.md](esquema-general.md).

## Descripción del modo manual

Interfaz web servida por el propio ESP32, con controles para mover el robot en X, mover el brazo en Z, orientar el servo a una línea, y disparar manualmente el pedido de una columna/línea (simulando al sistema pick-to-light). Detalle en [modo-manual.md](modo-manual.md).

## Descripción de la rutina autónoma

Secuencia disparada por un pedido de software (columna N, línea M):
1. Reposo → traslado en X hasta la columna N.
2. Orientación: el servo gira el brazo hasta la línea M.
3. Extensión en Z: el brazo avanza hasta que el imán hace contacto con la placa del cajón.
4. Apertura: el mismo motor Z retrocede, tira del cajón y lo abre (y de paso vuelve el brazo a su posición retraída).
5. Sidestep en X (para no empujar el cajón de nuevo al retirarse) + vuelta a home.

Detalle en [diagrama-flujo-estados.md](diagrama-flujo-estados.md).

## Pieza que se diseñará para impresión 3D

Soporte del brazo (motor Z + extremo con el imán), montado sobre el eje del servo de orientación. Ver [pieza-3d.md](pieza-3d.md).

## Lista preliminar de materiales

TODO: completar (motor de ruedas, motor del brazo, servo, driver(s) L298N, ESP32, sensor óptico, imán de neodimio, chasis/ruedas, tornillería, filamento de impresión 3D).

## Principales riesgos o dificultades esperadas

- La fuerza del imán hay que calibrarla con pruebas reales (tamaño del imán, con el cajón cargado con la pieza más pesada prevista) para que enganche bien pero también se despegue sin forzarlo.
- El sensor óptico de columna depende de que las marcas estén bien espaciadas y sean consistentes — si el robot pierde una marca por velocidad o iluminación, el conteo de columna queda desalineado. Conviene probar el conteo en el banco antes de confiar en él para la demo.
- Confirmar si el driver L298N elegido alcanza para mover ambos motores (ruedas y brazo) con margen, o si hace falta uno por eje.

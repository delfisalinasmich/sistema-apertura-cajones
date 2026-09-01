# Propuesta inicial

Checklist según lo pedido por la cátedra (sección "Propuesta inicial" de la consigna) para aprobación previa a la etapa de construcción.

## Nombre del proyecto

Sistema de apertura automática de cajones

## Integrantes

TODO: completar nombres del grupo

## Descripción general

Adaptación del sistema pick-to-light: dada una solicitud por software de una pieza almacenada en una cajonera de una sola fila (piezas chicas y livianas, tipo laboratorio), el sistema traslada un brazo hasta la columna correspondiente y abre el cajón lo suficiente para que la persona lo vea y lo termine de extraer a mano.

## Objetivo del robot

Recibir un pedido (columna/cajón), posicionarse automáticamente frente a él y abrirlo mediante un mecanismo de enganche magnético, sin intervención manual continua.

## Actuadores a utilizar

- Motor para el riel X (traslación del carro, selección de columna).
- Motor para el vástago (combina extensión/retracción en Z y rotación de apertura mediante mecanismo de trabas por ranura bayoneta).

## Sensores a utilizar

- Fin de carrera en el riel X (confirma la columna correcta).
- Fin de carrera / sensor óptico en la ranura de trabas (confirma la transición extensión → rotación).
- Opcional: sensor de contacto o de corriente en el electroimán (confirma el enganche antes de rotar).

## Microcontrolador seleccionado

ESP32 (propuesto, dado el trabajo previo del grupo con el sistema pick-to-light y la necesidad de WiFi para la interfaz web del modo manual). TODO: confirmar con la cátedra.

## Grados de libertad previstos

3, todos actuados:
1. **X** — traslación del carro sobre el riel (selecciona columna).
2. **Z** — extensión/retracción del vástago.
3. **Rotación** — giro del brazo en la punta del vástago (abre el cajón).

Ver detalle y justificación en [esquema-general.md](esquema-general.md).

## Descripción del modo manual

Interfaz web servida por el propio ESP32, con controles para mover el carro en X, extender/retraer el vástago, y disparar manualmente el pedido de una columna (simulando al sistema pick-to-light). Detalle en [modo-manual.md](modo-manual.md).

## Descripción de la rutina autónoma

Secuencia de 4 fases disparada por un pedido de software (columna N):
1. Reposo → posicionamiento en X.
2. Extensión en Z hasta la altura de la fila de cajones.
3. Rotación del brazo: el imán excéntrico engancha la placa del cajón y lo abre en un arco corto.
4. Sidestep en X (para no empujar el cajón de nuevo al retirarse) + retracción en Z + vuelta a home.

Detalle en [diagrama-flujo-estados.md](diagrama-flujo-estados.md).

## Pieza que se diseñará para impresión 3D

Soporte del pivote con la ranura de trabas (mecanismo bayoneta que combina extensión y rotación con un solo motor) + excéntrica del imán. Ver [pieza-3d.md](pieza-3d.md).

## Lista preliminar de materiales

TODO: completar (motores, driver de motor, ESP32, fines de carrera, imán de neodimio, perfiles/riel, tornillería, filamento de impresión 3D).

## Principales riesgos o dificultades esperadas

- La pieza de la ranura de trabas es la de mayor tolerancia de todo el proyecto: es esperable iterar varias veces en impresión 3D (FDM) hasta que el pasador no juegue ni se trabe. Conviene prototiparla sola antes de integrar el resto del brazo.
- La transición extensión → rotación no cae siempre en el mismo paso del motor (fricción, desgaste), por lo que depende de un sensor físico y no de contar pasos.
- La fuerza del imán hay que calibrarla con pruebas reales (tamaño del imán, con el cajón cargado con la pieza más pesada prevista) para que enganche bien pero también se despegue solo al final del arco de rotación.

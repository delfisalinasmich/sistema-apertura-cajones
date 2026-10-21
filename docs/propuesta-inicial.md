# Propuesta inicial

Checklist según lo pedido por la cátedra (sección "Propuesta inicial" de la consigna) para aprobación previa a la etapa de construcción.

## Nombre del proyecto

Sistema de apertura automática de cajones

## Integrantes

TODO: completar nombres del grupo (incluye a Coty, que armó el hardware y el firmware base)

## Descripción general

Adaptación del sistema pick-to-light: dada una solicitud por software de una pieza almacenada en una cajonera de 3×3 cajones, el sistema traslada un robot hasta la columna correspondiente, orienta el brazo hacia el lado (izquierda/derecha) donde está el cajón pedido, y lo abre lo suficiente para que la persona lo vea y lo termine de extraer a mano.

## Objetivo del robot

Recibir un pedido (columna/lado), posicionarse automáticamente frente al cajón y abrirlo mediante un mecanismo de enganche magnético, sin intervención manual continua.

## Actuadores a utilizar

- Motor X (ruedas, vía driver L298N) — traslación horizontal del robot, selección de columna.
- Base giratoria (motor DC + reductora, vía L298N, por pulsos de tiempo) — orienta el brazo hacia el lado pedido.
- Micro servo del brazo — avanza hasta que el imán toca la tuerca del cajón; al retroceder, abre el cajón (ver [mecanismo-apertura.md](mecanismo-apertura.md)).

## Sensores a utilizar

- IR TCRT5000 (analógico, calibrado en flash): confirma, contando franjas negras, a qué columna llegó el robot.
- Opcional, no implementado: sensor de contacto o de corriente en el imán (confirmaría el enganche antes de abrir).

## Microcontrolador seleccionado

ESP32 NodeMCU-32S (ya elegido y usado para el hardware armado). TODO: confirmar con la cátedra.

## Grados de libertad previstos

3, todos actuados:
1. **X** — traslación horizontal del robot sobre ruedas (selecciona columna).
2. **Lado** — giro de la base hacia izquierda o derecha.
3. **Z** — avance/retroceso del brazo con el servo (abre el cajón al retroceder).

Ver detalle y justificación en [esquema-general.md](esquema-general.md).

## Descripción del modo manual

Interfaz web servida por el propio ESP32, con controles para mover el robot en X, girar la base un pulso, mover el servo a un ángulo directo, y disparar manualmente el pedido de una columna/lado (simulando al sistema pick-to-light). Detalle en [modo-manual.md](modo-manual.md).

## Descripción de la rutina autónoma

Secuencia disparada por un pedido de software (columna N, lado izquierda/derecha):
1. Reposo → traslado en X hasta la columna N (contando franjas del sensor de línea).
2. Orientación: la base gira hacia el lado pedido (tren de pulsos calibrado).
3. Extensión: el servo avanza el brazo hasta que el imán hace contacto con la tuerca del cajón.
4. Apertura: el mismo servo retrocede despacio, tira del cajón y lo abre (y de paso vuelve el brazo a su posición de reposo).
5. La base vuelve de lado + sidestep en X (para no empujar el cajón de nuevo al retirarse) + vuelta a home (contando franjas en sentido inverso).

Detalle en [diagrama-flujo-estados.md](diagrama-flujo-estados.md).

## Pieza que se diseñará para impresión 3D

Soporte del brazo (servo + extremo con el imán), montado sobre el eje de la base giratoria. Ver [pieza-3d.md](pieza-3d.md).

## Lista preliminar de materiales

Ya definidos por el hardware armado: ESP32 NodeMCU-32S, driver L298N, motor TT con reductora (ruedas), motor DC con reductora (base giratoria), micro servo SG90, sensor IR TCRT5000, imán de neodimio. TODO: completar (chasis/ruedas, tornillería, filamento de impresión 3D, presupuesto).

## Principales riesgos o dificultades esperadas

- La calibración de pulsos de la base solo se probó yendo hacia un lado (a propósito, para no arriesgar que se enreden los cables) — abrir cajones del lado opuesto queda fuera de alcance por ahora.
- Mapeo de pines base/ruedas con una contradicción sin resolver entre el código de `prueba.cpp` y la confirmación de Coty — pendiente un chequeo visual en el banco (ver [pendientes.md](pendientes.md)).
- El sensor de columna depende de que las franjas estén bien espaciadas y sean consistentes — si el robot pierde una marca por velocidad o iluminación, el conteo de columna queda desalineado.

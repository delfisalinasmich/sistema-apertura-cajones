# Esquema general y grados de libertad

## Objetivo

Diseñar un mecanismo que, dada una solicitud por software de una pieza almacenada en una cajonera (varias líneas y columnas, piezas chicas y livianas, tipo laboratorio), traslade un brazo hasta la columna y línea correspondientes y abra el cajón lo suficiente para que la persona lo vea y lo termine de extraer a mano.

## Arquitectura general — 3 grados de libertad

La cátedra exige un mínimo de 3 GDL actuados y justificados con esquema. El diseño los cubre así:

| GDL | Descripción | Actuado por |
|---|---|---|
| X | Traslación horizontal del robot móvil (2 ruedas). Selecciona la columna, y también se reutiliza para despejarse del cajón abierto (sidestep) y volver a home. | Motor X (ruedas, vía driver L298N) |
| Rotación de línea | Giro del brazo para alcanzar una de las líneas (filas) de cajones. | Servo de orientación |
| Z | Extensión/retracción recta del brazo adelante/atrás. Avanza para enganchar el imán con la placa del cajón, y al retroceder tira y lo abre. | Motor Z (brazo, vía driver L298N) |

Los 3 GDL están directamente actuados, cada uno con su propio motor: no hay grados de libertad "pasivos" resultantes de la configuración mecánica (a diferencia de, por ejemplo, un robot móvil diferencial cuya orientación resulta indirectamente de dos motores de tracción).

## Actuadores

3 motores en total:
- Motor X (ruedas) — traslación horizontal del robot.
- Servo de orientación — selecciona la línea de cajones.
- Motor Z (brazo) — extensión/retracción; el mismo movimiento de retroceso es el que abre el cajón (ver [mecanismo-apertura.md](mecanismo-apertura.md)).

Cumple con margen el mínimo de 2 actuadores pedido por la cátedra.

## Secuencia de funcionamiento

- **A. Reposo** — brazo retraído, servo en la línea home. Posición segura por defecto.
- **B. Traslado en X** — el robot se mueve hasta la columna pedida, contando las marcas del sensor óptico de columna.
- **C. Orientación de línea** — el servo gira el brazo hasta la línea pedida.
- **D. Extensión** — el motor Z avanza hasta que el imán hace contacto con la placa del cajón.
- **E. Apertura** — el mismo motor Z retrocede: tira del cajón y lo abre, y de paso vuelve el brazo a su posición retraída (no hace falta una fase separada de retracción).
- **F. Sidestep + vuelta a home** — el robot se desplaza en X hacia una zona sin cajón abierto en el medio, y desde ahí vuelve a la posición de reposo/home.

Ver el detalle como máquina de estados en [diagrama-flujo-estados.md](diagrama-flujo-estados.md).

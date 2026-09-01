# Esquema general y grados de libertad

## Objetivo

Diseñar un mecanismo que, dada una solicitud por software de una pieza almacenada en una cajonera de una sola fila (piezas chicas y livianas, tipo laboratorio), traslade un brazo hasta la columna correspondiente y abra el cajón lo suficiente para que la persona lo vea y lo termine de extraer a mano.

## Arquitectura general — 3 grados de libertad

La cátedra exige un mínimo de 3 GDL actuados y justificados con esquema. El diseño los cubre así:

| GDL | Descripción | Actuado por |
|---|---|---|
| X | Traslación del carro sobre un riel ubicado por debajo de la cajonera. Selecciona la columna, y también se reutiliza para despejar el brazo al guardarse (sidestep). | Motor del riel X |
| Z | Extensión/retracción recta del vástago. Lo acerca a la altura de la fila de cajones. | Motor del vástago (fase de traslación, ver [mecanismo-trabas.md](mecanismo-trabas.md)) |
| Rotación | Giro del brazo en la punta del vástago. Es lo que efectivamente abre el cajón. | Mismo motor del vástago (fase de rotación) |

Los 3 GDL están directamente actuados: no hay grados de libertad "pasivos" resultantes de la configuración mecánica en este diseño (a diferencia de, por ejemplo, un robot móvil diferencial cuya orientación resulta indirectamente de dos motores de tracción).

## Actuadores

2 motores en total:
- Motor riel X.
- Motor del vástago, que combina extensión (Z) y rotación mediante el mecanismo de trabas descripto en [mecanismo-trabas.md](mecanismo-trabas.md).

Cumple el mínimo de 2 actuadores pedido por la cátedra, con margen (2 motores producen 3 GDL).

## Secuencia de funcionamiento

- **A. Reposo** — vástago retraído, brazo plegado colgando bajo el riel. Posición segura por defecto: si se corta la energía, el brazo cae solo a esta posición (fail-safe por gravedad).
- **B. Extensión** — el carro se ubica en la columna pedida (X) y el vástago sube en línea recta (Z) hasta quedar a la altura de la fila de cajones.
- **C. Rotación / apertura** — el motor rota el brazo. El imán, montado descentrado respecto del eje (tipo excéntrica), toca la placa metálica del cajón y lo tira hacia afuera en un arco corto — solo lo necesario para destrabarlo y que se vea abierto.
- **D. Sidestep + retracción** — en vez de invertir la rotación (lo que empujaría el cajón de nuevo hacia adentro), el carro se desplaza sobre el eje X hacia una zona sin cajón abierto en el medio, y desde ahí el vástago se retrae en línea recta hacia abajo. Después vuelve por X a la posición de reposo/home.

Ver el detalle como máquina de estados en [diagrama-flujo-estados.md](diagrama-flujo-estados.md).

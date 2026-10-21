# Esquema general y grados de libertad

## Objetivo

Diseñar un mecanismo que, dada una solicitud por software de una pieza almacenada en una cajonera de 3 cajones (ver [referencias/Guía del proyecto - Coty.md](../referencias/Guía%20del%20proyecto%20-%20Coty.md)), traslade un brazo hasta la columna correspondiente, lo oriente hacia el lado (izquierda/derecha) donde está el cajón pedido, y lo abra lo suficiente para que la persona lo vea y lo termine de extraer a mano. Confirmado por Coty: la columna se selecciona con la traslación en X + el sensor de línea, y no hay filas adicionales — el "lado" (base giratoria) ya cubre el tercer GDL.

## Arquitectura general — 3 grados de libertad

La cátedra exige un mínimo de 3 GDL actuados y justificados con esquema. El diseño los cubre así:

| GDL | Descripción | Actuado por |
|---|---|---|
| X | Traslación horizontal del robot móvil (ruedas). Selecciona la columna; también se reutiliza para despejarse del cajón abierto (sidestep) y volver a home. | Motor X (ruedas, vía driver L298N) |
| Lado | Giro de la base hacia el lado izquierdo o derecho del recorrido, donde están las filas de cajones. | Base giratoria (motor DC + reductora, vía L298N, por pulsos de tiempo, con topes mecánicos) |
| Z | Avance/retroceso del brazo con el imán en la punta. Avanza hasta que el imán toca la tuerca metálica del cajón; al retroceder, tira y lo abre. | Micro servo del brazo |

Los 3 GDL están directamente actuados, cada uno con su propio actuador: no hay grados de libertad "pasivos" resultantes de la configuración mecánica.

## Actuadores

3 en total:
- Motor X (ruedas) — traslación horizontal del robot.
- Base giratoria (motor DC + reductora) — selecciona el lado.
- Servo del brazo — avanza/retrocede; el mismo movimiento de retroceso es el que abre el cajón (ver [mecanismo-apertura.md](mecanismo-apertura.md)).

Cumple con margen el mínimo de 2 actuadores pedido por la cátedra.

## Secuencia de funcionamiento

- **A. Reposo** — brazo retraído (servo en la posición de reposo), base sin girar. Posición segura por defecto.
- **B. Traslado en X** — el robot avanza hasta la columna pedida, contando las franjas negras del sensor de línea.
- **C. Orientación de la base** — un tren de pulsos gira la base hacia el lado pedido.
- **D. Extensión** — el servo avanza el brazo hasta que el imán hace contacto con la tuerca del cajón.
- **E. Apertura** — el mismo servo retrocede despacio: tira del cajón y lo abre, y de paso vuelve el brazo a su posición de reposo (no hace falta una fase separada de retracción).
- **F. Vuelta de la base + sidestep + vuelta a home** — la base gira de vuelta, el robot se desplaza en X a una zona sin cajón abierto en el medio, y desde ahí vuelve a la posición de reposo/home contando franjas en sentido inverso.

Ver el detalle como máquina de estados en [diagrama-flujo-estados.md](diagrama-flujo-estados.md).

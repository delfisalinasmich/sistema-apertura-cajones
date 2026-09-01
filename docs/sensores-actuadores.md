# Sensores y actuadores

## Actuadores

| Actuador | Función |
|---|---|
| Motor riel X | Traslada el carro para seleccionar la columna; también ejecuta el sidestep de despeje. |
| Motor del vástago | Combina extensión/retracción (Z) y rotación de apertura mediante el mecanismo de trabas por ranura bayoneta. |

## Sensores

| Sensor | Ubicación | Qué confirma | Decisión que dispara |
|---|---|---|---|
| Fin de carrera X | Riel X | Que el carro llegó a la columna pedida | Si no está en posición, no se habilita la fase de extensión (B) |
| Fin de carrera / óptico en la ranura de trabas | Punto de transición extensión → rotación del vástago | Que el pasador terminó el tramo recto y entró al tramo diagonal | Si no se detecta la transición, el firmware no avanza a la fase de rotación (C); evita romper el mecanismo por forzar el giro antes de tiempo |
| Opcional: contacto/corriente en el electroimán | Punta del brazo | Que el imán efectivamente enganchó la placa metálica del cajón | Si no hubo contacto, no se ejecuta la rotación (evita tirar al vacío) |

## Por qué hace falta el sensor de la ranura

El motor del vástago no "sabe" en qué parte de la ranura está solo contando pasos: la transición entre el tramo recto (traslación) y el tramo diagonal (rotación) no cae siempre en el mismo paso exacto, por fricción y desgaste del mecanismo. Por eso la decisión de pasar de extensión a rotación depende de un sensor físico, no de un conteo de pasos — este es el sensor que cumple el requisito de la cátedra de "la lectura del sensor debe modificar de manera observable el comportamiento del robot".

Detalle del mecanismo en [mecanismo-trabas.md](mecanismo-trabas.md).

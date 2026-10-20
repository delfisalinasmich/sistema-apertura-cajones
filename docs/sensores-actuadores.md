# Sensores y actuadores

## Actuadores

| Actuador | Función |
|---|---|
| Motor X (ruedas) | Traslada el robot para seleccionar la columna; también ejecuta el sidestep de despeje y la vuelta a home. |
| Servo de orientación | Gira el brazo para alcanzar la línea de cajones pedida. |
| Motor Z (brazo) | Avanza para enganchar el imán con la placa del cajón; al retroceder tira y abre el cajón (ver [mecanismo-apertura.md](mecanismo-apertura.md)). |

## Sensores

| Sensor | Ubicación | Qué confirma | Decisión que dispara |
|---|---|---|---|
| Óptico de columna (IR) | Debajo del robot, apunta a marcas sobre la base/riel | Que el robot pasó por una marca de columna | El firmware cuenta flancos de esta lectura mientras se mueve en X; cuando el conteo llega a la columna pedida, detiene la traslación y pasa a orientar la línea |
| Opcional: contacto/corriente en el imán | Punta del brazo | Que el imán efectivamente enganchó la placa metálica del cajón | Si no hubo contacto, no se ejecuta la apertura (evita tirar al vacío) |

## Por qué hace falta contar marcas, no solo detectar una

A diferencia de un fin de carrera fijo por columna (como en el diseño anterior de riel+carro), acá el robot se mueve libremente sobre ruedas: una sola lectura booleana del sensor óptico ("hay una marca debajo") no alcanza para saber **a qué columna** llegó. Por eso el firmware cuenta cuántas marcas pasó desde el home y compara contra la columna pedida — este conteo, hecho en software a partir de la lectura del sensor, es la decisión que efectivamente determina el comportamiento del robot (requisito de la cátedra: "la lectura del sensor debe modificar de manera observable el comportamiento del robot").

El cambio de línea no necesita un sensor dedicado: un servo se posiciona solo al ángulo comandado, así que esa fase se resuelve con un tiempo fijo de espera (ver [mecanismo-apertura.md](mecanismo-apertura.md)).

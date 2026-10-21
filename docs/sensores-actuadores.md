# Sensores y actuadores

## Actuadores

| Actuador | Función |
|---|---|
| Motor X (ruedas) | Traslada el robot para seleccionar la columna; también ejecuta el sidestep de despeje y la vuelta a home. |
| Base giratoria | Motor DC + reductora, sin sensor ni encoder, con topes mecánicos a cada lado. Orienta el brazo hacia izquierda o derecha, por trenes de pulsos de tiempo fijo. |
| Servo del brazo | Avanza hasta que el imán hace contacto con la tuerca del cajón; al retroceder tira y lo abre (ver [mecanismo-apertura.md](mecanismo-apertura.md)). |

## Sensores

| Sensor | Ubicación | Qué confirma | Decisión que dispara |
|---|---|---|---|
| IR TCRT5000 (analógico) | Debajo del robot, apunta al piso | Que el robot pasó por una franja negra (marca de columna) | El firmware cuenta flancos de esta lectura mientras se mueve en X (y a la inversa al volver a home); cuando el conteo llega a la columna pedida, detiene la traslación |
| Opcional: contacto/corriente en el imán | Punta del brazo | Que el imán efectivamente enganchó la tuerca del cajón | No implementado — queda como posible mejora futura, no es parte del diseño actual |

## El sensor es analógico, no un módulo digital

A diferencia de lo que se había documentado antes, el sensor de columna es un **TCRT5000 suelto** (LED infrarrojo + fototransistor con resistencias soldadas a mano), leído con `analogRead()` en un pin ADC1 — no un módulo con comparador integrado que dé una salida digital limpia. Por eso hace falta calibración: `include/sensor_linea.h` guarda en flash (`Preferences`) el valor típico sobre piso, sobre la franja negra, el umbral (el promedio entre ambos) y la polaridad, y aplica un margen de histéresis para no generar lecturas falsas cerca del umbral. Se calibra con el entorno `calibracion` del firmware (comandos `w`, `b`, `r`, `l`, `s`).

## Por qué hace falta contar marcas, no solo detectar una

El robot se mueve libremente sobre ruedas: una sola lectura booleana del sensor ("hay una franja debajo") no alcanza para saber **a qué columna** llegó. Por eso el firmware cuenta cuántas franjas pasó desde el home y compara contra la columna pedida (y a la inversa para volver) — este conteo, hecho en software a partir de la lectura del sensor, es la decisión que efectivamente determina el comportamiento del robot (requisito de la cátedra: "la lectura del sensor debe modificar de manera observable el comportamiento del robot").

La orientación hacia un lado no necesita sensor: la base no tiene encoder ni fin de carrera, así que se mueve por un tren de pulsos de tiempo calibrado a mano contra los topes mecánicos (ver [mecanismo-apertura.md](mecanismo-apertura.md)).

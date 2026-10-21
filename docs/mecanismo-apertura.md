# Mecanismo de apertura y de orientación hacia un lado

El brazo tiene 2 actuadores además del de traslación en X: uno abre el cajón, el otro orienta el brazo hacia el lado izquierdo o derecho. Son dos problemas distintos, resueltos por dos actuadores distintos — a diferencia de un diseño anterior que se había explorado, que intentaba combinar ambos movimientos en un solo motor con una ranura tipo bayoneta (descartada por ser la pieza de mayor riesgo de fabricación de todo el proyecto).

## Apertura: micro servo del brazo

Un micro servo (tipo SG90), con un imán de neodimio en la punta del brazo:

- **Avance** — el servo mueve el brazo hacia adelante (ángulo calibrado `ANG_ADELANTE = 90°`) hasta que el imán hace contacto con una tuerca metálica pegada en el frente de cada cajón. Avance de a 3° cada 15ms.
- **Apertura** — el mismo servo invierte el sentido, despacio (1° cada 30ms, a propósito más lento que el avance, para que el imán no se despegue de golpe), volviendo al ángulo de reposo (`ANG_ATRAS = 40°`). Al retroceder tira del cajón y lo abre. Como retroceder es a la vez "abrir" y "volver a la posición de reposo", no hace falta una fase separada de retracción.

Estos ángulos y velocidades ya están calibrados a mano sobre el hardware real (`src/prueba.cpp`) — no son un placeholder. Pendiente real (de Coty, no resuelto todavía): **el imán no se suelta solo después de abrir** — con el cajón abierto, el imán sigue pegado, y al girar la base de vuelta puede arrastrar el cajón consigo. Falta un paso mecánico o de software para despegarlo (ver [pendientes.md](pendientes.md)).

El imán se autoalinea al acercarse a la tuerca, lo que tolera mejor errores de impresión 3D o de posicionamiento del robot que un gancho mecánico rígido.

## Orientación hacia un lado: base giratoria

Un motor DC con reductora, controlado por un driver L298N, mueve una base giratoria con topes mecánicos a cada extremo — **no tiene encoder ni fin de carrera**, así que no puede posicionarse por ángulo como el servo. Se mueve por un **tren de pulsos de tiempo fijo** (cada pulso: ~50ms de giro + 100ms de freno activo, con 150ms de pausa entre pulsos), calibrado a mano contra los topes físicos: 7 pulsos para orientarse hacia un lado, 6 para volver.

Es un grado de libertad real e independiente de la traslación en X y de la apertura/Z (no abre nada, solo reorienta el brazo). Pendiente real (de Coty): la asimetría 7/6 todavía no se verificó como repetible corriendo la secuencia varias veces seguidas — y la calibración solo se probó yendo hacia un lado; falta confirmar que los mismos pulsos sirven para el lado opuesto (ver [pendientes.md](pendientes.md)).

## Por qué esto es más simple que combinar todo en un solo motor

Una ranura tipo bayoneta que combinara apertura y orientación en un solo motor concentraría el riesgo de fabricación en una sola pieza de alta tolerancia. Separarlos en 2 actuadores dedicados (servo + motor DC) cuesta un actuador más, pero cada uno es mecánicamente simple — el soporte que los sostiene (ver [pieza-3d.md](pieza-3d.md)) no necesita tolerancias especiales.

# Mecanismo de apertura y de cambio de línea

El brazo tiene 2 motores además del de traslación en X: uno abre el cajón, el otro reorienta el brazo hacia una segunda línea de cajones. Son dos problemas distintos, resueltos por dos actuadores distintos — a diferencia del diseño anterior, que intentaba combinar extensión y apertura en un solo motor con una ranura tipo bayoneta (descartada por ser la pieza de mayor riesgo de fabricación de todo el proyecto).

## Apertura: motor Z (el "brazito" adelante/atrás)

Un solo motor DC con reductora, controlado en ambos sentidos vía driver tipo L298N:

- **Extensión (adelante)** — el brazo avanza hasta que el imán en la punta hace contacto con la placa metálica del cajón.
- **Apertura (atrás)** — el mismo motor invierte el sentido. Al retroceder, tira del cajón (enganchado por el imán) y lo abre. Como retroceder es a la vez "abrir" y "volver a la posición retraída", no hace falta una fase separada de retracción después de abrir — es el mismo movimiento.

El imán se autoalinea al acercarse a la placa, lo que tolera mejor errores de impresión 3D o de posicionamiento del robot que un gancho mecánico rígido. Los cajones son chicos y livianos, así que no hace falta una fuerza de enganche muy alta — pero el tamaño/potencia del imán hay que calibrarlo con pruebas reales (con el cajón cargado con la pieza más pesada prevista), para que enganche bien y además se pueda soltar sin que haga falta forzarlo.

## Cambio de línea: servo de orientación

La cajonera tiene más de una línea (fila) de cajones. Un servo gira el brazo completo para alcanzar la línea pedida antes de extenderlo — es un grado de libertad real e independiente de la traslación en X y de la apertura en Z (no abre nada, solo reorienta). Como un servo se posiciona solo al ángulo comandado (`write(angulo)`), no hace falta un sensor de fin de carrera para esta fase: se resuelve con un tiempo fijo de espera, suficiente para que el servo llegue a destino.

Los ángulos por línea son placeholder en el firmware (`ANGULOS_LINEA` en `firmware/src/motores.cpp`) — a calibrar en el banco según cuántas líneas tenga la cajonera final (ver [pendientes.md](pendientes.md)).

## Por qué esto es más simple que el diseño anterior

La ranura bayoneta combinaba 2 movimientos en 1 motor, lo que la hacía la pieza de mayor tolerancia de todo el proyecto (esperaba varias iteraciones de impresión FDM para que el pasador no jugara ni se trabara). Separar apertura y cambio de línea en 2 motores dedicados cuesta un actuador más, pero elimina esa pieza crítica: ni el motor Z ni el servo necesitan tolerancias especiales, solo el soporte mecánico que los sostiene (ver [pieza-3d.md](pieza-3d.md)).

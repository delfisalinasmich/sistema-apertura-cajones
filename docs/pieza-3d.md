# Pieza impresa en 3D

## Qué es

Soporte del brazo: sostiene el micro servo y, en su extremo, la sujeción del imán — todo montado sobre el eje de la base giratoria (ver fotos del banco armado y el mecanismo completo en [mecanismo-apertura.md](mecanismo-apertura.md)).

## Función que cumple

Es la pieza funcional central del brazo: transmite el movimiento del servo hasta la punta, donde sostiene el imán que engancha la tuerca del cajón, y es lo que la base giratoria efectivamente gira al orientar hacia un lado. No es una pieza decorativa — sin ella no existe ni el mecanismo de apertura ni la orientación.

## Por qué es más simple que un diseño con ranura bayoneta

Una ranura tipo bayoneta (pasador deslizando entre un tramo recto y uno diagonal, con tolerancias muy ajustadas) habría sido la pieza de mayor riesgo de fabricación del proyecto. Al separar apertura (servo) y orientación (base giratoria) en dos actuadores dedicados, esta pieza es un soporte mecánico convencional, sin tolerancias especiales — el margen de error de impresión FDM es mucho mayor.

## Pendiente

- Archivo CAD/STL: confirmar si ya está diseñado o sigue pendiente. Va en la carpeta [`cad/`](../cad/) cuando esté listo.
- Dimensionamiento del imán (a calibrar con el prototipo, con el cajón cargado con la pieza más pesada prevista).
- Restricciones de tamaño, tiempo de impresión y materiales indicadas por la cátedra: TODO confirmar.

# Pieza impresa en 3D

## Qué es

Soporte del brazo (motor Z + extremo con el imán), montado sobre el eje del servo de orientación de línea. Ver el mecanismo completo en [mecanismo-apertura.md](mecanismo-apertura.md).

## Función que cumple

Es la pieza funcional central del brazo: sostiene el motor Z y transmite su movimiento adelante/atrás hasta la punta, donde sujeta el imán que engancha y arrastra el cajón. También es lo que el servo efectivamente gira al cambiar de línea. No es una pieza decorativa — sin ella no existe el mecanismo de apertura ni el cambio de línea.

## Por qué cambió respecto al diseño anterior

El diseño anterior concentraba el riesgo de fabricación en una ranura tipo bayoneta (pasador deslizando entre un tramo recto y uno diagonal, con tolerancias muy ajustadas). Al separar apertura (motor Z) y cambio de línea (servo) en dos actuadores dedicados, esta pieza pasa a ser un soporte mecánico convencional — sin la ranura de alta tolerancia, el margen de error de impresión FDM es mucho mayor.

## Pendiente

- Archivo CAD/STL: todavía no diseñado. Va en la carpeta [`cad/`](../cad/) cuando esté listo.
- Dimensionamiento del imán (a calibrar con el prototipo, con el cajón cargado con la pieza más pesada prevista).
- Restricciones de tamaño, tiempo de impresión y materiales indicadas por la cátedra: TODO confirmar.

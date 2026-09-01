# Pieza impresa en 3D

## Qué es

Soporte del pivote con la ranura de trabas (mecanismo bayoneta que combina extensión y rotación con un solo motor) + excéntrica del imán en la punta del brazo. Ver el diseño del mecanismo en [mecanismo-trabas.md](mecanismo-trabas.md).

## Función que cumple

Es la pieza funcional central del brazo: convierte el giro de un único motor en dos movimientos distintos (primero traslación, después rotación) mediante una ranura con tramo recto y tramo diagonal, y en su extremo sostiene el imán descentrado que engancha y arrastra el cajón. No es una pieza decorativa — sin ella no existe el mecanismo de apertura.

## Por qué es el punto crítico del proyecto

Es la pieza de mayor tolerancia de todo el diseño: el pasador que recorre la ranura tiene que deslizar sin jugar y sin trabarse en la transición entre el tramo recto y el diagonal. Con impresión FDM es esperable iterar varias veces (imprimir → probar → ajustar tolerancias) antes de integrarla con el resto del brazo.

## Pendiente

- Archivo CAD/STL: todavía no diseñado. Va en la carpeta [`cad/`](../cad/) cuando esté listo.
- Dimensionamiento del imán y radio de la excéntrica (a calibrar con el prototipo, con el cajón cargado con la pieza más pesada prevista).
- Restricciones de tamaño, tiempo de impresión y materiales indicadas por la cátedra: TODO confirmar.

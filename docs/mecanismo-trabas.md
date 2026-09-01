# Mecanismo de trabas (un solo motor, dos movimientos)

El vástago combina extensión y rotación con un solo motor mediante una ranura tipo bayoneta: parte recta + parte diagonal.

- Mientras un pasador solidario al motor recorre la parte recta de la ranura, el giro del motor se traduce en traslación (el vástago sube).
- Al llegar al final de la parte recta, un tope mecánico frena la traslación; a partir de ahí el pasador entra en la parte diagonal y el mismo giro pasa a traducirse en rotación pura.

## Punto crítico de fabricación

Es la pieza de mayor tolerancia de todo el proyecto. Conviene prototiparla sola (imprimir → probar → ajustar) antes de integrar el resto del brazo, porque con FDM es esperable iterar varias veces hasta que el pasador no juegue ni se trabe.

## Por qué hace falta un sensor en la transición

Como el motor no "sabe" en qué parte de la ranura está solo contando pasos (la transición no cae siempre en el mismo paso exacto por fricción y desgaste), se necesita un sensor de fin de carrera en el punto de transición extensión → rotación. Si no se detecta la transición, el firmware no avanza a la fase de rotación — ver [sensores-actuadores.md](sensores-actuadores.md).

## Extremo del brazo — imán descentrado

El imán no va sobre el eje de rotación (ahí giraría en el lugar sin arrastrar nada), sino a cierta distancia de él, funcionando como una manivela/excéntrica: al girar, describe un arco y arrastra al cajón consigo.

- La ventaja frente a un gancho mecánico es que el imán se autoalinea al acercarse a la placa — tolera mejor errores de impresión 3D o de posicionamiento del carro.
- A medida que la rotación continúa más allá del punto de máximo tirón, el ángulo de contacto imán–placa cambia (cada vez más de canto) y la fuerza magnética efectiva cae, lo que ayuda a que el imán se despegue solo. Este comportamiento depende de la potencia del imán elegido y hay que calibrarlo con pruebas reales (imán de neodimio, tamaño chico-mediano; probar con el cajón cargado con la pieza más pesada prevista).
- Los cajones son chicos y livianos, así que no hace falta un resorte potente: solo tienen que abrirse lo suficiente para que la persona los vea y los termine de sacar a mano.

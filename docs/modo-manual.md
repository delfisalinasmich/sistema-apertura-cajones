# Modo manual

## Medio de control

Interfaz web servida directamente por el ESP32 (aprovecha el WiFi que ya tiene el microcontrolador, sin depender de una app externa). Cumple el requisito de la cátedra de que el usuario controle en tiempo real una parte significativa del movimiento del robot.

## Controles que expone la página

- Mover el carro en X (izquierda / derecha).
- Extender / retraer el vástago en Z.
- Rotar el brazo (apertura manual, para pruebas).
- Botón de pedido manual: "abrir columna N" — dispara la misma rutina autónoma que el pedido por software.
- Parada de emergencia.
- Estado actual del sistema (qué fase de la máquina de estados está corriendo, lectura de los sensores).

## El "pedido por software"

La consigna del grupo no tenía resuelto cómo llega la orden de qué cajón abrir (en la versión final se integraría con el sistema pick-to-light existente). Para no depender de esa integración, el ESP32 expone un endpoint HTTP simple:

```
GET /pedido?columna=N
```

Este endpoint dispara la rutina autónoma (ver [diagrama-flujo-estados.md](diagrama-flujo-estados.md)) para la columna N. Hoy lo llama el mismo botón de la interfaz web (para la demo); el día de mañana lo puede llamar cualquier sistema externo, incluido el pick-to-light real, sin tocar el firmware del brazo.

## Pendiente de definir

- Diseño visual/UX de la página (todavía no hecho, alcanza con algo funcional para la demo).
- Si el ESP32 actúa como access point propio o se conecta a una red existente.

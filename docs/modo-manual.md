# Modo manual

## Medio de control

Interfaz web servida directamente por el ESP32, que arma su propio **Access Point** (`CajonesBot`, ver [pines.md](../firmware/pines.md) para SSID/clave/IP) — no depende de una red existente ni de una app externa. Cumple el requisito de la cátedra de que el usuario controle en tiempo real una parte significativa del movimiento del robot.

Implementado en `firmware/src/servidor_web.*`.

## Controles que expone la página

- Mover el carro en X (izquierda / derecha / parar).
- Mover el vástago (extender / retraer / parar) — un solo control, porque es un único motor que combina extensión y rotación (ver [mecanismo-trabas.md](mecanismo-trabas.md)); no hay un control separado de "rotación".
- Pedido: "abrir columna N" — dispara la misma rutina autónoma que el pedido por software (`/pedido?columna=N`).
- Estado actual del sistema (modo + fase de la máquina de estados), que se refresca solo cada 1 segundo.

Los controles manuales solo tienen efecto si el sistema está en modo `MANUAL` y no está en `ERROR_SEGURO` (ver `firmware/src/servidor_web.cpp`, `controlManualPermitido()`).

## Por qué la parada de emergencia NO está en la página web

Es a propósito: un mecanismo de parada segura no debería depender de que el WiFi o el servidor web estén funcionando. La parada de emergencia es el botón físico `BTN_EMERGENCIA` (ver [pines.md](../firmware/pines.md) y [sensores-actuadores.md](sensores-actuadores.md)), leído directamente por el microcontrolador con prioridad sobre cualquier otra lógica.

## Cómo se reparte el control entre modo manual y modo autónomo

El sistema tiene un modo global (`MANUAL` / `AUTONOMO`, en `firmware/src/modo.*`) separado del estado interno de la rutina autónoma:

- Por defecto arranca en `MANUAL`.
- Al llegar un pedido (botón físico o `/pedido?columna=N`), el sistema pasa a `AUTONOMO` y corre la rutina completa (ver [diagrama-flujo-estados.md](diagrama-flujo-estados.md)).
- Al terminar la rutina (o al resetear una parada de emergencia), vuelve solo a `MANUAL` — el operador recupera el control manual sin tener que hacer nada extra.

## El "pedido por software"

La consigna del grupo no tenía resuelto cómo llega la orden de qué cajón abrir (en la versión final se integraría con el sistema pick-to-light existente). Para no depender de esa integración, el ESP32 expone un endpoint HTTP simple:

```
GET /pedido?columna=N
```

Este endpoint dispara la rutina autónoma para la columna N. Hoy lo llama el mismo botón de la interfaz web (para la demo); el día de mañana lo puede llamar cualquier sistema externo, incluido el pick-to-light real, sin tocar el firmware del brazo.

## Ya resuelto en esta iteración

- ~~Diseño visual/UX de la página~~ — versión funcional simple implementada (botones + polling de estado), suficiente para la demo.
- ~~Si el ESP32 actúa como access point propio o se conecta a una red existente~~ — Access Point propio (`CajonesBot`).

## Pendiente de definir

- Contraseña definitiva del AP antes de la demo (hoy es un valor de prueba, ver `firmware/pines.md`).

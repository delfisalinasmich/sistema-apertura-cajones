# Mapeo de pines — sketch de prueba (ESP32 dev board)

Este mapeo es solo para el sketch de simulación con LEDs y botones (validar la máquina de estados en el banco, sin el brazo físico armado). Cuando exista la electrónica real, estos pines se reasignan a los drivers de motor y a los fines de carrera reales — la lógica de `maquina_estados.*` no cambia, solo `motores.*` y `sensores.*` (ver `firmware/README.md`).

## Salidas — LEDs que simulan los motores

| Pin | Señal | Simula |
|---|---|---|
| GPIO 25 | LED_MOTOR_X | Motor riel X en movimiento |
| GPIO 26 | LED_MOTOR_VASTAGO | Motor del vástago en movimiento (extensión o rotación) |
| GPIO 27 | LED_ERROR | Estado `ERROR_SEGURO` |

## Entradas — botones que simulan sensores

Todos configurados como `INPUT_PULLUP`: presionado = `LOW`.

| Pin | Señal | Simula |
|---|---|---|
| GPIO 32 | BTN_FIN_CARRERA_X | Fin de carrera del riel X (carro llegó a la columna) |
| GPIO 33 | BTN_FIN_CARRERA_TRANSICION | Fin de carrera de la transición de la ranura bayoneta (extensión → rotación) |
| GPIO 34 | BTN_PEDIDO | Simula el pedido de columna (hace las veces del endpoint `/pedido` que todavía no existe) |
| GPIO 35 | BTN_EMERGENCIA | Botón de parada de emergencia |

> GPIO 34 y 35 son solo de entrada en la mayoría de las dev boards ESP32 (sin pull-up interno) — llevan resistencia pull-up externa a 3.3V, o se puede recablear a otro par de pines con pull-up interno si es más simple para el armado en protoboard.

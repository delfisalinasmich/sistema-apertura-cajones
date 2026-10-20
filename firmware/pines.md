# Mapeo de pines — ESP32 dev board

Estos pines son **placeholder**, elegidos evitando pines problemáticos del ESP32 (de strapping, de flash interno, o de solo-entrada para señales de salida) — hay que ajustarlos al cableado real una vez armada la electrónica definitiva. La lógica de `maquina_estados.*` no depende de estos valores, solo `pines.h` (ver [firmware/README.md](README.md)).

## Salidas — drivers de motor y servo

| Pin | Señal | Función |
|---|---|---|
| GPIO 16 | PIN_X_IN1 | Dirección motor X (ruedas), canal 1 del driver L298N |
| GPIO 17 | PIN_X_IN2 | Dirección motor X (ruedas), canal 2 del driver L298N |
| GPIO 18 | PIN_Z_IN1 | Dirección motor Z (brazo), canal 1 del driver L298N |
| GPIO 19 | PIN_Z_IN2 | Dirección motor Z (brazo), canal 2 del driver L298N |
| GPIO 21 | PIN_SERVO_ROTACION | Señal PWM del servo de orientación de línea |
| GPIO 27 | PIN_LED_ERROR | Estado `ERROR_SEGURO` |

Supuestos a confirmar con el cableado real:
- Las 2 ruedas se controlan con las mismas 2 señales de dirección (ambas reciben el mismo comando — no hay giro en el lugar en este diseño). Si en algún momento hace falta virar, hay que separar el control por rueda.
- ENA/ENB del L298N se asumen puenteados a habilitado fijo (sin control de velocidad por PWM todavía).

## Entradas — sensor y botones

Todos configurados como `INPUT_PULLUP`: activo = `LOW`.

| Pin | Señal | Función |
|---|---|---|
| GPIO 34 | PIN_SENSOR_COLUMNA | Sensor óptico IR de columna — leído como entrada digital con antirrebote (ver [pendientes.md](../docs/pendientes.md) sobre digital vs. analógico) |
| GPIO 35 | PIN_BTN_PEDIDO | Pedido de columna 0/línea 0 (equivalente al endpoint `/pedido` de la web; útil para probar sin WiFi) |
| GPIO 32 | PIN_BTN_EMERGENCIA | Botón de parada de emergencia — es físico a propósito, no depende de la web (ver [modo-manual.md](../docs/modo-manual.md)) |

> GPIO 34 y 35 son solo de entrada en la mayoría de las dev boards ESP32 (sin pull-up interno) — llevan resistencia pull-up externa a 3.3V, o se puede recablear a otro par de pines con pull-up interno si es más simple para el armado.

## Modo manual — Access Point del ESP32

| Dato | Valor |
|---|---|
| SSID | `CajonesBot` |
| Contraseña | `AbreCajon26` |
| IP del ESP32 | `192.168.4.1` (IP por defecto del modo Access Point) |

Conectarse a esa red WiFi desde el celular/notebook y abrir `http://192.168.4.1` en el navegador.

# Mapeo de pines — ESP32 NodeMCU-32S

Cableado real del robot armado por Coty (ver su guía en [`referencias/`](../referencias/)), con los nombres de `include/pines.h` renombrados para dejar sin ambigüedad qué motor usa cada salida del L298N (`BASE_IN_*` vs `RUEDAS_IN_*`) — antes del renombre, el mismo par de pines (GPIO27/26) significaba cosas distintas según el entorno, y era un pendiente abierto de la guía de Coty.

## Salidas — L298N y servo

| Pin | Señal | Función |
|---|---|---|
| GPIO 32 | `BASE_IN_A` | L298N IN3 — dirección de la base giratoria |
| GPIO 33 | `BASE_IN_B` | L298N IN4 — dirección de la base giratoria |
| GPIO 27 | `RUEDAS_IN_A` | L298N IN1 — dirección de las ruedas |
| GPIO 26 | `RUEDAS_IN_B` | L298N IN2 — dirección de las ruedas |
| GPIO 25 | `SERVO_PIN` | Señal del micro servo del brazo (rojo a 5V, marrón a GND común) |

**Contradicción sin resolver, confirmar con un chequeo visual hoy en el banco:** `src/prueba.cpp` (sin tocar) mueve la base por IN1/IN2 (GPIO27/26), y con eso la base giró en las pruebas reales ya hechas (asimetría 7/6 pulsos, confirmada repetible). Pero Coty confirmó por escrito que el cableado real es al revés — Motor B (base) en IN3/IN4 (GPIO32/33), Motor A (ruedas) en IN1/IN2 (GPIO27/26, no usado en esa prueba). La tabla de arriba adopta esa confirmación explícita. Para saldar la duda: correr `t` en el entorno `prueba` y mirar qué pieza gira. Si gira la base (no una rueda), `BASE_IN_A/B` y `RUEDAS_IN_A/B` están al revés — alcanza con cambiar estos 4 valores en `pines.h`, nada más.

ENA y ENB del L298N tienen el jumper puesto: no van al ESP32, control solo HIGH/LOW a velocidad máxima, sin PWM.

## Entradas — sensor

| Pin | Señal | Función |
|---|---|---|
| GPIO 34 | `SENSOR_PIN` | Sensor IR TCRT5000 (analógico, salida AO) — ADC1, solo entrada |

El sensor es analógico puro (no un módulo con comparador digital): `analogRead()` + calibración guardada en flash (`include/sensor_linea.h`, entorno `calibracion`). La resistencia de 10 kΩ entre GPIO34 y 3V3 ya está soldada y la calibración ya está guardada (confirmado por Coty).

No hay botón físico de emergencia ni de pedido todavía — la parada de emergencia es el comando Serial `x` (ver [README.md](README.md)).

## Alimentación

- GND de la fuente de los motores, del L298N, del servo y del ESP32 todos unidos.
- El servo va a 5V, nunca a 3V3 — ya está conectado a la salida de 5V del puente H (L298N), no a USB, confirmado por Coty.
- `motorPinesSeguros()` (en `pines.h`) pone `BASE_IN_A/B`/`RUEDAS_IN_A/B` en OUTPUT/LOW — se llama primero en `setup()` de todos los entornos, para que ningún motor arranque solo.

## Modo manual — Access Point del ESP32

| Dato | Valor |
|---|---|
| SSID | `CajonesBot` |
| Contraseña | `AbreCajon26` |
| IP del ESP32 | `192.168.4.1` (IP por defecto del modo Access Point) |

Conectarse a esa red WiFi desde el celular/notebook y abrir `http://192.168.4.1` en el navegador.

# Mapeo de pines — ESP32 NodeMCU-32S

Cableado real del robot armado por Coty (ver su guía en [`referencias/`](../referencias/)), con los nombres de `include/pines.h` renombrados para dejar sin ambigüedad qué motor usa cada salida del L298N (`BASE_IN_*` vs `RUEDAS_IN_*`) — antes del renombre, el mismo par de pines (GPIO27/26) significaba cosas distintas según el entorno, y era un pendiente abierto de la guía de Coty.

## Salidas — L298N y servo

| Pin | Señal | Función |
|---|---|---|
| GPIO 27 | `BASE_IN_A` | L298N IN1 — dirección de la base giratoria |
| GPIO 26 | `BASE_IN_B` | L298N IN2 — dirección de la base giratoria |
| GPIO 32 | `RUEDAS_IN_A` | L298N IN3 — dirección de las ruedas |
| GPIO 33 | `RUEDAS_IN_B` | L298N IN4 — dirección de las ruedas |
| GPIO 25 | `SERVO_PIN` | Señal del micro servo del brazo (rojo a 5V, marrón a GND común) |

**Confirmar contra el cableado físico actual** que `BASE_IN_A/B` sigue siendo el motor de la base y `RUEDAS_IN_A/B` el de las ruedas — el mapeo de arriba está tomado de `src/prueba.cpp` (el entorno más reciente y probado), pero si en la placa real están al revés, alcanza con cambiar estos 4 valores en `pines.h`.

ENA y ENB del L298N tienen el jumper puesto: no van al ESP32, control solo HIGH/LOW a velocidad máxima, sin PWM.

## Entradas — sensor

| Pin | Señal | Función |
|---|---|---|
| GPIO 34 | `SENSOR_PIN` | Sensor IR TCRT5000 (analógico, salida AO) — ADC1, solo entrada |

El sensor es analógico puro (no un módulo con comparador digital): `analogRead()` + calibración guardada en flash (`include/sensor_linea.h`, entorno `calibracion`). **Falta la resistencia de 10 kΩ entre GPIO34 y 3V3** — sin eso el sensor lee cerca de 0 siempre (ver [pendientes.md](../docs/pendientes.md)).

No hay botón físico de emergencia ni de pedido todavía — la parada de emergencia es el comando Serial `x` (ver [README.md](README.md)).

## Alimentación

- GND de la fuente de los motores, del L298N, del servo y del ESP32 todos unidos.
- El servo va a 5V, nunca a 3V3 — con USB solo puede reiniciar la placa al moverse; conviene una fuente externa de 5V (por ejemplo, la salida +5V del L298N con batería).
- `motorPinesSeguros()` (en `pines.h`) pone `BASE_IN_A/B`/`RUEDAS_IN_A/B` en OUTPUT/LOW — se llama primero en `setup()` de todos los entornos, para que ningún motor arranque solo.

## Modo manual — Access Point del ESP32

| Dato | Valor |
|---|---|
| SSID | `CajonesBot` |
| Contraseña | `AbreCajon26` |
| IP del ESP32 | `192.168.4.1` (IP por defecto del modo Access Point) |

Conectarse a esa red WiFi desde el celular/notebook y abrir `http://192.168.4.1` en el navegador.

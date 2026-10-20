#ifndef PINES_H
#define PINES_H

// Mapeo de pines del brazo real de 3 motores (ver firmware/pines.md). Son
// valores placeholder — hay que ajustarlos al cableado real una vez armado.

// Driver del motor X (ruedas) — 2 pines de dirección. Las 2 ruedas se
// controlan como un solo eje (mismas señales a ambos canales del driver):
// no hay giro en el lugar en este diseño. ENA del driver se asume
// puenteado a habilitado fijo (sin PWM de velocidad todavía).
const int PIN_X_IN1 = 16;
const int PIN_X_IN2 = 17;

// Driver del motor Z (brazo adelante/atrás) — 2 pines de dirección. Mismo
// supuesto de ENB puenteado a habilitado fijo.
const int PIN_Z_IN1 = 18;
const int PIN_Z_IN2 = 19;

// Servo de orientación de línea (gira el brazo entre líneas de cajones).
const int PIN_SERVO_ROTACION = 21;

// LED de estado ERROR_SEGURO.
const int PIN_LED_ERROR = 27;

// Sensor óptico de columna (IR, apunta a una marca por columna). Leído
// como entrada digital con antirrebote (ver sensores.cpp). Si el sensor
// real resulta ser un fotodiodo crudo sin comparador integrado, esto va a
// necesitar analogRead() + umbral en vez de digitalRead() — ver
// docs/pendientes.md.
const int PIN_SENSOR_COLUMNA = 34;

// Botón de pedido físico (equivalente al /pedido de la web).
const int PIN_BTN_PEDIDO = 35;

// Botón de parada de emergencia — físico a propósito, no depende de la web.
const int PIN_BTN_EMERGENCIA = 32;

#endif

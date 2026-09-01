#ifndef PINES_H
#define PINES_H

// LEDs que simulan los motores (ver firmware/pines.md)
const int LED_MOTOR_X = 25;
const int LED_MOTOR_VASTAGO = 26;
const int LED_ERROR = 27;

// Botones que simulan sensores (INPUT_PULLUP, presionado = LOW)
const int BTN_FIN_CARRERA_X = 32;
const int BTN_FIN_CARRERA_TRANSICION = 33;
const int BTN_PEDIDO = 34;
const int BTN_EMERGENCIA = 35;

#endif

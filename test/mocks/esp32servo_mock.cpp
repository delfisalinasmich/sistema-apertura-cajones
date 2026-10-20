#include "ESP32Servo.h"

static int anguloActual = 0;
static int pinActual = -1;

void Servo::attach(int pin) {
  pinActual = pin;
}

void Servo::write(int angulo) {
  anguloActual = angulo;
}

int mock_anguloServo() {
  return anguloActual;
}

int mock_pinServo() {
  return pinActual;
}

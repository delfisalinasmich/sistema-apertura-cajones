#ifndef ESP32SERVO_MOCK_H
#define ESP32SERVO_MOCK_H

// Mock mínimo de la librería ESP32Servo que usa servo_brazo.cpp
// (setPeriodHertz/attach/write), para poder compilar y correr la máquina
// de estados nativa sin el core de ESP32 instalado. Ver test/README.md.

class Servo {
public:
  void setPeriodHertz(int hz);
  void attach(int pin);
  void attach(int pin, int pulsoMinUs, int pulsoMaxUs);
  void write(int angulo);
};

// Funciones de control del mock, usadas solo desde los tests.
int mock_anguloServo();
int mock_pinServo();

#endif

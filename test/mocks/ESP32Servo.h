#ifndef ESP32SERVO_MOCK_H
#define ESP32SERVO_MOCK_H

// Mock mínimo de la librería ESP32Servo que usa motores.cpp (attach/write),
// para poder compilar y correr la máquina de estados nativa sin el core de
// ESP32 instalado. Ver test/README.md.

class Servo {
public:
  void attach(int pin);
  void write(int angulo);
};

// Funciones de control del mock, usadas solo desde los tests.
int mock_anguloServo();
int mock_pinServo();

#endif

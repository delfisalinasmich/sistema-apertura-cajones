#ifndef ARDUINO_MOCK_H
#define ARDUINO_MOCK_H

// Mock mínimo de las primitivas de Arduino que usa el firmware
// (motores.cpp, sensores.cpp, seguridad.cpp), para poder compilarlo y
// correrlo nativo en la compu sin ESP32 ni el core de Arduino instalado.
// Ver test/README.md.

#define HIGH 1
#define LOW 0
#define OUTPUT 1
#define INPUT_PULLUP 2

unsigned long millis();
int digitalRead(int pin);
void digitalWrite(int pin, int value);
void pinMode(int pin, int mode);

// Funciones de control del mock, usadas solo desde los tests (no forman
// parte de la API real de Arduino).
void mock_reset();
void mock_avanzarTiempoMs(unsigned long ms);
void mock_setPin(int pin, int value);
int mock_pinEscrito(int pin);

#endif

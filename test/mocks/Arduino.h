#ifndef ARDUINO_MOCK_H
#define ARDUINO_MOCK_H

// Mock mínimo de las primitivas de Arduino que usa el firmware (ruedas.cpp,
// base.cpp, servo_brazo.cpp, maquina_estados.cpp), para poder compilarlo y
// correrlo nativo en la compu sin ESP32 ni el core de Arduino instalado.
// Ver test/README.md.

#include <cstdint>
#include <cstdlib>

#define HIGH 1
#define LOW 0
#define OUTPUT 1
#define INPUT 0
#define INPUT_PULLUP 2
#define ADC_11db 3

#define constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))
#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif
// abs() ya viene de <cstdlib> (incluido arriba) — no se redefine como macro
// porque choca con la declaración de abs(int) de la libc al incluir <map>.

unsigned long millis();
int digitalRead(int pin);
void digitalWrite(int pin, int value);
void pinMode(int pin, int mode);
int analogRead(int pin);
void analogReadResolution(int bits);
void analogSetPinAttenuation(int pin, int atenuacion);

// Mock mínimo de Serial — no imprime nada, solo evita que el código de
// producción deje de compilar por llamarlo.
struct SerialMock {
  void println(const char* s);
  void print(const char* s);
};
extern SerialMock Serial;

// Funciones de control del mock, usadas solo desde los tests (no forman
// parte de la API real de Arduino).
void mock_reset();
void mock_avanzarTiempoMs(unsigned long ms);
void mock_setPin(int pin, int value);
int mock_pinEscrito(int pin);
void mock_setAnalog(int pin, int valor);

#endif

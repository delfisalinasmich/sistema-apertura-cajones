#include "Arduino.h"
#include <map>

static unsigned long tiempoActualMs = 0;
static std::map<int, int> pinesEntrada;
static std::map<int, int> pinesSalida;
static std::map<int, int> pinesAnalogicos;

SerialMock Serial;

void SerialMock::println(const char*) {}
void SerialMock::print(const char*) {}

unsigned long millis() {
  return tiempoActualMs;
}

int digitalRead(int pin) {
  auto it = pinesEntrada.find(pin);
  // Sin configurar explícitamente, un pin INPUT_PULLUP lee HIGH (no presionado).
  return it != pinesEntrada.end() ? it->second : HIGH;
}

void digitalWrite(int pin, int value) {
  pinesSalida[pin] = value;
}

void pinMode(int pin, int mode) {
  (void)pin;
  (void)mode;
}

int analogRead(int pin) {
  auto it = pinesAnalogicos.find(pin);
  return it != pinesAnalogicos.end() ? it->second : 0;
}

void analogReadResolution(int bits) {
  (void)bits;
}

void analogSetPinAttenuation(int pin, int atenuacion) {
  (void)pin;
  (void)atenuacion;
}

void mock_reset() {
  tiempoActualMs = 0;
  pinesEntrada.clear();
  pinesSalida.clear();
  pinesAnalogicos.clear();
}

void mock_avanzarTiempoMs(unsigned long ms) {
  tiempoActualMs += ms;
}

void mock_setPin(int pin, int value) {
  pinesEntrada[pin] = value;
}

int mock_pinEscrito(int pin) {
  auto it = pinesSalida.find(pin);
  return it != pinesSalida.end() ? it->second : LOW;
}

void mock_setAnalog(int pin, int valor) {
  pinesAnalogicos[pin] = valor;
}

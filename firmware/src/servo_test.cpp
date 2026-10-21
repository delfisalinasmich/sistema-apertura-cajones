#include <Arduino.h>

// Prueba breve de un microservo (SG90 o similar) en GPIO25.
//
// CABLEADO:
//   Servo marrón/negro -> GND (común con el ESP32)
//   Servo rojo         -> 5V (VIN/5V del ESP32 por USB; si tiembla o reinicia, fuente externa 5V)
//   Servo naranja/amar -> GPIO25
//
// Comandos por el monitor serie (115200):
//   un número 0-180 -> mueve el servo a ese ángulo
//   s               -> barrido 0 -> 180 -> 0
//   t               -> test automático 0, 90, 180, 90
// Al arrancar hace el test automático una vez.

constexpr int SERVO_PIN = 25;
constexpr int CANAL = 4;            // canal LEDC (no choca con otros usos)
constexpr int FREC = 50;            // 50 Hz = 20 ms de período
constexpr int RESOLUCION = 16;      // bits
constexpr int PULSO_MIN_US = 500;   // 0°   (ajustar si el servo topa)
constexpr int PULSO_MAX_US = 2400;  // 180°

void servoInit() {
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcAttachChannel(SERVO_PIN, FREC, RESOLUCION, CANAL);
#else
  ledcSetup(CANAL, FREC, RESOLUCION);
  ledcAttachPin(SERVO_PIN, CANAL);
#endif
}

void servoAngulo(int grados) {
  grados = constrain(grados, 0, 180);
  uint32_t us = map(grados, 0, 180, PULSO_MIN_US, PULSO_MAX_US);
  uint32_t duty = (uint32_t)(((uint64_t)us * ((1UL << RESOLUCION) - 1)) / (1000000UL / FREC));
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcWrite(SERVO_PIN, duty);
#else
  ledcWrite(CANAL, duty);
#endif
}

void testAuto() {
  const int pasos[] = {0, 90, 180, 90};
  for (int a : pasos) {
    Serial.printf("-> %d grados\n", a);
    servoAngulo(a);
    delay(800);
  }
}

void barrido() {
  Serial.println("Barrido 0 -> 180 -> 0");
  for (int a = 0; a <= 180; a += 2) { servoAngulo(a); delay(15); }
  for (int a = 180; a >= 0; a -= 2) { servoAngulo(a); delay(15); }
}

void setup() {
  Serial.begin(115200);
  servoInit();
  Serial.printf("Test servo en GPIO%d. Mandá 0-180, s=barrido, t=test\n", SERVO_PIN);
  testAuto();
}

void loop() {
  if (!Serial.available()) return;
  char c = Serial.peek();
  if (c == 's') { Serial.read(); barrido(); }
  else if (c == 't') { Serial.read(); testAuto(); }
  else if (isDigit(c)) {
    int a = Serial.parseInt();
    Serial.printf("-> %d grados\n", constrain(a, 0, 180));
    servoAngulo(a);
  } else Serial.read();  // descarta \n, \r y otros
}

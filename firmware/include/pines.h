#pragma once
#include <Arduino.h>

// Pines compartidos por todos los entornos. Mapeo real del robot armado por
// Coty (ver referencias/Guía del proyecto - Coty.md).
//
// Motor (L298N): ENA y ENB tienen el jumper puesto, NO van al ESP32.
// El control es solo digital (HIGH/LOW) en IN1-IN4, a velocidad máxima.
//
// BASE_IN_A/B y RUEDAS_IN_A/B (en vez de IN1-IN4 sin nombre) resuelven el
// pendiente de la guía de Coty: "definir qué salida del L298N mueve las
// ruedas y cuál la base". Acá se adopta el mapeo de prueba.cpp (el entorno
// más reciente y probado físicamente): base en IN1/IN2, ruedas en IN3/IN4.
// CONFIRMAR contra el cableado físico actual antes de subir a la placa real
// — si está al revés, alcanza con cambiar estos 4 valores, nada más.
constexpr int BASE_IN_A = 27;    // L298N IN1
constexpr int BASE_IN_B = 26;    // L298N IN2
constexpr int RUEDAS_IN_A = 32;  // L298N IN3
constexpr int RUEDAS_IN_B = 33;  // L298N IN4

// Sensor infrarrojo analógico único (TCRT5000): VCC -> 3V3, GND -> GND,
// AO -> GPIO34 (ADC1, solo entrada).
constexpr int SENSOR_PIN = 34;

// Micro servo del brazo con imán (5V, GND común).
constexpr int SERVO_PIN = 25;

// Deja IN1-IN4 como salida en LOW cuanto antes (motores frenados/libres).
// Llamar al principio de setup() en todos los entornos.
inline void motorPinesSeguros() {
  const int pines[4] = {BASE_IN_A, BASE_IN_B, RUEDAS_IN_A, RUEDAS_IN_B};
  for (int p : pines) {
    digitalWrite(p, LOW);   // pre-carga el nivel antes de pasar a salida
    pinMode(p, OUTPUT);
    digitalWrite(p, LOW);
  }
}

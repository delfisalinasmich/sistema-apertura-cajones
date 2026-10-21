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
// ruedas y cuál la base".
//
// OJO — hay una contradicción sin resolver todavía (ver docs/pendientes.md):
// el código de src/prueba.cpp (sin tocar) maneja la base por IN1/IN2
// (GPIO27/26), y con eso la base giró en las pruebas reales (asimetría
// 7/6 pulsos confirmada repetible). Pero Coty confirmó por escrito que el
// cableado real es justo al revés: "Motor B (base giratoria) en IN3/IN4
// (GPIO32/33)" y "Motor A (ruedas) en IN1/IN2 (GPIO27/26), no usado en esa
// prueba". Acá se adopta esa confirmación explícita (más detallada que el
// comentario viejo de prueba.cpp), pero hace falta un chequeo visual rápido
// en el banco: correr `t` en el entorno `prueba` y mirar qué pieza gira.
// Si gira la base (no una rueda), estos dos pares están al revés — alcanza
// con cambiar estos 4 valores, nada más.
constexpr int BASE_IN_A = 32;    // L298N IN3
constexpr int BASE_IN_B = 33;    // L298N IN4
constexpr int RUEDAS_IN_A = 27;  // L298N IN1
constexpr int RUEDAS_IN_B = 26;  // L298N IN2

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

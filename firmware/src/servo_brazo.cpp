#include "servo_brazo.h"
#include "pines.h"
#include <Arduino.h>
#include <ESP32Servo.h>

// Rango de pulsos calibrado a mano en src/prueba.cpp.
static const int PULSO_MIN_US = 500;   // 0°
static const int PULSO_MAX_US = 2400;  // 180°

static Servo servo;
static int angulo = 0;
static int anguloObjetivo = 0;
static int servoGrados = 1;
static unsigned long servoIntervalo = 0;
static unsigned long tServo = 0;
static bool servoMoviendo = false;

void servoBrazo_iniciar() {
  servo.setPeriodHertz(50);
  servo.attach(SERVO_PIN, PULSO_MIN_US, PULSO_MAX_US);
}

void servoBrazo_irA(int objetivo, int gradosPorPaso, unsigned long msPorPaso) {
  anguloObjetivo = constrain(objetivo, 0, 180);
  servoGrados = max(1, gradosPorPaso);
  servoIntervalo = msPorPaso;
  tServo = millis();
  servoMoviendo = (angulo != anguloObjetivo);
}

void servoBrazo_actualizar() {
  if (!servoMoviendo || millis() - tServo < servoIntervalo) return;
  tServo = millis();
  int dif = anguloObjetivo - angulo;
  int paso = min(servoGrados, abs(dif));
  angulo += (dif > 0) ? paso : -paso;
  servo.write(angulo);
  if (angulo == anguloObjetivo) servoMoviendo = false;
}

void servoBrazo_saltoInmediato(int anguloNuevo) {
  angulo = anguloObjetivo = constrain(anguloNuevo, 0, 180);
  servoMoviendo = false;
  servo.write(angulo);
}

bool servoBrazo_moviendo() {
  return servoMoviendo;
}

int servoBrazo_anguloActual() {
  return angulo;
}

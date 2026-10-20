#include "motores.h"
#include "pines.h"
#include <Arduino.h>
#include <ESP32Servo.h>

// Ángulos por línea — placeholder, a calibrar en el banco con el brazo real
// (ver docs/pendientes.md). LINEA_HOME es la línea de reposo por defecto.
static const int ANGULOS_LINEA[] = {0, 90};
static const int CANTIDAD_LINEAS = sizeof(ANGULOS_LINEA) / sizeof(ANGULOS_LINEA[0]);

static Servo servoRotacion;

static void escribirDireccion(int pinA, int pinB, Sentido sentido) {
  if (sentido == SENTIDO_POSITIVO) {
    digitalWrite(pinA, HIGH);
    digitalWrite(pinB, LOW);
  } else {
    digitalWrite(pinA, LOW);
    digitalWrite(pinB, HIGH);
  }
}

static void pararEje(int pinA, int pinB) {
  digitalWrite(pinA, LOW);
  digitalWrite(pinB, LOW);
}

void motores_iniciar() {
  pinMode(PIN_X_IN1, OUTPUT);
  pinMode(PIN_X_IN2, OUTPUT);
  pinMode(PIN_Z_IN1, OUTPUT);
  pinMode(PIN_Z_IN2, OUTPUT);
  servoRotacion.attach(PIN_SERVO_ROTACION);
  motores_pararTodos();
  motores_orientarLinea(0);
}

void motores_moverX(Sentido sentido) {
  escribirDireccion(PIN_X_IN1, PIN_X_IN2, sentido);
}

void motores_moverZ(Sentido sentido) {
  escribirDireccion(PIN_Z_IN1, PIN_Z_IN2, sentido);
}

void motores_orientarLinea(int linea) {
  if (linea < 0 || linea >= CANTIDAD_LINEAS) {
    linea = 0;
  }
  servoRotacion.write(ANGULOS_LINEA[linea]);
}

void motores_pararTodos() {
  pararEje(PIN_X_IN1, PIN_X_IN2);
  pararEje(PIN_Z_IN1, PIN_Z_IN2);
  // El servo no se "para": mantiene la última línea comandada.
}

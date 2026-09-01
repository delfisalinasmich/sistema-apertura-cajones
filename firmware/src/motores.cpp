#include "motores.h"
#include "pines.h"
#include <Arduino.h>

void motores_iniciar() {
  pinMode(LED_MOTOR_X, OUTPUT);
  pinMode(LED_MOTOR_VASTAGO, OUTPUT);
  motores_pararTodos();
}

void motores_moverX() {
  digitalWrite(LED_MOTOR_X, HIGH);
  digitalWrite(LED_MOTOR_VASTAGO, LOW);
}

void motores_moverVastago() {
  digitalWrite(LED_MOTOR_X, LOW);
  digitalWrite(LED_MOTOR_VASTAGO, HIGH);
}

void motores_pararTodos() {
  digitalWrite(LED_MOTOR_X, LOW);
  digitalWrite(LED_MOTOR_VASTAGO, LOW);
}

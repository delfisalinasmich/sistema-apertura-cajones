#include "ruedas.h"
#include "pines.h"
#include <Arduino.h>

void ruedas_iniciar() {
  pinMode(RUEDAS_IN_A, OUTPUT);
  pinMode(RUEDAS_IN_B, OUTPUT);
  ruedas_parar();
}

void ruedas_mover(Sentido sentido) {
  digitalWrite(RUEDAS_IN_A, sentido == SENTIDO_ADELANTE ? HIGH : LOW);
  digitalWrite(RUEDAS_IN_B, sentido == SENTIDO_ADELANTE ? LOW : HIGH);
}

void ruedas_parar() {
  digitalWrite(RUEDAS_IN_A, LOW);
  digitalWrite(RUEDAS_IN_B, LOW);
}

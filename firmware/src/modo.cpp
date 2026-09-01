#include "modo.h"

static ModoOperacion modoActual = MODO_MANUAL;

void modo_iniciar() {
  modoActual = MODO_MANUAL;
}

ModoOperacion modo_actual() {
  return modoActual;
}

void modo_establecer(ModoOperacion nuevoModo) {
  modoActual = nuevoModo;
}

const char* modo_nombre(ModoOperacion modo) {
  return modo == MODO_MANUAL ? "MANUAL" : "AUTONOMO";
}

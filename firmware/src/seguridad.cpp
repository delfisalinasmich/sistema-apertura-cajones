#include "seguridad.h"
#include "sensores.h"
#include "motores.h"
#include "pines.h"
#include <Arduino.h>

void seguridad_iniciar() {
  pinMode(PIN_LED_ERROR, OUTPUT);
  digitalWrite(PIN_LED_ERROR, LOW);
}

void seguridad_actualizar(bool enErrorSeguro) {
  digitalWrite(PIN_LED_ERROR, enErrorSeguro ? HIGH : LOW);
  if (enErrorSeguro) {
    motores_pararTodos();
  }
}

bool seguridad_emergenciaActiva() {
  return sensores_emergenciaActiva();
}

// Reset manual: soltar el botón de emergencia y presionar el de pedido.
// Evita depender de un quinto pin físico solo para esto en el sketch de banco.
bool seguridad_resetSolicitado() {
  return !sensores_emergenciaActiva() && sensores_pedidoPresionado();
}

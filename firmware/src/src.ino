#include "pines.h"
#include "motores.h"
#include "sensores.h"
#include "seguridad.h"
#include "maquina_estados.h"

static EstadoSistema estadoAnterior = ESTADO_REPOSO;

void setup() {
  Serial.begin(115200);
  motores_iniciar();
  sensores_iniciar();
  seguridad_iniciar();
  maquinaEstados_iniciar();
  Serial.println("Estado inicial: REPOSO");
}

void loop() {
  maquinaEstados_actualizar();

  EstadoSistema estadoActual = maquinaEstados_estadoActual();
  if (estadoActual != estadoAnterior) {
    Serial.print("Estado: ");
    Serial.println(maquinaEstados_nombre(estadoActual));
    estadoAnterior = estadoActual;
  }
}

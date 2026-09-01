#ifndef MAQUINA_ESTADOS_H
#define MAQUINA_ESTADOS_H

// Máquina de estados de la rutina autónoma (ver docs/diagrama-flujo-estados.md).

enum EstadoSistema {
  ESTADO_REPOSO,
  ESTADO_MOVIENDO_X,
  ESTADO_EXTENDIENDO_Z,
  ESTADO_ROTANDO_APERTURA,
  ESTADO_SIDESTEP_X,
  ESTADO_RETRAYENDO_Z,
  ESTADO_VOLVIENDO_HOME,
  ESTADO_ERROR_SEGURO
};

void maquinaEstados_iniciar();
void maquinaEstados_actualizar();
EstadoSistema maquinaEstados_estadoActual();
const char* maquinaEstados_nombre(EstadoSistema estado);

#endif

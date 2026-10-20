#ifndef MAQUINA_ESTADOS_H
#define MAQUINA_ESTADOS_H

// Máquina de estados de la rutina autónoma (ver docs/diagrama-flujo-estados.md).

enum EstadoSistema {
  ESTADO_REPOSO,
  ESTADO_MOVIENDO_X,
  ESTADO_ORIENTANDO_LINEA,
  ESTADO_EXTENDIENDO_Z,
  ESTADO_ABRIENDO,
  ESTADO_SIDESTEP_X,
  ESTADO_VOLVIENDO_HOME,
  ESTADO_ERROR_SEGURO
};

void maquinaEstados_iniciar();
void maquinaEstados_actualizar();
EstadoSistema maquinaEstados_estadoActual();
const char* maquinaEstados_nombre(EstadoSistema estado);

// Arranca la rutina para la columna y línea pedidas (botón físico o /pedido
// de la web). Solo tiene efecto si el sistema está en REPOSO; cambia el
// modo a AUTONOMO.
void maquinaEstados_solicitarPedido(int columna, int linea);
int maquinaEstados_columnaSolicitada();
int maquinaEstados_lineaSolicitada();

#endif

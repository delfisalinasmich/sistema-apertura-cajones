#include "maquina_estados.h"
#include "sensores.h"
#include "motores.h"
#include "seguridad.h"
#include "modo.h"
#include <Arduino.h>

// Duraciones simuladas para las fases sin sensor dedicado en el mínimo de
// hardware confirmado (docs/sensores-actuadores.md). ROTANDO_APERTURA y
// SIDESTEP_X ya se documentaron como definidas por ángulo/tiempo. RETRAYENDO_Z
// también se resuelve por tiempo: el diseño depende del fail-safe por
// gravedad para volver a reposo, no de un tercer fin de carrera dedicado
// (ver docs/mecanismo-trabas.md). Si el grupo agrega sensores dedicados más
// adelante, alcanza con reemplazar estas comparaciones por lecturas de
// sensores.*, sin tocar el resto de la máquina de estados.
static const unsigned long DURACION_ROTACION_MS = 1500;
static const unsigned long DURACION_SIDESTEP_MS = 800;
static const unsigned long DURACION_RETRACCION_MS = 1500;
static const unsigned long DURACION_VOLVIENDO_HOME_MS = 1000;

static EstadoSistema estadoActual = ESTADO_REPOSO;
static unsigned long inicioEstadoMs = 0;
static int columnaSolicitada = 0;

static void cambiarEstado(EstadoSistema nuevoEstado) {
  estadoActual = nuevoEstado;
  inicioEstadoMs = millis();
}

static unsigned long tiempoEnEstadoMs() {
  return millis() - inicioEstadoMs;
}

// Vuelve a REPOSO y le devuelve el control al modo manual: la rutina
// autónoma es un paréntesis transitorio que se dispara con un pedido, no un
// modo persistente (ver docs/modo-manual.md).
static void volverAReposoManual() {
  cambiarEstado(ESTADO_REPOSO);
  modo_establecer(MODO_MANUAL);
}

void maquinaEstados_iniciar() {
  cambiarEstado(ESTADO_REPOSO);
  motores_pararTodos();
}

EstadoSistema maquinaEstados_estadoActual() {
  return estadoActual;
}

int maquinaEstados_columnaSolicitada() {
  return columnaSolicitada;
}

void maquinaEstados_solicitarPedido(int columna) {
  if (estadoActual != ESTADO_REPOSO) {
    return; // ya hay una rutina en curso, se ignora el pedido
  }
  columnaSolicitada = columna;
  modo_establecer(MODO_AUTONOMO);
  cambiarEstado(ESTADO_MOVIENDO_X);
}

void maquinaEstados_actualizar() {
  // La emergencia tiene prioridad sobre cualquier otra transición, desde
  // cualquier estado, en cualquier modo.
  bool enErrorSeguro = (estadoActual == ESTADO_ERROR_SEGURO);
  if (seguridad_emergenciaActiva() && !enErrorSeguro) {
    cambiarEstado(ESTADO_ERROR_SEGURO);
    enErrorSeguro = true;
  }
  seguridad_actualizar(enErrorSeguro);

  switch (estadoActual) {

    case ESTADO_REPOSO:
      // El pedido que dispara esta rutina llega por
      // maquinaEstados_solicitarPedido() (botón físico o /pedido de la web,
      // ver src.ino), no se escucha directamente acá.
      motores_pararTodos();
      break;

    case ESTADO_MOVIENDO_X:
      motores_moverX();
      if (sensores_finCarreraX()) {
        cambiarEstado(ESTADO_EXTENDIENDO_Z);
      }
      break;

    case ESTADO_EXTENDIENDO_Z:
      motores_moverVastago();
      if (sensores_finCarreraTransicion()) {
        cambiarEstado(ESTADO_ROTANDO_APERTURA);
      }
      break;

    case ESTADO_ROTANDO_APERTURA:
      motores_moverVastago();
      if (tiempoEnEstadoMs() >= DURACION_ROTACION_MS) {
        cambiarEstado(ESTADO_SIDESTEP_X);
      }
      break;

    case ESTADO_SIDESTEP_X:
      motores_moverX();
      if (tiempoEnEstadoMs() >= DURACION_SIDESTEP_MS) {
        cambiarEstado(ESTADO_RETRAYENDO_Z);
      }
      break;

    case ESTADO_RETRAYENDO_Z:
      motores_moverVastago();
      if (tiempoEnEstadoMs() >= DURACION_RETRACCION_MS) {
        cambiarEstado(ESTADO_VOLVIENDO_HOME);
      }
      break;

    case ESTADO_VOLVIENDO_HOME:
      motores_moverX();
      if (tiempoEnEstadoMs() >= DURACION_VOLVIENDO_HOME_MS) {
        volverAReposoManual();
      }
      break;

    case ESTADO_ERROR_SEGURO:
      motores_pararTodos();
      if (seguridad_resetSolicitado()) {
        volverAReposoManual();
      }
      break;
  }
}

const char* maquinaEstados_nombre(EstadoSistema estado) {
  switch (estado) {
    case ESTADO_REPOSO: return "REPOSO";
    case ESTADO_MOVIENDO_X: return "MOVIENDO_X";
    case ESTADO_EXTENDIENDO_Z: return "EXTENDIENDO_Z";
    case ESTADO_ROTANDO_APERTURA: return "ROTANDO_APERTURA";
    case ESTADO_SIDESTEP_X: return "SIDESTEP_X";
    case ESTADO_RETRAYENDO_Z: return "RETRAYENDO_Z";
    case ESTADO_VOLVIENDO_HOME: return "VOLVIENDO_HOME";
    case ESTADO_ERROR_SEGURO: return "ERROR_SEGURO";
    default: return "DESCONOCIDO";
  }
}

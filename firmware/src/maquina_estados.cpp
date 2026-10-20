#include "maquina_estados.h"
#include "sensores.h"
#include "motores.h"
#include "seguridad.h"
#include "modo.h"
#include <Arduino.h>

// Línea de reposo por defecto — ver motores.cpp (ANGULOS_LINEA).
static const int LINEA_HOME = 0;

// Duraciones simuladas para las fases sin sensor dedicado (ver
// docs/sensores-actuadores.md: el único sensor confirmado como mínimo es el
// óptico de columna, usado en MOVIENDO_X). El resto de las fases dependen
// del fail-safe ya documentado, no de hardware adicional. Si el grupo
// termina agregando sensores dedicados, alcanza con reemplazar estas
// comparaciones de tiempo por lecturas de sensores.*, sin tocar el resto de
// la máquina de estados.
static const unsigned long DURACION_ORIENTACION_MS = 1000;
static const unsigned long DURACION_EXTENSION_MS = 1200;
static const unsigned long DURACION_APERTURA_MS = 1200;
static const unsigned long DURACION_SIDESTEP_MS = 800;
static const unsigned long DURACION_VOLVIENDO_HOME_MS = 1000;

static EstadoSistema estadoActual = ESTADO_REPOSO;
static unsigned long inicioEstadoMs = 0;
static int columnaSolicitada = 0;
static int lineaSolicitada = LINEA_HOME;

// Conteo de marcas del sensor óptico durante MOVIENDO_X: un robot móvil no
// puede confirmar "llegué a la columna N" con una sola lectura booleana
// como lo hacía el fin de carrera fijo del diseño anterior — hace falta
// contar cuántas marcas de columna pasó desde el home. marcaAnterior
// detecta el flanco (no cuenta mientras el sensor sigue "viendo" la misma
// marca).
static int marcasContadas = 0;
static bool marcaAnterior = false;

static void cambiarEstado(EstadoSistema nuevoEstado) {
  estadoActual = nuevoEstado;
  inicioEstadoMs = millis();
}

static unsigned long tiempoEnEstadoMs() {
  return millis() - inicioEstadoMs;
}

// Vuelve a REPOSO y le devuelve el control al modo manual: la rutina
// autónoma es un paréntesis transitorio que se dispara con un pedido, no un
// modo persistente (ver docs/modo-manual.md). También reinicia el contador
// de marcas y la orientación del servo a la línea home, consistente con que
// REPOSO es la posición seguro por defecto.
static void volverAReposoManual() {
  cambiarEstado(ESTADO_REPOSO);
  modo_establecer(MODO_MANUAL);
  marcasContadas = 0;
  motores_orientarLinea(LINEA_HOME);
}

void maquinaEstados_iniciar() {
  cambiarEstado(ESTADO_REPOSO);
  motores_pararTodos();
  marcasContadas = 0;
}

EstadoSistema maquinaEstados_estadoActual() {
  return estadoActual;
}

int maquinaEstados_columnaSolicitada() {
  return columnaSolicitada;
}

int maquinaEstados_lineaSolicitada() {
  return lineaSolicitada;
}

void maquinaEstados_solicitarPedido(int columna, int linea) {
  if (estadoActual != ESTADO_REPOSO) {
    return; // ya hay una rutina en curso, se ignora el pedido
  }
  columnaSolicitada = columna;
  lineaSolicitada = linea;
  marcasContadas = 0;
  marcaAnterior = sensores_marcaColumna();
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

    case ESTADO_MOVIENDO_X: {
      motores_moverX(SENTIDO_POSITIVO);
      bool marcaAhora = sensores_marcaColumna();
      if (marcaAhora && !marcaAnterior) {
        marcasContadas++;
      }
      marcaAnterior = marcaAhora;
      if (marcasContadas >= columnaSolicitada) {
        cambiarEstado(ESTADO_ORIENTANDO_LINEA);
      }
      break;
    }

    case ESTADO_ORIENTANDO_LINEA:
      motores_orientarLinea(lineaSolicitada);
      if (tiempoEnEstadoMs() >= DURACION_ORIENTACION_MS) {
        cambiarEstado(ESTADO_EXTENDIENDO_Z);
      }
      break;

    case ESTADO_EXTENDIENDO_Z:
      motores_moverZ(SENTIDO_POSITIVO);
      if (tiempoEnEstadoMs() >= DURACION_EXTENSION_MS) {
        cambiarEstado(ESTADO_ABRIENDO);
      }
      break;

    case ESTADO_ABRIENDO:
      // El motor Z en reversa tira del cajón y de paso vuelve el brazo a
      // su posición retraída — por eso no hace falta una fase separada de
      // retracción (a diferencia del diseño anterior).
      motores_moverZ(SENTIDO_NEGATIVO);
      if (tiempoEnEstadoMs() >= DURACION_APERTURA_MS) {
        cambiarEstado(ESTADO_SIDESTEP_X);
      }
      break;

    case ESTADO_SIDESTEP_X:
      motores_moverX(SENTIDO_POSITIVO);
      if (tiempoEnEstadoMs() >= DURACION_SIDESTEP_MS) {
        cambiarEstado(ESTADO_VOLVIENDO_HOME);
      }
      break;

    case ESTADO_VOLVIENDO_HOME:
      motores_moverX(SENTIDO_NEGATIVO);
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
    case ESTADO_ORIENTANDO_LINEA: return "ORIENTANDO_LINEA";
    case ESTADO_EXTENDIENDO_Z: return "EXTENDIENDO_Z";
    case ESTADO_ABRIENDO: return "ABRIENDO";
    case ESTADO_SIDESTEP_X: return "SIDESTEP_X";
    case ESTADO_VOLVIENDO_HOME: return "VOLVIENDO_HOME";
    case ESTADO_ERROR_SEGURO: return "ERROR_SEGURO";
    default: return "DESCONOCIDO";
  }
}

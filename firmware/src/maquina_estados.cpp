#include "maquina_estados.h"
#include "ruedas.h"
#include "base.h"
#include "servo_brazo.h"
#include "sensor_linea.h"
#include "pines.h"
#include "modo.h"
#include <Arduino.h>
#include <Preferences.h>

// ---- Constantes calibradas a mano en src/prueba.cpp — se reutilizan tal
// cual, no se adivinan de nuevo. ----
static const int ANG_ATRAS = 40;      // reposo: brazo hacia atrás (tira del cajón al volver acá)
static const int ANG_ADELANTE = 90;   // brazo hacia adelante: el imán toca la tuerca del cajón
static const int VEL_SERVO_IDA_GRADOS = 3;
static const int VEL_SERVO_IDA_MS = 15;
static const int VEL_SERVO_VUELTA_GRADOS = 1;   // más lenta para que no se despegue el imán
static const int VEL_SERVO_VUELTA_MS = 30;
static const unsigned long T_CONTACTO_MS = 500;   // espera con el imán pegado a la tuerca
static const unsigned long T_ABIERTO_MS = 500;    // espera con el cajón abierto
static const int BASE_PULSOS_IDA = 7;             // pulsos de la base para llegar al cajón
static const int BASE_PULSOS_VUELTA = 6;          // pulsos para volver (asimetría sin resolver, ver pendientes.md)

// Fases sin equivalente calibrado en prueba.cpp (traslación en X, que
// prueba.cpp no probaba). Mismo criterio que ya se venía usando: resolver
// por tiempo o por sensor según haya o no hardware dedicado, sin inventar
// lo que no está.
static const unsigned long DURACION_SIDESTEP_MS = 800;

static Preferences prefs;
static CalLinea cal;

static EstadoSistema estadoActual = ESTADO_REPOSO;
static unsigned long inicioEstadoMs = 0;
static Lado ladoSolicitado = LADO_DERECHA;
static int columnaSolicitada = 0;

// Cantidad de franjas desde home. Se usa tanto para saber si ya se llegó a
// la columna pedida (MOVIENDO_X) como para volver simétricamente
// (VOLVIENDO_HOME) — en vez de resolver la vuelta por tiempo fijo, ya que
// el sensor de franjas sirve para las dos direcciones.
static int marcasDesdeHome = 0;
static bool marcaAnterior = false;

// Espera posterior a que el servo termina de moverse (T_CONTACTO_MS /
// T_ABIERTO_MS), igual que accionTerminada()+esperando en prueba.cpp: el
// tiempo de espera cuenta desde que el servo llega, no desde que arranca a
// moverse.
static bool enEsperaPostServo = false;
static unsigned long tEsperaPostServo = 0;

static void cambiarEstado(EstadoSistema nuevoEstado) {
  estadoActual = nuevoEstado;
  inicioEstadoMs = millis();
}

static unsigned long tiempoEnEstadoMs() {
  return millis() - inicioEstadoMs;
}

static bool leerMarca() {
  return detectaLinea(cal, analogRead(SENSOR_PIN));
}

static void volverAReposoManual() {
  cambiarEstado(ESTADO_REPOSO);
  modo_establecer(MODO_MANUAL);
  ruedas_parar();
}

void maquinaEstados_iniciar() {
  ruedas_iniciar();
  base_iniciar();
  servoBrazo_iniciar();
  servoBrazo_saltoInmediato(ANG_ATRAS);

  prefs.begin("linea", true);
  if (!cargarCalibracion(prefs, cal)) {
    Serial.println("Sin calibración válida del sensor de línea: correr el entorno calibracion (w y b).");
  }

  cambiarEstado(ESTADO_REPOSO);
  marcasDesdeHome = 0;
}

EstadoSistema maquinaEstados_estadoActual() {
  return estadoActual;
}

const char* estado() {
  switch (estadoActual) {
    case ESTADO_REPOSO: return "REPOSO";
    case ESTADO_MOVIENDO_X: return "MOVIENDO_X";
    case ESTADO_ORIENTANDO_BASE: return "ORIENTANDO_BASE";
    case ESTADO_EXTENDIENDO: return "EXTENDIENDO";
    case ESTADO_ABRIENDO: return "ABRIENDO";
    case ESTADO_VOLVIENDO_BASE: return "VOLVIENDO_BASE";
    case ESTADO_SIDESTEP_X: return "SIDESTEP_X";
    case ESTADO_VOLVIENDO_HOME: return "VOLVIENDO_HOME";
    case ESTADO_ERROR_SEGURO: return "ERROR_SEGURO";
    default: return "DESCONOCIDO";
  }
}

bool abrirCajon(Lado lado, int n) {
  if (estadoActual != ESTADO_REPOSO) {
    return false; // ya hay una rutina en curso, o estamos en ERROR_SEGURO
  }
  ladoSolicitado = lado;
  columnaSolicitada = n;
  marcasDesdeHome = 0;
  marcaAnterior = leerMarca();
  modo_establecer(MODO_AUTONOMO);
  cambiarEstado(ESTADO_MOVIENDO_X);
  return true;
}

void girarBase(Lado lado) {
  if (modo_actual() != MODO_MANUAL || estadoActual == ESTADO_ERROR_SEGURO || base_activa()) {
    return;
  }
  base_girarPulsos(lado == LADO_IZQUIERDA, 1);
}

void irAHome() {
  if (estadoActual == ESTADO_ERROR_SEGURO) {
    // No hay botón físico de emergencia dedicado todavía (ver
    // docs/pendientes.md): "ir a home" también hace de reset manual.
    volverAReposoManual();
    return;
  }
  if (estadoActual != ESTADO_REPOSO) {
    return; // hay una rutina en curso, se ignora
  }
  modo_establecer(MODO_AUTONOMO);
  cambiarEstado(ESTADO_VOLVIENDO_HOME);
}

void parar() {
  ruedas_parar();
  base_apagar();
  servoBrazo_saltoInmediato(servoBrazo_anguloActual());
  cambiarEstado(ESTADO_ERROR_SEGURO);
}

void maquinaEstados_actualizar() {
  switch (estadoActual) {

    case ESTADO_REPOSO:
      ruedas_parar();
      break;

    case ESTADO_MOVIENDO_X: {
      ruedas_mover(SENTIDO_ADELANTE);
      bool marcaAhora = leerMarca();
      if (marcaAhora && !marcaAnterior) {
        marcasDesdeHome++;
      }
      marcaAnterior = marcaAhora;
      if (marcasDesdeHome >= columnaSolicitada) {
        ruedas_parar();
        cambiarEstado(ESTADO_ORIENTANDO_BASE);
        base_girarPulsos(ladoSolicitado == LADO_IZQUIERDA, BASE_PULSOS_IDA);
      }
      break;
    }

    case ESTADO_ORIENTANDO_BASE:
      // base_actualizar() se llama siempre desde main.cpp, sin importar el
      // modo — acá solo se consulta si ya terminó.
      if (!base_activa()) {
        cambiarEstado(ESTADO_EXTENDIENDO);
        enEsperaPostServo = false;
        servoBrazo_irA(ANG_ADELANTE, VEL_SERVO_IDA_GRADOS, VEL_SERVO_IDA_MS);
      }
      break;

    case ESTADO_EXTENDIENDO:
      // servoBrazo_actualizar() se llama siempre desde main.cpp.
      if (!servoBrazo_moviendo()) {
        if (!enEsperaPostServo) {
          enEsperaPostServo = true;
          tEsperaPostServo = millis();
        } else if (millis() - tEsperaPostServo >= T_CONTACTO_MS) {
          // TODO: acá es donde habría que resolver que el imán no se
          // suelta solo después de abrir (pendiente de Coty, ver
          // docs/pendientes.md) — por ahora la secuencia sigue igual que
          // en prueba.cpp.
          cambiarEstado(ESTADO_ABRIENDO);
          enEsperaPostServo = false;
          servoBrazo_irA(ANG_ATRAS, VEL_SERVO_VUELTA_GRADOS, VEL_SERVO_VUELTA_MS);
        }
      }
      break;

    case ESTADO_ABRIENDO:
      if (!servoBrazo_moviendo()) {
        if (!enEsperaPostServo) {
          enEsperaPostServo = true;
          tEsperaPostServo = millis();
        } else if (millis() - tEsperaPostServo >= T_ABIERTO_MS) {
          cambiarEstado(ESTADO_VOLVIENDO_BASE);
          base_girarPulsos(ladoSolicitado != LADO_IZQUIERDA, BASE_PULSOS_VUELTA);
        }
      }
      break;

    case ESTADO_VOLVIENDO_BASE:
      if (!base_activa()) {
        cambiarEstado(ESTADO_SIDESTEP_X);
        ruedas_mover(SENTIDO_ADELANTE);
      }
      break;

    case ESTADO_SIDESTEP_X:
      if (tiempoEnEstadoMs() >= DURACION_SIDESTEP_MS) {
        ruedas_parar();
        cambiarEstado(ESTADO_VOLVIENDO_HOME);
        // marcaAnterior se reinicia acá, con la lectura real en este
        // momento: entre MOVIENDO_X y acá pasaron ORIENTANDO_BASE,
        // EXTENDIENDO, ABRIENDO y VOLVIENDO_BASE, que no tocan el sensor,
        // así que marcaAnterior podría haber quedado en "true" desde el
        // final de MOVIENDO_X y comerse el primer flanco real de la vuelta.
        marcaAnterior = leerMarca();
      }
      break;

    case ESTADO_VOLVIENDO_HOME: {
      ruedas_mover(SENTIDO_ATRAS);
      bool marcaAhora = leerMarca();
      if (marcaAhora && !marcaAnterior) {
        marcasDesdeHome--;
      }
      marcaAnterior = marcaAhora;
      if (marcasDesdeHome <= 0) {
        volverAReposoManual();
      }
      break;
    }

    case ESTADO_ERROR_SEGURO:
      ruedas_parar();
      base_apagar();
      // El reset se dispara llamando a irAHome(), no acá — ver su comentario.
      break;
  }
}

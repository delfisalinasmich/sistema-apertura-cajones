#include "sensores.h"
#include "pines.h"

// Antirrebote por software, no bloqueante (sin delay()).
struct BotonDebounced {
  int pin;
  int estadoEstable;
  int ultimaLectura;
  unsigned long ultimoCambioMs;
};

static const unsigned long DEBOUNCE_MS = 30;

static BotonDebounced botonFinCarreraX = {BTN_FIN_CARRERA_X, HIGH, HIGH, 0};
static BotonDebounced botonFinCarreraTransicion = {BTN_FIN_CARRERA_TRANSICION, HIGH, HIGH, 0};
static BotonDebounced botonPedido = {BTN_PEDIDO, HIGH, HIGH, 0};
static BotonDebounced botonEmergencia = {BTN_EMERGENCIA, HIGH, HIGH, 0};

static void actualizarBoton(BotonDebounced &boton) {
  int lectura = digitalRead(boton.pin);
  if (lectura != boton.ultimaLectura) {
    boton.ultimoCambioMs = millis();
    boton.ultimaLectura = lectura;
  }
  if (millis() - boton.ultimoCambioMs > DEBOUNCE_MS) {
    boton.estadoEstable = lectura;
  }
}

void sensores_iniciar() {
  pinMode(BTN_FIN_CARRERA_X, INPUT_PULLUP);
  pinMode(BTN_FIN_CARRERA_TRANSICION, INPUT_PULLUP);
  pinMode(BTN_PEDIDO, INPUT_PULLUP);
  pinMode(BTN_EMERGENCIA, INPUT_PULLUP);
}

void sensores_actualizar() {
  actualizarBoton(botonFinCarreraX);
  actualizarBoton(botonFinCarreraTransicion);
  actualizarBoton(botonPedido);
  actualizarBoton(botonEmergencia);
}

bool sensores_finCarreraX() {
  return botonFinCarreraX.estadoEstable == LOW;
}

bool sensores_finCarreraTransicion() {
  return botonFinCarreraTransicion.estadoEstable == LOW;
}

bool sensores_pedidoPresionado() {
  return botonPedido.estadoEstable == LOW;
}

bool sensores_emergenciaActiva() {
  return botonEmergencia.estadoEstable == LOW;
}

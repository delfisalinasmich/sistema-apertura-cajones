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

static BotonDebounced sensorMarcaColumna = {PIN_SENSOR_COLUMNA, HIGH, HIGH, 0};
static BotonDebounced botonPedido = {PIN_BTN_PEDIDO, HIGH, HIGH, 0};
static BotonDebounced botonEmergencia = {PIN_BTN_EMERGENCIA, HIGH, HIGH, 0};

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

// Reinicia el estado de antirrebote de un botón a "no presionado", con el
// timestamp en el momento real de inicialización (no un 0 fijo) para que
// sensores_iniciar() deje el módulo en un estado limpio sin importar cuánto
// llevaba corriendo el reloj (esto es lo que permite reiniciar el módulo
// entre tests nativos, ver test/test_maquina_estados.cpp).
static void inicializarBoton(BotonDebounced &boton, int pin) {
  boton.pin = pin;
  boton.estadoEstable = HIGH;
  boton.ultimaLectura = HIGH;
  boton.ultimoCambioMs = millis();
}

void sensores_iniciar() {
  pinMode(PIN_SENSOR_COLUMNA, INPUT_PULLUP);
  pinMode(PIN_BTN_PEDIDO, INPUT_PULLUP);
  pinMode(PIN_BTN_EMERGENCIA, INPUT_PULLUP);

  inicializarBoton(sensorMarcaColumna, PIN_SENSOR_COLUMNA);
  inicializarBoton(botonPedido, PIN_BTN_PEDIDO);
  inicializarBoton(botonEmergencia, PIN_BTN_EMERGENCIA);
}

void sensores_actualizar() {
  actualizarBoton(sensorMarcaColumna);
  actualizarBoton(botonPedido);
  actualizarBoton(botonEmergencia);
}

bool sensores_marcaColumna() {
  return sensorMarcaColumna.estadoEstable == LOW;
}

bool sensores_pedidoPresionado() {
  return botonPedido.estadoEstable == LOW;
}

bool sensores_emergenciaActiva() {
  return botonEmergencia.estadoEstable == LOW;
}

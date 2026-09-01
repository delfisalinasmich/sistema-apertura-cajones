// Tests nativos (sin ESP32) de la máquina de estados real del firmware.
// Se compilan los .cpp de producción tal cual (ver test/run_tests.sh),
// mockeando solo las primitivas de Arduino (test/mocks/Arduino.h).
// Ver test/README.md.

#include <cstdio>
#include <cstdlib>
#include "Arduino.h"
#include "pines.h"
#include "modo.h"
#include "sensores.h"
#include "maquina_estados.h"

static int totalTests = 0;
static int fallos = 0;

static void verificar(bool condicion, const char* mensaje, int linea) {
  totalTests++;
  if (condicion) {
    std::printf("  OK   %s\n", mensaje);
  } else {
    fallos++;
    std::printf("  FAIL %s (linea %d)\n", mensaje, linea);
  }
}

#define VERIFICAR(cond, msg) verificar((cond), (msg), __LINE__)

static void reiniciar() {
  mock_reset();
  sensores_iniciar();
  modo_iniciar();
  maquinaEstados_iniciar();
}

// Simula presionar/soltar un botón respetando el antirrebote real de
// sensores.cpp: hace falta que el pin quede estable por más de
// DEBOUNCE_MS (30ms) entre dos llamadas a sensores_actualizar().
static void presionar(int pin) {
  mock_setPin(pin, LOW);
  sensores_actualizar();
  mock_avanzarTiempoMs(40);
  sensores_actualizar();
}

static void soltar(int pin) {
  mock_setPin(pin, HIGH);
  sensores_actualizar();
  mock_avanzarTiempoMs(40);
  sensores_actualizar();
}

static void test_estadoInicial() {
  std::printf("test_estadoInicial\n");
  reiniciar();
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_REPOSO, "estado inicial debe ser REPOSO");
  VERIFICAR(modo_actual() == MODO_MANUAL, "modo inicial debe ser MANUAL");
}

static void test_cicloCompletoFeliz() {
  std::printf("test_cicloCompletoFeliz\n");
  reiniciar();

  maquinaEstados_solicitarPedido(3);
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_MOVIENDO_X, "pedido desde REPOSO pasa a MOVIENDO_X");
  VERIFICAR(modo_actual() == MODO_AUTONOMO, "pedido cambia el modo a AUTONOMO");
  VERIFICAR(maquinaEstados_columnaSolicitada() == 3, "guarda la columna pedida");

  presionar(BTN_FIN_CARRERA_X);
  maquinaEstados_actualizar();
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_EXTENDIENDO_Z, "fin de carrera X avanza a EXTENDIENDO_Z");
  soltar(BTN_FIN_CARRERA_X);
  maquinaEstados_actualizar();

  presionar(BTN_FIN_CARRERA_TRANSICION);
  maquinaEstados_actualizar();
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_ROTANDO_APERTURA, "fin de carrera de transicion avanza a ROTANDO_APERTURA");
  soltar(BTN_FIN_CARRERA_TRANSICION);
  maquinaEstados_actualizar();

  mock_avanzarTiempoMs(1600); // > DURACION_ROTACION_MS (1500)
  maquinaEstados_actualizar();
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_SIDESTEP_X, "tiempo de rotacion cumplido avanza a SIDESTEP_X");

  mock_avanzarTiempoMs(900); // > DURACION_SIDESTEP_MS (800)
  maquinaEstados_actualizar();
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_RETRAYENDO_Z, "tiempo de sidestep cumplido avanza a RETRAYENDO_Z");

  mock_avanzarTiempoMs(1600); // > DURACION_RETRACCION_MS (1500)
  maquinaEstados_actualizar();
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_VOLVIENDO_HOME, "tiempo de retraccion cumplido avanza a VOLVIENDO_HOME");

  mock_avanzarTiempoMs(1100); // > DURACION_VOLVIENDO_HOME_MS (1000)
  maquinaEstados_actualizar();
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_REPOSO, "tiempo de vuelta a home cumplido vuelve a REPOSO");
  VERIFICAR(modo_actual() == MODO_MANUAL, "al volver a REPOSO el modo vuelve solo a MANUAL");
}

static void test_pedidoIgnoradoConRutinaEnCurso() {
  std::printf("test_pedidoIgnoradoConRutinaEnCurso\n");
  reiniciar();

  maquinaEstados_solicitarPedido(1);
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_MOVIENDO_X, "primer pedido arranca la rutina");

  maquinaEstados_solicitarPedido(99);
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_MOVIENDO_X, "un segundo pedido no debe alterar el estado mientras hay una rutina en curso");
  VERIFICAR(maquinaEstados_columnaSolicitada() == 1, "la columna pedida no cambia si el segundo pedido se ignora");
}

static void test_emergenciaConPrioridad() {
  std::printf("test_emergenciaConPrioridad\n");
  reiniciar();

  maquinaEstados_solicitarPedido(2);
  presionar(BTN_FIN_CARRERA_X);
  maquinaEstados_actualizar(); // ahora en EXTENDIENDO_Z, motor del vástago "en movimiento"

  presionar(BTN_EMERGENCIA);
  maquinaEstados_actualizar();
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_ERROR_SEGURO, "la emergencia fuerza ERROR_SEGURO desde cualquier estado");
  VERIFICAR(mock_pinEscrito(LED_MOTOR_X) == LOW, "motor X queda detenido en emergencia");
  VERIFICAR(mock_pinEscrito(LED_MOTOR_VASTAGO) == LOW, "motor del vastago queda detenido en emergencia");
  VERIFICAR(mock_pinEscrito(LED_ERROR) == HIGH, "el LED de error se enciende");

  presionar(BTN_PEDIDO);
  maquinaEstados_actualizar();
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_ERROR_SEGURO, "no debe resetear mientras la emergencia sigue presionada");

  soltar(BTN_EMERGENCIA);
  maquinaEstados_actualizar(); // BTN_PEDIDO sigue presionado de antes
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_REPOSO, "soltar emergencia con el pedido ya presionado resetea a REPOSO");
  VERIFICAR(modo_actual() == MODO_MANUAL, "el reset de emergencia devuelve el control a modo MANUAL");
}

static void test_antirrebote() {
  std::printf("test_antirrebote\n");
  reiniciar();

  maquinaEstados_solicitarPedido(0); // pasa a MOVIENDO_X

  mock_setPin(BTN_FIN_CARRERA_X, LOW);
  sensores_actualizar();
  mock_avanzarTiempoMs(10); // menos que DEBOUNCE_MS (30)
  sensores_actualizar();
  maquinaEstados_actualizar();
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_MOVIENDO_X, "un pulso mas corto que el antirrebote no debe registrarse");

  mock_avanzarTiempoMs(40); // ahora sí supera el antirrebote total
  sensores_actualizar();
  maquinaEstados_actualizar();
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_EXTENDIENDO_Z, "una vez superado el antirrebote, el fin de carrera si se registra");
}

int main() {
  test_estadoInicial();
  test_cicloCompletoFeliz();
  test_pedidoIgnoradoConRutinaEnCurso();
  test_emergenciaConPrioridad();
  test_antirrebote();

  std::printf("\n%d/%d tests OK\n", totalTests - fallos, totalTests);
  return fallos == 0 ? 0 : 1;
}

// Tests nativos (sin ESP32) de la máquina de estados real del firmware.
// Se compilan los .cpp de producción tal cual (ver test/run_tests.sh),
// mockeando solo las primitivas de Arduino (test/mocks/Arduino.h) y de
// ESP32Servo (test/mocks/ESP32Servo.h). Ver test/README.md.

#include <cstdio>
#include <cstdlib>
#include "Arduino.h"
#include "ESP32Servo.h"
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

// Simula presionar/soltar un botón o sensor respetando el antirrebote real
// de sensores.cpp: hace falta que el pin quede estable por más de
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

// Simula que el robot pasa por una marca de columna (un flanco completo:
// presionar + soltar), para que maquina_estados.cpp cuente un flanco más.
static void pasarMarcaColumna() {
  presionar(PIN_SENSOR_COLUMNA);
  maquinaEstados_actualizar();
  soltar(PIN_SENSOR_COLUMNA);
  maquinaEstados_actualizar();
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

  maquinaEstados_solicitarPedido(3, 1);
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_MOVIENDO_X, "pedido desde REPOSO pasa a MOVIENDO_X");
  VERIFICAR(modo_actual() == MODO_AUTONOMO, "pedido cambia el modo a AUTONOMO");
  VERIFICAR(maquinaEstados_columnaSolicitada() == 3, "guarda la columna pedida");
  VERIFICAR(maquinaEstados_lineaSolicitada() == 1, "guarda la línea pedida");

  // Columna 3: hacen falta 3 marcas del sensor óptico para llegar.
  pasarMarcaColumna();
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_MOVIENDO_X, "con 1 marca de 3 sigue en MOVIENDO_X");
  pasarMarcaColumna();
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_MOVIENDO_X, "con 2 marcas de 3 sigue en MOVIENDO_X");
  pasarMarcaColumna();
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_ORIENTANDO_LINEA, "con 3 marcas avanza a ORIENTANDO_LINEA");

  mock_avanzarTiempoMs(1100); // > DURACION_ORIENTACION_MS (1000)
  maquinaEstados_actualizar();
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_EXTENDIENDO_Z, "tiempo de orientación cumplido avanza a EXTENDIENDO_Z");
  VERIFICAR(mock_anguloServo() == 90, "el servo quedó en el ángulo de la línea 1 (placeholder)");

  mock_avanzarTiempoMs(1300); // > DURACION_EXTENSION_MS (1200)
  maquinaEstados_actualizar();
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_ABRIENDO, "tiempo de extensión cumplido avanza a ABRIENDO");

  mock_avanzarTiempoMs(1300); // > DURACION_APERTURA_MS (1200)
  maquinaEstados_actualizar();
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_SIDESTEP_X, "tiempo de apertura cumplido avanza a SIDESTEP_X");

  mock_avanzarTiempoMs(900); // > DURACION_SIDESTEP_MS (800)
  maquinaEstados_actualizar();
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_VOLVIENDO_HOME, "tiempo de sidestep cumplido avanza a VOLVIENDO_HOME");

  mock_avanzarTiempoMs(1100); // > DURACION_VOLVIENDO_HOME_MS (1000)
  maquinaEstados_actualizar();
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_REPOSO, "tiempo de vuelta a home cumplido vuelve a REPOSO");
  VERIFICAR(modo_actual() == MODO_MANUAL, "al volver a REPOSO el modo vuelve solo a MANUAL");
  VERIFICAR(mock_anguloServo() == 0, "al volver a REPOSO el servo vuelve a la línea home");
}

static void test_pedidoIgnoradoConRutinaEnCurso() {
  std::printf("test_pedidoIgnoradoConRutinaEnCurso\n");
  reiniciar();

  maquinaEstados_solicitarPedido(1, 0);
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_MOVIENDO_X, "primer pedido arranca la rutina");

  maquinaEstados_solicitarPedido(99, 1);
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_MOVIENDO_X, "un segundo pedido no debe alterar el estado mientras hay una rutina en curso");
  VERIFICAR(maquinaEstados_columnaSolicitada() == 1, "la columna pedida no cambia si el segundo pedido se ignora");
  VERIFICAR(maquinaEstados_lineaSolicitada() == 0, "la línea pedida no cambia si el segundo pedido se ignora");
}

static void test_emergenciaConPrioridad() {
  std::printf("test_emergenciaConPrioridad\n");
  reiniciar();

  maquinaEstados_solicitarPedido(2, 0);
  pasarMarcaColumna();
  pasarMarcaColumna(); // 2 marcas -> ORIENTANDO_LINEA
  mock_avanzarTiempoMs(1100);
  maquinaEstados_actualizar(); // -> EXTENDIENDO_Z, motor Z "en movimiento"

  presionar(PIN_BTN_EMERGENCIA);
  maquinaEstados_actualizar();
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_ERROR_SEGURO, "la emergencia fuerza ERROR_SEGURO desde cualquier estado");
  VERIFICAR(mock_pinEscrito(PIN_X_IN1) == LOW && mock_pinEscrito(PIN_X_IN2) == LOW, "motor X queda detenido en emergencia");
  VERIFICAR(mock_pinEscrito(PIN_Z_IN1) == LOW && mock_pinEscrito(PIN_Z_IN2) == LOW, "motor Z queda detenido en emergencia");
  VERIFICAR(mock_pinEscrito(PIN_LED_ERROR) == HIGH, "el LED de error se enciende");

  presionar(PIN_BTN_PEDIDO);
  maquinaEstados_actualizar();
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_ERROR_SEGURO, "no debe resetear mientras la emergencia sigue presionada");

  soltar(PIN_BTN_EMERGENCIA);
  maquinaEstados_actualizar(); // PIN_BTN_PEDIDO sigue presionado de antes
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_REPOSO, "soltar emergencia con el pedido ya presionado resetea a REPOSO");
  VERIFICAR(modo_actual() == MODO_MANUAL, "el reset de emergencia devuelve el control a modo MANUAL");
}

static void test_antirrebote() {
  std::printf("test_antirrebote\n");
  reiniciar();

  maquinaEstados_solicitarPedido(1, 0); // pasa a MOVIENDO_X, hace falta 1 marca

  mock_setPin(PIN_SENSOR_COLUMNA, LOW);
  sensores_actualizar();
  mock_avanzarTiempoMs(10); // menos que DEBOUNCE_MS (30)
  sensores_actualizar();
  maquinaEstados_actualizar();
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_MOVIENDO_X, "un pulso mas corto que el antirrebote no debe registrarse");

  mock_avanzarTiempoMs(40); // ahora sí supera el antirrebote total
  sensores_actualizar();
  maquinaEstados_actualizar();
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_ORIENTANDO_LINEA, "una vez superado el antirrebote, la marca de columna si se registra");
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

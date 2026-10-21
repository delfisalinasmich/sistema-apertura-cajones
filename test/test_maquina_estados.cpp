// Tests nativos (sin ESP32) de la máquina de estados real del firmware.
// Se compilan los .cpp de producción tal cual (ver test/run_tests.sh),
// mockeando las primitivas de Arduino (test/mocks/Arduino.h), ESP32Servo
// (test/mocks/ESP32Servo.h) y Preferences (test/mocks/Preferences.h).
// Ver test/README.md.
//
// sensor_linea.h (de Coty) no se vuelve a probar acá en profundidad — es un
// módulo ya correcto que se reutiliza tal cual. Estos tests arrancan sin
// calibración guardada (cargarCalibracion() falla), así que detectaLinea()
// usa el umbral por defecto (2048, sin histéresis) — alcanza para simular
// que el sensor "ve" o "no ve" una marca.

#include <cstdio>
#include <cstdlib>
#include "Arduino.h"
#include "ESP32Servo.h"
#include "pines.h"
#include "modo.h"
#include "ruedas.h"
#include "base.h"
#include "servo_brazo.h"
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
  modo_iniciar();
  maquinaEstados_iniciar();
}

// Un "tick" de loop: avanza el reloj y llama a todo lo que main.cpp llama
// siempre (base/servo), más la máquina de estados (en los tests se llama
// siempre, a diferencia de main.cpp que la gatea por modo — el gateo en sí
// no es parte de maquina_estados.cpp, así que no hace falta repetirlo acá
// salvo en el test de control manual).
static void tick(unsigned long ms) {
  mock_avanzarTiempoMs(ms);
  base_actualizar();
  servoBrazo_actualizar();
  maquinaEstados_actualizar();
}

// Simula al robot pasando sobre una marca de columna: sube el valor
// analógico por encima del umbral por defecto (2048) y lo vuelve a bajar,
// generando un flanco completo.
static void pasarMarca() {
  mock_setAnalog(SENSOR_PIN, 3000);
  maquinaEstados_actualizar();
  mock_setAnalog(SENSOR_PIN, 500);
  maquinaEstados_actualizar();
}

// Avanza de a pasoMs hasta llegar al estado esperado o agotar maxPasos.
static void avanzarHastaEstado(EstadoSistema esperado, unsigned long pasoMs, int maxPasos) {
  for (int i = 0; i < maxPasos && maquinaEstados_estadoActual() != esperado; i++) {
    tick(pasoMs);
  }
}

static void test_estadoInicial() {
  std::printf("test_estadoInicial\n");
  reiniciar();
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_REPOSO, "estado inicial debe ser REPOSO");
  VERIFICAR(modo_actual() == MODO_MANUAL, "modo inicial debe ser MANUAL");
  VERIFICAR(mock_anguloServo() == 40, "el servo arranca en la línea de reposo (ANG_ATRAS=40)");
}

static void test_cicloCompletoFeliz() {
  std::printf("test_cicloCompletoFeliz\n");
  reiniciar();

  VERIFICAR(abrirCajon(LADO_DERECHA, 3), "abrirCajon acepta el pedido desde REPOSO");
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_MOVIENDO_X, "pasa a MOVIENDO_X");
  VERIFICAR(modo_actual() == MODO_AUTONOMO, "cambia el modo a AUTONOMO");

  pasarMarca();
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_MOVIENDO_X, "con 1 marca de 3 sigue en MOVIENDO_X");
  pasarMarca();
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_MOVIENDO_X, "con 2 marcas de 3 sigue en MOVIENDO_X");
  pasarMarca();
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_ORIENTANDO_BASE, "con 3 marcas avanza a ORIENTANDO_BASE");

  // 7 pulsos de ida (~1950ms) — margen generoso de 3000ms.
  avanzarHastaEstado(ESTADO_EXTENDIENDO, 10, 300);
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_EXTENDIENDO, "la base termina los pulsos de ida y pasa a EXTENDIENDO");

  // Servo 40->90 (~255ms) + espera de contacto (500ms) — margen de 1500ms.
  avanzarHastaEstado(ESTADO_ABRIENDO, 15, 100);
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_ABRIENDO, "el servo llega adelante y, tras la espera de contacto, pasa a ABRIENDO");
  VERIFICAR(mock_anguloServo() == 90, "el servo quedó en el ángulo de contacto (ANG_ADELANTE=90)");

  // Servo 90->40 despacio (~1500ms) + espera de abierto (500ms) — margen de 2250ms.
  avanzarHastaEstado(ESTADO_VOLVIENDO_BASE, 15, 150);
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_VOLVIENDO_BASE, "el servo vuelve atrás y, tras la espera, pasa a VOLVIENDO_BASE");
  VERIFICAR(mock_anguloServo() == 40, "el servo volvió a la posición de reposo");

  // 6 pulsos de vuelta (~1650ms) — margen de 3000ms.
  avanzarHastaEstado(ESTADO_SIDESTEP_X, 10, 300);
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_SIDESTEP_X, "la base termina los pulsos de vuelta y pasa a SIDESTEP_X");

  // DURACION_SIDESTEP_MS fijo (800ms) — margen de 2000ms.
  avanzarHastaEstado(ESTADO_VOLVIENDO_HOME, 10, 200);
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_VOLVIENDO_HOME, "tiempo de sidestep cumplido avanza a VOLVIENDO_HOME");

  pasarMarca();
  pasarMarca();
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_VOLVIENDO_HOME, "con 2 marcas de 3 de vuelta sigue en VOLVIENDO_HOME");
  pasarMarca();
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_REPOSO, "con las 3 marcas de vuelta llega a home y pasa a REPOSO");
  VERIFICAR(modo_actual() == MODO_MANUAL, "al volver a REPOSO el modo vuelve solo a MANUAL");
}

static void test_pedidoIgnoradoConRutinaEnCurso() {
  std::printf("test_pedidoIgnoradoConRutinaEnCurso\n");
  reiniciar();

  VERIFICAR(abrirCajon(LADO_DERECHA, 1), "primer pedido arranca la rutina");
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_MOVIENDO_X, "queda en MOVIENDO_X");

  VERIFICAR(!abrirCajon(LADO_IZQUIERDA, 99), "un segundo pedido con rutina en curso se rechaza (false)");
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_MOVIENDO_X, "el estado no cambia por el pedido rechazado");

  // Si el pedido rechazado hubiera pisado la columna (99), haría falta 99
  // marcas para avanzar; con 1 sola alcanza, confirma que no la pisó.
  pasarMarca();
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_ORIENTANDO_BASE, "la columna pedida sigue siendo 1 (la del primer pedido)");
}

static void test_emergenciaConPrioridad() {
  std::printf("test_emergenciaConPrioridad\n");
  reiniciar();

  abrirCajon(LADO_DERECHA, 2);
  pasarMarca();
  pasarMarca(); // -> ORIENTANDO_BASE, la base está pulsando
  tick(10);
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_ORIENTANDO_BASE, "llegó a ORIENTANDO_BASE con la base en movimiento");

  parar();
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_ERROR_SEGURO, "parar() fuerza ERROR_SEGURO desde cualquier estado");
  VERIFICAR(mock_pinEscrito(BASE_IN_A) == LOW && mock_pinEscrito(BASE_IN_B) == LOW, "la base queda detenida");
  VERIFICAR(mock_pinEscrito(RUEDAS_IN_A) == LOW && mock_pinEscrito(RUEDAS_IN_B) == LOW, "las ruedas quedan detenidas");
  VERIFICAR(!base_activa(), "la base no sigue pulsando tras la emergencia");

  VERIFICAR(!abrirCajon(LADO_DERECHA, 1), "un pedido durante ERROR_SEGURO se rechaza");

  irAHome();
  VERIFICAR(maquinaEstados_estadoActual() == ESTADO_REPOSO, "irAHome() resetea desde ERROR_SEGURO a REPOSO");
  VERIFICAR(modo_actual() == MODO_MANUAL, "el reset devuelve el control a modo MANUAL");
}

static void test_controlManualBase() {
  std::printf("test_controlManualBase\n");
  reiniciar();

  VERIFICAR(modo_actual() == MODO_MANUAL, "arranca en modo MANUAL");
  girarBase(LADO_DERECHA);
  VERIFICAR(base_activa(), "un pulso manual arranca la base");

  for (int i = 0; i < 50 && base_activa(); i++) {
    mock_avanzarTiempoMs(10);
    base_actualizar();
  }
  VERIFICAR(!base_activa(), "el pulso manual termina solo (girar + freno)");

  // Durante una rutina autónoma, girarBase() no debe tener efecto.
  abrirCajon(LADO_DERECHA, 1); // -> MOVIENDO_X, modo AUTONOMO
  girarBase(LADO_IZQUIERDA);
  VERIFICAR(!base_activa(), "girarBase() se ignora fuera de modo MANUAL");
}

int main() {
  test_estadoInicial();
  test_cicloCompletoFeliz();
  test_pedidoIgnoradoConRutinaEnCurso();
  test_emergenciaConPrioridad();
  test_controlManualBase();

  std::printf("\n%d/%d tests OK\n", totalTests - fallos, totalTests);
  return fallos == 0 ? 0 : 1;
}

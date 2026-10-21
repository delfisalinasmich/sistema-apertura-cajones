#include <Arduino.h>
#include "pines.h"
#include "modo.h"
#include "ruedas.h"
#include "base.h"
#include "servo_brazo.h"
#include "maquina_estados.h"
#include "servidor_web.h"

// Programa principal (entorno nodemcu-32s): une la máquina de estados de la
// rutina autónoma, el modo manual vía interfaz web, y el reparto de control
// entre ambos. Ver firmware/README.md.
//
// No hay botón físico de emergencia dedicado todavía (ver
// docs/pendientes.md): la parada de emergencia es el comando Serial "x",
// igual que en src/prueba.cpp — se procesa sin esperar Enter, para que sea
// inmediata.

static EstadoSistema estadoAnterior = ESTADO_REPOSO;
static ModoOperacion modoAnterior = MODO_MANUAL;

static void leerSerie() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == 'x') {
      parar();
      Serial.println("PARADA DE EMERGENCIA");
    }
  }
}

void setup() {
  motorPinesSeguros(); // lo primero: todos los IN en OUTPUT y LOW
  Serial.begin(115200);
  modo_iniciar();
  maquinaEstados_iniciar();
  servidorWeb_iniciar();
  Serial.println("Listo. Modo: MANUAL, Estado: REPOSO");
  Serial.println("Conectate al WiFi \"CajonesBot\" y abri http://192.168.4.1");
  Serial.println("Comando por Serial: x = parada de emergencia");
}

void loop() {
  servidorWeb_actualizar();
  leerSerie();

  // base_actualizar() y servoBrazo_actualizar() avanzan sus propias
  // máquinas de estados no bloqueantes (pulsos de la base, movimiento
  // gradual del servo) sin importar el modo — si no se llamaran siempre,
  // un pulso manual de la base (girarBase(), desde /manual/base) o un
  // movimiento manual del servo (/manual/servo) se quedarían a mitad de
  // camino mientras el sistema está en modo manual.
  base_actualizar();
  servoBrazo_actualizar();

  // La máquina de estados de la rutina autónoma corre siempre que haya una
  // rutina en curso, o mientras el sistema esté en parada de emergencia
  // (para poder salir de ERROR_SEGURO sin importar el modo activo). Fuera
  // de eso, en modo manual, ruedas/base/servo se controlan directo desde
  // los handlers de servidor_web.cpp.
  bool debeActualizarFSM = (modo_actual() == MODO_AUTONOMO)
                         || (maquinaEstados_estadoActual() == ESTADO_ERROR_SEGURO);
  if (debeActualizarFSM) {
    maquinaEstados_actualizar();
  }

  EstadoSistema estadoActual = maquinaEstados_estadoActual();
  if (estadoActual != estadoAnterior) {
    Serial.print("Estado: ");
    Serial.println(estado());
    estadoAnterior = estadoActual;
  }

  ModoOperacion modoActualAhora = modo_actual();
  if (modoActualAhora != modoAnterior) {
    Serial.print("Modo: ");
    Serial.println(modo_nombre(modoActualAhora));
    modoAnterior = modoActualAhora;
  }
}

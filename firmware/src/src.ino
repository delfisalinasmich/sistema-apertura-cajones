#include "pines.h"
#include "motores.h"
#include "sensores.h"
#include "seguridad.h"
#include "modo.h"
#include "maquina_estados.h"
#include "servidor_web.h"

static EstadoSistema estadoAnterior = ESTADO_REPOSO;
static ModoOperacion modoAnterior = MODO_MANUAL;
static bool pedidoPresionadoAntes = false;

void setup() {
  Serial.begin(115200);
  motores_iniciar();
  sensores_iniciar();
  seguridad_iniciar();
  modo_iniciar();
  maquinaEstados_iniciar();
  servidorWeb_iniciar();
  Serial.println("Listo. Modo: MANUAL, Estado: REPOSO");
  Serial.println("Conectate al WiFi \"CajonesBot\" y abri http://192.168.4.1");
}

void loop() {
  servidorWeb_actualizar();
  sensores_actualizar();

  // La máquina de estados corre siempre que haya una rutina autónoma en
  // curso, o mientras el sistema esté en parada de emergencia (para poder
  // entrar/salir de ESTADO_ERROR_SEGURO sin importar el modo activo). Fuera
  // de eso, en modo manual, los motores se controlan directo desde los
  // handlers de servidor_web.cpp.
  bool debeActualizarFSM = (modo_actual() == MODO_AUTONOMO)
                         || (maquinaEstados_estadoActual() == ESTADO_ERROR_SEGURO);

  if (debeActualizarFSM) {
    maquinaEstados_actualizar();
  }

  // Botón físico de pedido (equivalente al /pedido de la web), útil para
  // probar el ciclo completo en el banco sin depender del WiFi. Se lee por
  // FLANCO, no por nivel: el reset de una emergencia exige mantener este
  // mismo botón presionado (ver seguridad_resetSolicitado()), así que si se
  // leyera por nivel, el pedido se volvería a disparar solo apenas se
  // resetea la emergencia, sin que el operador lo haya pedido de nuevo. El
  // flanco se rastrea en cada vuelta del loop (no solo cuando
  // debeActualizarFSM es falso) para que quede "consumido" mientras el
  // botón se mantuvo presionado durante la emergencia.
  bool pedidoAhora = sensores_pedidoPresionado();
  bool flancoPedido = pedidoAhora && !pedidoPresionadoAntes;
  pedidoPresionadoAntes = pedidoAhora;

  if (!debeActualizarFSM && flancoPedido) {
    maquinaEstados_solicitarPedido(0);
  }

  EstadoSistema estadoActual = maquinaEstados_estadoActual();
  if (estadoActual != estadoAnterior) {
    Serial.print("Estado: ");
    Serial.println(maquinaEstados_nombre(estadoActual));
    estadoAnterior = estadoActual;
  }

  ModoOperacion modoActualAhora = modo_actual();
  if (modoActualAhora != modoAnterior) {
    Serial.print("Modo: ");
    Serial.println(modo_nombre(modoActualAhora));
    modoAnterior = modoActualAhora;
  }
}

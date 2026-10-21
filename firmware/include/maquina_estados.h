#pragma once

// Máquina de estados de la rutina autónoma (ver docs/diagrama-flujo-estados.md),
// sobre los actuadores reales: ruedas (X), base giratoria (lado) y servo
// del brazo (apertura). API pública con los nombres que ya había propuesto
// Coty en su guía, para no inventar otros.

enum Lado {
  LADO_IZQUIERDA,
  LADO_DERECHA
};

enum EstadoSistema {
  ESTADO_REPOSO,
  ESTADO_MOVIENDO_X,
  ESTADO_ORIENTANDO_BASE,
  ESTADO_EXTENDIENDO,
  ESTADO_ABRIENDO,
  ESTADO_VOLVIENDO_BASE,
  ESTADO_SIDESTEP_X,
  ESTADO_VOLVIENDO_HOME,
  ESTADO_ERROR_SEGURO
};

void maquinaEstados_iniciar();
void maquinaEstados_actualizar();
EstadoSistema maquinaEstados_estadoActual();

// --- API pública ---

// Dispara la rutina para abrir el cajón n (columna, contada en franjas
// desde home) del lado pedido. Devuelve false si hay una rutina en curso o
// el sistema está en ERROR_SEGURO (el pedido se ignora, no se encola).
bool abrirCajon(Lado lado, int n);

// Manual: un pulso de la base hacia ese lado. Solo tiene efecto en
// MODO_MANUAL y fuera de ERROR_SEGURO.
void girarBase(Lado lado);

// Manual: dispara la vuelta a home. También es la forma de salir de
// ERROR_SEGURO (no hay botón físico de emergencia dedicado todavía — ver
// docs/pendientes.md — así que "ir a home" hace de reset).
void irAHome();

// Parada de emergencia: corta todo de inmediato y pasa a ERROR_SEGURO.
void parar();

// Nombre del estado actual, para mostrar en Serial o en /estado.
const char* estado();

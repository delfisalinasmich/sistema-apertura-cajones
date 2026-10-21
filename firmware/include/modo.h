#pragma once

// Modo de operación a nivel de todo el sistema, separado del estado interno
// de la máquina de estados autónoma (ver maquina_estados.h). La cátedra pide
// "modo de control manual" y "modo autónomo" como dos requisitos distintos.

enum ModoOperacion {
  MODO_MANUAL,
  MODO_AUTONOMO
};

void modo_iniciar();
ModoOperacion modo_actual();
void modo_establecer(ModoOperacion nuevoModo);
const char* modo_nombre(ModoOperacion modo);

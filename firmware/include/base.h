#pragma once

// Control no bloqueante de la base giratoria (orienta el brazo hacia el
// lado izquierdo o derecho). No tiene sensor ni encoder: se mueve por
// trenes de pulsos de tiempo fijo, con freno activo entre pulsos, igual que
// ya se probó físicamente en src/prueba.cpp (de donde sale esta lógica).

void base_iniciar();
void base_actualizar();

// Arranca un tren de n pulsos iguales en un mismo sentido (no bloqueante).
void base_girarPulsos(bool izquierda, int n);

// true mientras está girando o frenando (incluye las pausas entre pulsos).
bool base_activa();

// Corte inmediato, sin frenar (parada de emergencia).
void base_apagar();

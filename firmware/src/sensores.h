#ifndef SENSORES_H
#define SENSORES_H

#include <Arduino.h>

void sensores_iniciar();
void sensores_actualizar();

// Nota de diseño: para este sketch de banco, BTN_FIN_CARRERA_X se lee de
// forma genérica como "el carro llegó a la posición X objetivo" (se usa en
// MOVIENDO_X, SIDESTEP_X y VOLVIENDO_HOME) y BTN_FIN_CARRERA_TRANSICION como
// "el vástago cruzó el punto de transición de la ranura bayoneta" (se usa en
// EXTENDIENDO_Z). Esto simplifica el mínimo de 2 sensores confirmado en
// docs/sensores-actuadores.md sin inventar hardware que la cátedra no pidió.
// Ver la nota completa en firmware/README.md.

bool sensores_finCarreraX();
bool sensores_finCarreraTransicion();
bool sensores_pedidoPresionado();
bool sensores_emergenciaActiva();

#endif

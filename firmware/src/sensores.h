#ifndef SENSORES_H
#define SENSORES_H

#include <Arduino.h>

void sensores_iniciar();
void sensores_actualizar();

// sensores_marcaColumna(): lectura antirrebotada del sensor óptico de
// columna (ver docs/sensores-actuadores.md). maquina_estados.cpp cuenta
// flancos de esta lectura durante MOVIENDO_X para saber a qué columna
// llegó el robot, ya que un robot móvil no puede confirmar "llegué a la
// columna N" con una sola lectura booleana como lo hacía un fin de carrera
// fijo por columna.
bool sensores_marcaColumna();
bool sensores_pedidoPresionado();
bool sensores_emergenciaActiva();

#endif

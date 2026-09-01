#ifndef MOTORES_H
#define MOTORES_H

// Por ahora los "motores" son LEDs (ver firmware/pines.md): esto se reemplaza
// por el control real del driver de cada motor cuando exista la electrónica,
// sin tocar maquina_estados.cpp.

void motores_iniciar();
void motores_moverX();
void motores_moverVastago();
void motores_pararTodos();

#endif

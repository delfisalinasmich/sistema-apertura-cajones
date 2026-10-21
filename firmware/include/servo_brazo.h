#pragma once

// Movimiento gradual no bloqueante del micro servo del brazo (el que abre
// el cajón: avanza hasta que el imán toca la tuerca, y vuelve despacio
// tirando). Misma lógica que ya se probó físicamente en src/prueba.cpp.

void servoBrazo_iniciar();
void servoBrazo_actualizar();

// Arranca el movimiento gradual hacia "objetivo" (0-180), avanzando de a
// "gradosPorPaso" cada "msPorPaso". No bloqueante: hay que llamar
// servoBrazo_actualizar() en cada vuelta del loop.
void servoBrazo_irA(int objetivo, int gradosPorPaso, unsigned long msPorPaso);

// Movimiento inmediato (sin gradualidad) — uso en parada de emergencia o
// control manual directo.
void servoBrazo_saltoInmediato(int angulo);

bool servoBrazo_moviendo();
int servoBrazo_anguloActual();

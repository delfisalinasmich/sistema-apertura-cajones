#ifndef MOTORES_H
#define MOTORES_H

// Control real de los 3 motores del brazo (ver docs/mecanismo-apertura.md):
// motor X (ruedas), motor Z (brazo adelante/atrás) vía driver tipo L298N, y
// servo de orientación de línea.

// Sentido es relativo a cada eje: en X, POSITIVO = alejarse del home
// (derecha); en Z, POSITIVO = adelante (hacia el cajón). NEGATIVO es el
// sentido contrario en cada caso.
enum Sentido {
  SENTIDO_POSITIVO,
  SENTIDO_NEGATIVO
};

void motores_iniciar();
void motores_moverX(Sentido sentido);
void motores_moverZ(Sentido sentido);
void motores_orientarLinea(int linea);
void motores_pararTodos();

#endif

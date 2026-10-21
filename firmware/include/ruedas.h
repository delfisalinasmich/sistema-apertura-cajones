#pragma once

// Control continuo de las ruedas (traslación en X), vía L298N
// (RUEDAS_IN_A/B en pines.h). A diferencia de la base giratoria, acá no
// hace falta un control por pulsos: las ruedas avanzan sin parar mientras
// dura la fase, y la detención depende del sensor de línea (cantidad de
// franjas) o de un tiempo fijo, no de este módulo.

enum Sentido {
  SENTIDO_ADELANTE,
  SENTIDO_ATRAS
};

void ruedas_iniciar();
void ruedas_mover(Sentido sentido);
void ruedas_parar();

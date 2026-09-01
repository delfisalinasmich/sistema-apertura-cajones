#ifndef SEGURIDAD_H
#define SEGURIDAD_H

// Chequeo de emergencia, con prioridad sobre el resto de la máquina de
// estados: se evalúa antes que cualquier otra transición, desde cualquier
// estado (ver docs/diagrama-flujo-estados.md).

void seguridad_iniciar();
void seguridad_actualizar(bool enErrorSeguro);
bool seguridad_emergenciaActiva();
bool seguridad_resetSolicitado();

#endif

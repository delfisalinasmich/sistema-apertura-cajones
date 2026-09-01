#ifndef SERVIDOR_WEB_H
#define SERVIDOR_WEB_H

// Modo manual vía interfaz web (ver docs/modo-manual.md) + endpoint de
// pedido (/pedido?columna=N). El ESP32 arma su propio Access Point, no
// depende de una red existente.
//
// La parada de emergencia sigue siendo el botón físico (BTN_EMERGENCIA, ver
// pines.md), a propósito: un E-stop no debería depender de que el WiFi o el
// software estén funcionando.

void servidorWeb_iniciar();
void servidorWeb_actualizar();

#endif

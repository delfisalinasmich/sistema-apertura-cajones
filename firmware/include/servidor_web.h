#pragma once

// Modo manual vía interfaz web (ver docs/modo-manual.md) + endpoint de
// pedido (/pedido?columna=N&lado=izq|der). El ESP32 arma su propio Access
// Point, no depende de una red existente.
//
// No hay botón físico de emergencia dedicado todavía (ver
// docs/pendientes.md): la parada de emergencia por ahora es el comando
// Serial "x" (igual que en src/prueba.cpp), a propósito no expuesta en la
// web — un mecanismo de parada segura no debería depender de que el WiFi o
// el software estén funcionando.

void servidorWeb_iniciar();
void servidorWeb_actualizar();

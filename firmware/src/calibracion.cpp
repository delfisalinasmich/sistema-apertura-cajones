#include <Arduino.h>
#include <Preferences.h>
#include <math.h>
#include "pines.h"
#include "sensor_linea.h"

// Calibración del sensor infrarrojo analógico (salida AO) sobre el piso.
//
// CABLEADO:
//   Sensor IR (uno solo): VCC -> 3V3, GND -> GND, señal (AO) -> GPIO34
//     (ADC1, solo entrada; constante SENSOR_PIN en include/pines.h)
//   Motor L298N (include/pines.h): BASE_IN_A -> GPIO27, BASE_IN_B -> GPIO26,
//     RUEDAS_IN_A -> GPIO32, RUEDAS_IN_B -> GPIO33. ENA/ENB con jumper, sin
//     cable al ESP32. Acá los IN solo se dejan en LOW; el sensor no usa
//     ningún pin del motor.
//
// Comandos por el monitor serie (115200):
//   b  -> mide 200 lecturas sobre la LINEA NEGRA
//   w  -> mide 200 lecturas sobre el PISO (sin línea)
//   r  -> lecturas crudas (promedio de 20), para probar altura/trimmer
//   l  -> modo en vivo (LINEA / -----), otra tecla lo detiene
//   s  -> muestra los valores guardados
// Con negro y piso medidos calcula el umbral (promedio), la polaridad
// (negro > blanco o al revés) y los guarda en flash.

const int MUESTRAS = 200;        // lecturas por medición
const float DESVIO_ALTO = 50.0;  // desvío estándar (cuentas ADC) considerado ruidoso
const int PEGADO_BAJO = 20;      // promedio <= esto: pegado a 0
const int PEGADO_ALTO = 4075;    // promedio >= esto: pegado a 4095

Preferences prefs;
CalLinea cal;
bool medidoNegro = false;   // medido en esta sesión
bool medidoBlanco = false;
int negroSesion = 0;
int blancoSesion = 0;
bool modoVivo = false;
bool modoCrudo = false;

int leerPromedio(int n) {
  long suma = 0;
  for (int i = 0; i < n; i++) {
    suma += analogRead(SENSOR_PIN);
    delayMicroseconds(500);
  }
  return suma / n;
}

void guardarSiCompleto() {
  if (!medidoNegro || !medidoBlanco) {
    Serial.println("Falta medir el otro (b/w) en esta sesión para calcular el umbral.");
    return;
  }
  cal.negro = negroSesion;
  cal.blanco = blancoSesion;
  cal.umbral = (cal.negro + cal.blanco) / 2;
  cal.negroMayor = cal.negro > cal.blanco;
  cal.valida = true;
  cal.sobreLinea = false;
  int contraste = abs(cal.blanco - cal.negro);
  Serial.printf("negro=%d blanco=%d umbral=%d contraste=%d polaridad=%s\n",
                cal.negro, cal.blanco, cal.umbral, contraste,
                cal.negroMayor ? "negro>blanco" : "negro<blanco");
  if (contraste < CONTRASTE_MIN)
    Serial.printf("  AVISO: contraste < %d. Acercá el sensor (3-10 mm) o ajustá el trimmer.\n", CONTRASTE_MIN);
  guardarCalibracion(prefs, cal);
  Serial.println("Calibración guardada.");
}

void medir(bool esNegro) {
  Serial.println(esNegro ? "Midiendo NEGRO..." : "Midiendo PISO...");
  int minimo = 4095, maximo = 0;
  double suma = 0, suma2 = 0;
  for (int i = 0; i < MUESTRAS; i++) {
    int v = analogRead(SENSOR_PIN);
    if (v < minimo) minimo = v;
    if (v > maximo) maximo = v;
    suma += v;
    suma2 += (double)v * v;
    delayMicroseconds(500);
  }
  double prom = suma / MUESTRAS;
  double var = suma2 / MUESTRAS - prom * prom;
  float desvio = sqrt(var > 0 ? var : 0);
  Serial.printf("  min=%d max=%d promedio=%.1f desvio=%.1f\n", minimo, maximo, prom, desvio);

  if (desvio > DESVIO_ALTO)
    Serial.printf("  AVISO: desvío alto (> %.0f). Lectura inestable: sensor móvil, luz externa o mala conexión.\n", DESVIO_ALTO);
  if (prom <= PEGADO_BAJO || prom >= PEGADO_ALTO)
    Serial.printf("  AVISO: valores pegados a %s. Posible pin flotante o sensor desconectado (revisá VCC/GND/AO en GPIO%d).\n",
                  prom <= PEGADO_BAJO ? "0" : "4095", SENSOR_PIN);

  int v = (int)(prom + 0.5);
  if (esNegro) { negroSesion = v; medidoNegro = true; }
  else { blancoSesion = v; medidoBlanco = true; }
  guardarSiCompleto();
}

void mostrarGuardado() {
  if (!cal.valida) {
    Serial.println("No hay calibración válida guardada (vacía o formato viejo). Recalibrá con w y b.");
    return;
  }
  Serial.printf("negro=%d blanco=%d umbral=%d polaridad=%s\n", cal.negro, cal.blanco, cal.umbral,
                cal.negroMayor ? "negro>blanco" : "negro<blanco");
}

void setup() {
  motorPinesSeguros();
  Serial.begin(115200);
  sensorInit();
  prefs.begin("linea", false);
  Serial.printf("Calibración IR (sensor en GPIO%d): b=negro  w=piso  r=crudo  l=en vivo  s=guardado\n", SENSOR_PIN);
  if (!cargarCalibracion(prefs, cal))
    Serial.println("Sin calibración válida (vacía o formato viejo de 2 sensores/sin polaridad). Recalibrá con w y b.");
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') return;
    if (modoVivo || modoCrudo) {
      modoVivo = modoCrudo = false;
      Serial.println("Modo detenido");
      return;
    }
    switch (c) {
      case 'r': modoCrudo = true; break;
      case 'b': medir(true); break;
      case 'w': medir(false); break;
      case 's': mostrarGuardado(); break;
      case 'l':
        if (!cal.valida) Serial.println("No hay calibración válida. Recalibrá con w y b.");
        else { cal.sobreLinea = false; modoVivo = true; }
        break;
    }
  }

  if (modoCrudo) {
    Serial.printf("GPIO%d=%4d\n", SENSOR_PIN, leerPromedio(20));
    delay(200);
  }

  if (modoVivo) {
    int v = analogRead(SENSOR_PIN);
    bool linea = detectaLinea(cal, v);
    Serial.printf("GPIO%d=%4d %s\n", SENSOR_PIN, v, linea ? "LINEA" : "-----");
    delay(100);
  }
}

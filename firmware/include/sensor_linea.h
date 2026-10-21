#pragma once
#include <Arduino.h>
#include <Preferences.h>
#include "pines.h"

// Sensor infrarrojo analógico ÚNICO (seguidor de línea). Pin en pines.h (SENSOR_PIN = GPIO34).

constexpr int CAL_VERSION = 2;           // formato en flash: 1 sensor + polaridad
constexpr int CONTRASTE_MIN = 200;

struct CalLinea {
  bool valida = false;     // true si hay datos en formato actual
  int negro = 0;
  int blanco = 0;
  int umbral = 2048;
  bool negroMayor = true;  // polaridad: true si la lectura sobre negro > sobre blanco
  bool sobreLinea = false; // estado de la histéresis
};

inline void sensorInit() {
  analogReadResolution(12);
  pinMode(SENSOR_PIN, INPUT);
  analogSetPinAttenuation(SENSOR_PIN, ADC_11db);  // rango completo 0-3.3V
}

// Devuelve false si no hay datos o están en formato viejo (2 sensores / sin polaridad).
inline bool cargarCalibracion(Preferences &prefs, CalLinea &cal) {
  cal = CalLinea();
  if (prefs.getInt("ver", 0) != CAL_VERSION) return false;
  cal.negro = prefs.getInt("neg", -1);
  cal.blanco = prefs.getInt("bla", -1);
  cal.umbral = prefs.getInt("umb", -1);
  if (!prefs.isKey("pol") || cal.negro < 0 || cal.blanco < 0 || cal.umbral < 0) return false;
  cal.negroMayor = prefs.getBool("pol", true);
  cal.valida = true;
  return true;
}

inline void guardarCalibracion(Preferences &prefs, const CalLinea &cal) {
  prefs.putInt("neg", cal.negro);
  prefs.putInt("bla", cal.blanco);
  prefs.putInt("umb", cal.umbral);
  prefs.putBool("pol", cal.negroMayor);
  prefs.putInt("ver", CAL_VERSION);   // al final: si se corta, queda inválido
}

// true si el sensor está sobre la línea, con histéresis y según la polaridad guardada.
inline bool detectaLinea(CalLinea &cal, int valor) {
  int h = abs(cal.negro - cal.blanco) / 4;
  int alto = cal.umbral + h;
  int bajo = cal.umbral - h;
  if (cal.negroMayor) {
    if (valor > alto) cal.sobreLinea = true;
    else if (valor < bajo) cal.sobreLinea = false;
  } else {
    if (valor < bajo) cal.sobreLinea = true;
    else if (valor > alto) cal.sobreLinea = false;
  }
  return cal.sobreLinea;
}

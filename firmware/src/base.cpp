#include "base.h"
#include "pines.h"
#include <Arduino.h>

// Constantes calibradas a mano sobre el hardware real en src/prueba.cpp —
// se reutilizan tal cual, no se adivinan de nuevo.
static const unsigned long GIRO_MANUAL_MS = 50;     // duración de un pulso
static const unsigned long BASE_FRENO_MS = 100;     // freno activo al final de cada pulso
static const unsigned long BASE_PAUSA_PULSO_MS = 150; // pausa entre pulsos
static const bool GIRO_BASE_INVERTIDO = false;      // true si la base gira al revés de lo esperado

static bool baseActiva = false;
static bool baseFrenando = false;
static bool baseEnPausa = false;
static int baseRestantes = 0;
static bool baseDirIzq = false;
static unsigned long tBase = 0;
static unsigned long baseMs = 0;

void base_iniciar() {
  pinMode(BASE_IN_A, OUTPUT);
  pinMode(BASE_IN_B, OUTPUT);
  base_apagar();
}

void base_apagar() {
  digitalWrite(BASE_IN_A, LOW);
  digitalWrite(BASE_IN_B, LOW);
  baseActiva = false;
  baseFrenando = false;
  baseEnPausa = false;
  baseRestantes = 0;
}

static void baseGirar(bool izquierda, unsigned long ms) {
  bool sentido = (izquierda != GIRO_BASE_INVERTIDO);
  digitalWrite(BASE_IN_A, sentido ? HIGH : LOW);
  digitalWrite(BASE_IN_B, sentido ? LOW : HIGH);
  tBase = millis();
  baseMs = ms;
  baseActiva = true;
  baseFrenando = false;
}

static void baseIniciarPulso() {
  baseRestantes--;
  baseGirar(baseDirIzq, GIRO_MANUAL_MS);
}

void base_girarPulsos(bool izquierda, int n) {
  baseDirIzq = izquierda;
  baseRestantes = n;
  baseIniciarPulso();
}

static void baseFinPulso() {
  if (baseRestantes > 0) {
    digitalWrite(BASE_IN_A, LOW);
    digitalWrite(BASE_IN_B, LOW);
    baseFrenando = false;
    baseEnPausa = true;
    tBase = millis();
  } else {
    base_apagar();
  }
}

void base_actualizar() {
  if (!baseActiva) return;
  if (baseEnPausa) {
    if (millis() - tBase < BASE_PAUSA_PULSO_MS) return;
    baseEnPausa = false;
    baseIniciarPulso();
  } else if (!baseFrenando) {
    if (millis() - tBase < baseMs) return;
    if (BASE_FRENO_MS == 0) { baseFinPulso(); return; }
    digitalWrite(BASE_IN_A, HIGH);
    digitalWrite(BASE_IN_B, HIGH);
    baseFrenando = true;
    tBase = millis();
  } else if (millis() - tBase >= BASE_FRENO_MS) {
    baseFinPulso();
  }
}

bool base_activa() {
  return baseActiva;
}

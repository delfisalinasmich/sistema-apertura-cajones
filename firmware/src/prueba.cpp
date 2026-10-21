#include <Arduino.h>
#include <ESP32Servo.h>
#include "pines.h"

// PRUEBA de la mecánica (entorno "prueba"), separada del resto del código.
//
// CABLEADO:
//   Base giratoria (L298N): conectada a GPIO27 (BASE_IN_A) y GPIO26 (BASE_IN_B), solo HIGH/LOW (jumper en el enable)
//   Micro servo: señal -> GPIO25, rojo -> 5V, marrón -> GND (común con el ESP32)
//   RUEDAS_IN_A/B (GPIO32/33) y el sensor IR NO se usan acá: quedan siempre en LOW.
//
// Comandos por el monitor serie (115200, terminar con Enter):
//   t        -> corre la secuencia completa
//   x        -> parada de emergencia (motores en LOW, servo detenido donde está)
//   s<ang>   -> mueve el servo a ese ángulo (ej. s90)
//   g+ / g-  -> un pulso de la base (GIRO_MANUAL_MS) a la izquierda / derecha
//   ?        -> muestra las constantes y el ángulo actual del servo

// ---- Constantes a ajustar ----
const int ANG_ATRAS = 40;      // reposo: brazo hacia atrás (tira del cajón al volver acá)
const int ANG_ADELANTE = 90;   // brazo hacia adelante: el imán toca la tuerca del cajón

// Velocidad del servo: cuántos grados avanza por paso y cada cuántos ms.
const int VEL_SERVO_IDA_GRADOS = 3;       // ida (imán hacia la tuerca)
const int VEL_SERVO_IDA_MS = 15;
const int VEL_SERVO_VUELTA_GRADOS = 1;    // vuelta (tira del cajón): más lenta para que no se despegue
const int VEL_SERVO_VUELTA_MS = 30;

// Base: la secuencia gira con pulsos iguales a los de g+ / g- (GIRO_MANUAL_MS + freno).
const int BASE_PULSOS_IDA = 7;              // pulsos g- (derecha) para llegar al cajón
const int BASE_PULSOS_VUELTA = 6;           // pulsos g+ (izquierda) para volver
const unsigned long BASE_PAUSA_PULSO_MS = 150;  // pausa entre pulsos (la base se detiene del todo)
const unsigned long T_CONTACTO_MS = 500;    // espera con el imán pegado a la tuerca
const unsigned long T_ABIERTO_MS = 500;     // espera con el cajón abierto
const unsigned long PAUSA_ENTRE_PASOS_MS = 300;
const bool GIRO_BASE_INVERTIDO = false;     // true si la base gira al revés de lo esperado

const unsigned long BASE_FRENO_MS = 100;    // tiempo de freno activo al final de cada giro (0 = sin freno)
const unsigned long GIRO_MANUAL_MS = 50;    // duración de un pulso de la base (g+ / g- y secuencia)
const int PULSO_MIN_US = 500;               // rango de pulsos del servo (0° y 180°)
const int PULSO_MAX_US = 2400;

// ---- Servo (movimiento gradual no bloqueante) ----
Servo servo;
int angulo = ANG_ATRAS;
int anguloObjetivo = ANG_ATRAS;
int servoGrados = 1;
unsigned long servoIntervalo = 0;
unsigned long tServo = 0;
bool servoMoviendo = false;

void servoIrA(int objetivo, int grados, unsigned long ms) {
  anguloObjetivo = constrain(objetivo, 0, 180);
  servoGrados = max(1, grados);
  servoIntervalo = ms;
  tServo = millis();
  servoMoviendo = (angulo != anguloObjetivo);
}

void servoActualizar() {
  if (!servoMoviendo || millis() - tServo < servoIntervalo) return;
  tServo = millis();
  int dif = anguloObjetivo - angulo;
  int paso = min(servoGrados, abs(dif));
  angulo += (dif > 0) ? paso : -paso;
  servo.write(angulo);
  if (angulo == anguloObjetivo) servoMoviendo = false;
}

void servoSalto(int ang) {
  angulo = anguloObjetivo = constrain(ang, 0, 180);
  servoMoviendo = false;
  servo.write(angulo);
}

// ---- Base giratoria (BASE_IN_A/B de pines.h), por tiempo ----
constexpr int BASE_PIN_A = BASE_IN_A;  // GPIO27
constexpr int BASE_PIN_B = BASE_IN_B;  // GPIO26
bool baseActiva = false;     // true mientras gira o frena
bool baseFrenando = false;
bool baseEnPausa = false;    // entre pulsos de un tren de pulsos
int baseRestantes = 0;       // pulsos que faltan del tren actual
bool baseDirIzq = false;
unsigned long tBase = 0;
unsigned long baseMs = 0;

// Corte inmediato (parada de emergencia, fin de secuencia): ambos IN en LOW, sin freno.
void baseApagar() {
  digitalWrite(BASE_PIN_A, LOW);
  digitalWrite(BASE_PIN_B, LOW);
  baseActiva = false;
  baseFrenando = false;
  baseEnPausa = false;
  baseRestantes = 0;
}

void baseGirar(bool izquierda, unsigned long ms) {
  bool sentido = (izquierda != GIRO_BASE_INVERTIDO);
  digitalWrite(BASE_PIN_A, sentido ? HIGH : LOW);
  digitalWrite(BASE_PIN_B, sentido ? LOW : HIGH);
  tBase = millis();
  baseMs = ms;
  baseActiva = true;
  baseFrenando = false;
}

// Tren de n pulsos iguales en un mismo sentido, con pausa entre ellos.
void baseIniciarPulso() {
  baseRestantes--;
  baseGirar(baseDirIzq, GIRO_MANUAL_MS);
}

void baseGirarPulsos(bool izquierda, int n) {
  baseDirIzq = izquierda;
  baseRestantes = n;
  baseIniciarPulso();
}

// Fin de un pulso: si quedan más, suelta y espera; si no, termina.
void baseFinPulso() {
  if (baseRestantes > 0) {
    digitalWrite(BASE_PIN_A, LOW);
    digitalWrite(BASE_PIN_B, LOW);
    baseFrenando = false;
    baseEnPausa = true;
    tBase = millis();
  } else {
    baseApagar();
  }
}

// Al terminar el tiempo de giro frena en seco (ambos IN en HIGH) y después suelta.
void baseActualizar() {
  if (!baseActiva) return;
  if (baseEnPausa) {
    if (millis() - tBase < BASE_PAUSA_PULSO_MS) return;
    baseEnPausa = false;
    baseIniciarPulso();
  } else if (!baseFrenando) {
    if (millis() - tBase < baseMs) return;
    if (BASE_FRENO_MS == 0) { baseFinPulso(); return; }
    digitalWrite(BASE_PIN_A, HIGH);
    digitalWrite(BASE_PIN_B, HIGH);
    baseFrenando = true;
    tBase = millis();
  } else if (millis() - tBase >= BASE_FRENO_MS) {
    baseFinPulso();
  }
}

// ---- Secuencia ----
const int PASO_FINAL = 5;
bool corriendo = false;
int paso = 0;
bool esperando = false;          // false = ejecutando la acción del paso, true = en la espera posterior
unsigned long tEspera = 0;
unsigned long esperaMs = 0;

void apagarTodo() {
  baseApagar();
  digitalWrite(RUEDAS_IN_A, LOW);
  digitalWrite(RUEDAS_IN_B, LOW);
}

void iniciarPaso(int i) {
  paso = i;
  esperando = false;
  esperaMs = PAUSA_ENTRE_PASOS_MS;
  switch (i) {
    case 0:
      Serial.println("Paso 1: servo a reposo (atras)...");
      servoIrA(ANG_ATRAS, VEL_SERVO_IDA_GRADOS, VEL_SERVO_IDA_MS);
      break;
    case 1:
      Serial.printf("Paso 2: base a la derecha (%d pulsos g-)...\n", BASE_PULSOS_IDA);
      baseGirarPulsos(false, BASE_PULSOS_IDA);
      break;
    case 2:
      Serial.println("Paso 3: servo hacia adelante (contacto con la tuerca)...");
      servoIrA(ANG_ADELANTE, VEL_SERVO_IDA_GRADOS, VEL_SERVO_IDA_MS);
      esperaMs += T_CONTACTO_MS;
      break;
    case 3:
      Serial.println("Paso 4: servo hacia atras, despacio, abre el cajon...");
      servoIrA(ANG_ATRAS, VEL_SERVO_VUELTA_GRADOS, VEL_SERVO_VUELTA_MS);
      esperaMs += T_ABIERTO_MS;
      break;
    case 4:
      Serial.printf("Paso 5: base vuelve a la izquierda (%d pulsos g+)...\n", BASE_PULSOS_VUELTA);
      baseGirarPulsos(true, BASE_PULSOS_VUELTA);
      break;
    case PASO_FINAL:
      Serial.println("Paso 6: todo en LOW, servo en reposo...");
      apagarTodo();
      servoIrA(ANG_ATRAS, VEL_SERVO_IDA_GRADOS, VEL_SERVO_IDA_MS);
      corriendo = false;
      Serial.println("PRUEBA TERMINADA");
      break;
  }
}

bool accionTerminada() {
  switch (paso) {
    case 1: case 4: return !baseActiva;
    default: return !servoMoviendo;
  }
}

void secuenciaActualizar() {
  if (!corriendo) return;
  if (!esperando) {
    if (!accionTerminada()) return;
    esperando = true;
    tEspera = millis();
  } else if (millis() - tEspera >= esperaMs) {
    iniciarPaso(paso + 1);
  }
}

void paradaEmergencia() {
  apagarTodo();
  servoMoviendo = false;
  corriendo = false;
  Serial.printf("PARADA: motores en LOW, servo detenido en %d grados\n", angulo);
}

void mostrarConstantes() {
  Serial.printf("Servo actual: %d grados%s\n", angulo, servoMoviendo ? " (moviendose)" : "");
  Serial.printf("ANG_ATRAS=%d ANG_ADELANTE=%d\n", ANG_ATRAS, ANG_ADELANTE);
  Serial.printf("IDA: %d grados cada %d ms | VUELTA: %d grados cada %d ms\n",
                VEL_SERVO_IDA_GRADOS, VEL_SERVO_IDA_MS, VEL_SERVO_VUELTA_GRADOS, VEL_SERVO_VUELTA_MS);
  Serial.printf("T_CONTACTO_MS=%lu T_ABIERTO_MS=%lu PAUSA_ENTRE_PASOS_MS=%lu\n",
                T_CONTACTO_MS, T_ABIERTO_MS, PAUSA_ENTRE_PASOS_MS);
  Serial.printf("BASE: pulsos ida=%d vuelta=%d, pulso=%lu ms, freno=%lu ms, pausa=%lu ms, invertido=%s\n",
                BASE_PULSOS_IDA, BASE_PULSOS_VUELTA, GIRO_MANUAL_MS, BASE_FRENO_MS, BASE_PAUSA_PULSO_MS,
                GIRO_BASE_INVERTIDO ? "true" : "false");
}

void procesarComando(String cmd) {
  cmd.trim();
  cmd.toLowerCase();
  if (cmd.length() == 0) return;

  if (cmd == "x") { paradaEmergencia(); return; }
  if (cmd == "?") { mostrarConstantes(); return; }

  if (corriendo) {
    Serial.println("Secuencia en curso: solo funciona x (parada de emergencia).");
    return;
  }

  if (cmd == "t") {
    Serial.println("Iniciando secuencia (x para parar)...");
    corriendo = true;
    iniciarPaso(0);
  } else if (cmd == "g+" || cmd == "g-") {
    bool izquierda = (cmd == "g+");
    Serial.printf("Base %s %lu ms\n", izquierda ? "izquierda" : "derecha", GIRO_MANUAL_MS);
    baseGirar(izquierda, GIRO_MANUAL_MS);
  } else if (cmd[0] == 's' && cmd.length() > 1) {
    int ang = cmd.substring(1).toInt();
    Serial.printf("Servo a %d grados\n", constrain(ang, 0, 180));
    servoSalto(ang);
  } else {
    Serial.println("Comando desconocido. Usá: t, x, s<angulo>, g+, g-, ?");
  }
}

String lineaSerie;
unsigned long tUltimoCaracter = 0;
const unsigned long SERIE_SIN_ENTER_MS = 50;  // sin Enter: el comando se ejecuta tras este silencio

void leerSerie() {
  while (Serial.available()) {
    char c = Serial.read();
    tUltimoCaracter = millis();
    if (c == '\n' || c == '\r') {
      procesarComando(lineaSerie);
      lineaSerie = "";
    } else if (lineaSerie.length() < 32) {
      lineaSerie += c;
    }
    // "x" actúa sin esperar el Enter para que la parada sea inmediata
    if (lineaSerie == "x") {
      procesarComando(lineaSerie);
      lineaSerie = "";
    }
  }
  // Monitores que no mandan Enter (ej. el de VS Code sin fin de línea configurado)
  if (lineaSerie.length() > 0 && millis() - tUltimoCaracter >= SERIE_SIN_ENTER_MS) {
    procesarComando(lineaSerie);
    lineaSerie = "";
  }
}

void setup() {
  motorPinesSeguros();  // lo primero: todos los IN en OUTPUT y LOW (ruedas incluidas)
  Serial.begin(115200);
  servo.setPeriodHertz(50);
  servo.attach(SERVO_PIN, PULSO_MIN_US, PULSO_MAX_US);
  servoSalto(ANG_ATRAS);
  Serial.println("PRUEBA mecanica lista. t=secuencia x=parar s<ang> g+ g- ?=constantes");
}

void loop() {
  leerSerie();
  servoActualizar();
  baseActualizar();
  secuenciaActualizar();
}

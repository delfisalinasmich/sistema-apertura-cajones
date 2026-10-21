#include "servidor_web.h"
#include <WiFi.h>
#include <WebServer.h>
#include "modo.h"
#include "ruedas.h"
#include "maquina_estados.h"
#include "servo_brazo.h"

static const char* AP_SSID = "CajonesBot";
static const char* AP_PASSWORD = "AbreCajon26";

static WebServer server(80);

static const char PAGINA_HTML[] PROGMEM = R"HTML(<!DOCTYPE html>
<html lang="es">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Abridor de cajones</title>
<style>
  body { font-family: sans-serif; max-width: 420px; margin: 2rem auto; padding: 0 1rem; }
  h1 { font-size: 1.2rem; }
  button { padding: 0.7rem 1rem; margin: 0.25rem 0.25rem 0.25rem 0; font-size: 1rem; }
  .fila { margin-bottom: 1.2rem; }
  #estado { font-weight: bold; }
</style>
</head>
<body>
  <h1>Abridor de cajones</h1>
  <p>Modo: <span id="modo">-</span> &mdash; Estado: <span id="estado">-</span></p>
  <p><small>La parada de emergencia es el comando "x" por Serial, no depende de esta p&aacute;gina.</small></p>

  <div class="fila">
    <strong>Movimiento manual (ruedas)</strong><br>
    <button onclick="cmd('/manual/ruedas?dir=izq')">&#9664; Izquierda</button>
    <button onclick="cmd('/manual/ruedas?dir=parar')">Parar</button>
    <button onclick="cmd('/manual/ruedas?dir=der')">Derecha &#9654;</button>
  </div>

  <div class="fila">
    <strong>Base (pulso manual)</strong><br>
    <button onclick="cmd('/manual/base?dir=izq')">&#8630; Izquierda</button>
    <button onclick="cmd('/manual/base?dir=der')">Derecha &#8631;</button>
  </div>

  <div class="fila">
    <strong>Pedido (rutina aut&oacute;noma)</strong><br>
    <input type="number" id="columna" value="0" min="0" style="width:4rem"> columna
    <select id="lado"><option value="der">derecha</option><option value="izq">izquierda</option></select><br>
    <button onclick="pedir()">Abrir caj&oacute;n</button>
    <button onclick="cmd('/home')">Ir a home</button>
  </div>

<script>
function cmd(ruta) {
  fetch(ruta).catch(function () {});
}
function pedir() {
  var columna = document.getElementById('columna').value;
  var lado = document.getElementById('lado').value;
  cmd('/pedido?columna=' + columna + '&lado=' + lado);
}
function actualizarEstado() {
  fetch('/estado').then(function (r) { return r.text(); }).then(function (t) {
    var partes = t.split('|');
    document.getElementById('modo').textContent = partes[0] || '-';
    document.getElementById('estado').textContent = partes[1] || '-';
  }).catch(function () {});
}
setInterval(actualizarEstado, 1000);
actualizarEstado();
</script>
</body>
</html>
)HTML";

static bool controlManualPermitido() {
  return modo_actual() == MODO_MANUAL && maquinaEstados_estadoActual() != ESTADO_ERROR_SEGURO;
}

static void manejarRaiz() {
  server.send_P(200, "text/html", PAGINA_HTML);
}

static void manejarEstado() {
  String texto = String(modo_nombre(modo_actual())) + "|" + estado();
  server.send(200, "text/plain", texto);
}

static Lado ladoDesdeTexto(const String& texto) {
  return texto == "izq" ? LADO_IZQUIERDA : LADO_DERECHA;
}

static void manejarPedido() {
  int columna = server.hasArg("columna") ? server.arg("columna").toInt() : 0;
  Lado lado = ladoDesdeTexto(server.arg("lado"));
  bool aceptado = abrirCajon(lado, columna);
  server.send(aceptado ? 200 : 409, "text/plain", aceptado ? "OK" : "Ocupado o en ERROR_SEGURO");
}

static void manejarHome() {
  irAHome();
  server.send(200, "text/plain", "OK");
}

static void manejarManualRuedas() {
  if (!controlManualPermitido()) {
    server.send(409, "text/plain", "No disponible en este modo/estado");
    return;
  }
  String dir = server.arg("dir");
  if (dir == "der") {
    ruedas_mover(SENTIDO_ADELANTE);
  } else if (dir == "izq") {
    ruedas_mover(SENTIDO_ATRAS);
  } else {
    ruedas_parar();
  }
  server.send(200, "text/plain", "OK");
}

static void manejarManualBase() {
  if (!controlManualPermitido()) {
    server.send(409, "text/plain", "No disponible en este modo/estado");
    return;
  }
  girarBase(ladoDesdeTexto(server.arg("dir")));
  server.send(200, "text/plain", "OK");
}

static void manejarManualServo() {
  if (!controlManualPermitido()) {
    server.send(409, "text/plain", "No disponible en este modo/estado");
    return;
  }
  int angulo = server.hasArg("ang") ? server.arg("ang").toInt() : servoBrazo_anguloActual();
  servoBrazo_saltoInmediato(angulo);
  server.send(200, "text/plain", "OK");
}

void servidorWeb_iniciar() {
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  server.on("/", manejarRaiz);
  server.on("/estado", manejarEstado);
  server.on("/pedido", manejarPedido);
  server.on("/home", manejarHome);
  server.on("/manual/ruedas", manejarManualRuedas);
  server.on("/manual/base", manejarManualBase);
  server.on("/manual/servo", manejarManualServo);
  server.begin();
}

void servidorWeb_actualizar() {
  server.handleClient();
}

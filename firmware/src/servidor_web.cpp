#include "servidor_web.h"
#include <WiFi.h>
#include <WebServer.h>
#include "modo.h"
#include "motores.h"
#include "maquina_estados.h"

// Ver pines.md para el SSID/clave del AP.
static const char* AP_SSID = "CajonesBot";
static const char* AP_PASSWORD = "AbreCajon26";

static WebServer server(80);

static const char PAGINA_HTML[] PROGMEM = R"HTML(<!DOCTYPE html>
<html lang="es">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Sistema de apertura de cajones</title>
<style>
  body { font-family: sans-serif; max-width: 420px; margin: 2rem auto; padding: 0 1rem; }
  h1 { font-size: 1.2rem; }
  button { padding: 0.7rem 1rem; margin: 0.25rem 0.25rem 0.25rem 0; font-size: 1rem; }
  .fila { margin-bottom: 1.2rem; }
  #estado { font-weight: bold; }
</style>
</head>
<body>
  <h1>Sistema de apertura de cajones</h1>
  <p>Modo: <span id="modo">-</span> &mdash; Estado: <span id="estado">-</span></p>
  <p><small>La parada de emergencia es el bot&oacute;n f&iacute;sico, no depende de esta p&aacute;gina.</small></p>

  <div class="fila">
    <strong>Movimiento manual (X, ruedas)</strong><br>
    <button onclick="cmd('/manual/x?dir=izq')">&#9664; Izquierda</button>
    <button onclick="cmd('/manual/x?dir=parar')">Parar</button>
    <button onclick="cmd('/manual/x?dir=der')">Derecha &#9654;</button>
  </div>

  <div class="fila">
    <strong>Movimiento manual (Z, brazo)</strong><br>
    <button onclick="cmd('/manual/z?dir=adelante')">&#9650; Adelante</button>
    <button onclick="cmd('/manual/z?dir=parar')">Parar</button>
    <button onclick="cmd('/manual/z?dir=atras')">&#9660; Atr&aacute;s</button>
  </div>

  <div class="fila">
    <strong>Orientaci&oacute;n manual (l&iacute;nea)</strong><br>
    <button onclick="cmd('/manual/linea?valor=0')">L&iacute;nea 1</button>
    <button onclick="cmd('/manual/linea?valor=1')">L&iacute;nea 2</button>
  </div>

  <div class="fila">
    <strong>Pedido (rutina aut&oacute;noma)</strong><br>
    <input type="number" id="columna" value="0" min="0" style="width:4rem"> columna
    <input type="number" id="linea" value="0" min="0" style="width:4rem"> l&iacute;nea<br>
    <button onclick="pedir()">Abrir caj&oacute;n</button>
  </div>

<script>
function cmd(ruta) {
  fetch(ruta).catch(function () {});
}
function pedir() {
  var columna = document.getElementById('columna').value;
  var linea = document.getElementById('linea').value;
  cmd('/pedido?columna=' + columna + '&linea=' + linea);
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
  String texto = String(modo_nombre(modo_actual())) + "|" + maquinaEstados_nombre(maquinaEstados_estadoActual());
  server.send(200, "text/plain", texto);
}

static void manejarPedido() {
  int columna = server.hasArg("columna") ? server.arg("columna").toInt() : 0;
  int linea = server.hasArg("linea") ? server.arg("linea").toInt() : 0;
  maquinaEstados_solicitarPedido(columna, linea);
  server.send(200, "text/plain", "OK");
}

static void manejarManualX() {
  if (!controlManualPermitido()) {
    server.send(409, "text/plain", "No disponible en este modo/estado");
    return;
  }
  String dir = server.arg("dir");
  if (dir == "der") {
    motores_moverX(SENTIDO_POSITIVO);
  } else if (dir == "izq") {
    motores_moverX(SENTIDO_NEGATIVO);
  } else {
    motores_pararTodos();
  }
  server.send(200, "text/plain", "OK");
}

static void manejarManualZ() {
  if (!controlManualPermitido()) {
    server.send(409, "text/plain", "No disponible en este modo/estado");
    return;
  }
  String dir = server.arg("dir");
  if (dir == "adelante") {
    motores_moverZ(SENTIDO_POSITIVO);
  } else if (dir == "atras") {
    motores_moverZ(SENTIDO_NEGATIVO);
  } else {
    motores_pararTodos();
  }
  server.send(200, "text/plain", "OK");
}

static void manejarManualLinea() {
  if (!controlManualPermitido()) {
    server.send(409, "text/plain", "No disponible en este modo/estado");
    return;
  }
  int linea = server.hasArg("valor") ? server.arg("valor").toInt() : 0;
  motores_orientarLinea(linea);
  server.send(200, "text/plain", "OK");
}

void servidorWeb_iniciar() {
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  server.on("/", manejarRaiz);
  server.on("/estado", manejarEstado);
  server.on("/pedido", manejarPedido);
  server.on("/manual/x", manejarManualX);
  server.on("/manual/z", manejarManualZ);
  server.on("/manual/linea", manejarManualLinea);
  server.begin();
}

void servidorWeb_actualizar() {
  server.handleClient();
}

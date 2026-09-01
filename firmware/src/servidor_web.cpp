#include "servidor_web.h"
#include <WiFi.h>
#include <WebServer.h>
#include "modo.h"
#include "motores.h"
#include "maquina_estados.h"

// Ver pines.md para el SSID/clave del AP.
static const char* AP_SSID = "CajonesBot";
static const char* AP_PASSWORD = "cajones123";

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
    <strong>Movimiento manual (X)</strong><br>
    <button onclick="cmd('/manual/x?dir=izq')">&#9664; Izquierda</button>
    <button onclick="cmd('/manual/x?dir=parar')">Parar</button>
    <button onclick="cmd('/manual/x?dir=der')">Derecha &#9654;</button>
  </div>

  <div class="fila">
    <strong>Movimiento manual (v&aacute;stago)</strong><br>
    <button onclick="cmd('/manual/z?dir=extender')">&#9650; Extender</button>
    <button onclick="cmd('/manual/z?dir=parar')">Parar</button>
    <button onclick="cmd('/manual/z?dir=retraer')">&#9660; Retraer</button>
  </div>

  <div class="fila">
    <strong>Pedido (rutina aut&oacute;noma)</strong><br>
    <input type="number" id="columna" value="0" min="0" style="width:4rem">
    <button onclick="pedir()">Abrir columna</button>
  </div>

<script>
function cmd(ruta) {
  fetch(ruta).catch(function () {});
}
function pedir() {
  var columna = document.getElementById('columna').value;
  cmd('/pedido?columna=' + columna);
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
  maquinaEstados_solicitarPedido(columna);
  server.send(200, "text/plain", "OK");
}

static void manejarManualX() {
  if (!controlManualPermitido()) {
    server.send(409, "text/plain", "No disponible en este modo/estado");
    return;
  }
  String dir = server.arg("dir");
  if (dir == "izq" || dir == "der") {
    motores_moverX();
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
  if (dir == "extender" || dir == "retraer") {
    motores_moverVastago();
  } else {
    motores_pararTodos();
  }
  server.send(200, "text/plain", "OK");
}

void servidorWeb_iniciar() {
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  server.on("/", manejarRaiz);
  server.on("/estado", manejarEstado);
  server.on("/pedido", manejarPedido);
  server.on("/manual/x", manejarManualX);
  server.on("/manual/z", manejarManualZ);
  server.begin();
}

void servidorWeb_actualizar() {
  server.handleClient();
}

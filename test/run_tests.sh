#!/usr/bin/env bash
# Compila y corre los tests nativos de la máquina de estados (sin ESP32).
# Ver test/README.md.
set -euo pipefail

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="$DIR/test/.build"
mkdir -p "$BUILD_DIR"

c++ -std=c++17 -Wall -Wextra \
  -I "$DIR/firmware/src" -I "$DIR/test/mocks" \
  "$DIR/test/mocks/arduino_mock.cpp" \
  "$DIR/test/mocks/esp32servo_mock.cpp" \
  "$DIR/test/test_maquina_estados.cpp" \
  "$DIR/firmware/src/motores.cpp" \
  "$DIR/firmware/src/sensores.cpp" \
  "$DIR/firmware/src/seguridad.cpp" \
  "$DIR/firmware/src/modo.cpp" \
  "$DIR/firmware/src/maquina_estados.cpp" \
  -o "$BUILD_DIR/test_maquina_estados"

"$BUILD_DIR/test_maquina_estados"

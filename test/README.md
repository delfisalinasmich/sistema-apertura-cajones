# Tests

Tests nativos de la máquina de estados del firmware, pensados para correr en la compu sin necesitar una ESP32 conectada. Complementan (no reemplazan) la prueba física en el banco con LEDs y botones — ver [firmware/README.md](../firmware/README.md).

## Qué prueban

Se compilan **los `.cpp` de producción tal cual** (`motores.cpp`, `sensores.cpp`, `seguridad.cpp`, `modo.cpp`, `maquina_estados.cpp`), no una reimplementación — así el test también prueba el antirrebote real de `sensores.cpp`, no una versión simplificada. Solo se sustituyen las primitivas de Arduino (`millis`, `digitalRead`, `digitalWrite`, `pinMode`) por un mock mínimo en `mocks/`, porque acá no hay ni ESP32 ni el core de Arduino instalado.

`servidor_web.cpp` y `src.ino` quedan fuera (dependen de WiFi/WebServer, específicos del core ESP32) — esos se verifican compilando con `arduino-cli` (ver `firmware/README.md`).

## Cómo correrlos

```
bash test/run_tests.sh
```

Compila con `clang++`/`g++` (ya viene instalado en Mac, no hace falta instalar nada) y corre el binario. Cada verificación imprime `OK`/`FAIL`; el script devuelve código de salida distinto de cero si algo falla.

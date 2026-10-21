# Tests

Tests nativos de la máquina de estados del firmware, pensados para correr en la compu sin necesitar una ESP32 conectada. Complementan (no reemplazan) la prueba física en el banco — ver [firmware/README.md](../firmware/README.md).

## Qué prueban

Se compilan **los `.cpp` de producción tal cual** (`ruedas.cpp`, `base.cpp`, `servo_brazo.cpp`, `modo.cpp`, `maquina_estados.cpp`), no una reimplementación — así el test también prueba el conteo real de marcas y el control real de la base/el servo, no una versión simplificada. Solo se sustituyen las primitivas de Arduino (`millis`, `digitalRead`, `digitalWrite`, `pinMode`, `analogRead`), la librería `ESP32Servo` y `Preferences` (NVS) por mocks mínimos en `mocks/`, porque acá no hay ni ESP32 ni esas librerías instaladas.

`sensor_linea.h` (de Coty) no se vuelve a probar en profundidad — es un módulo ya correcto que se reutiliza tal cual. Los tests arrancan sin calibración guardada en el mock de `Preferences`, así que `detectaLinea()` usa el umbral por defecto (2048, sin histéresis), suficiente para simular que el sensor "ve" o "no ve" una marca.

`servidor_web.cpp`, `main.cpp` y los entornos de banco (`calibracion.cpp`, `servo_test.cpp`, `prueba.cpp`) quedan fuera (dependen de WiFi/WebServer o son herramientas de banco aparte) — esos se verifican compilando con PlatformIO (ver `firmware/README.md`).

## Cómo correrlos

```
bash test/run_tests.sh
```

Compila con `clang++`/`g++` (ya viene instalado en Mac, no hace falta instalar nada) y corre el binario. Cada verificación imprime `OK`/`FAIL`; el script devuelve código de salida distinto de cero si algo falla.

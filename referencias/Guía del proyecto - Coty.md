# Abridor de cajones – Guía del proyecto para el equipo

Oct 20, 2026 · @Constanza Strumia

## Resumen

El robot se desplaza frente a una cajonera, se ubica frente al cajón pedido y lo abre con un brazo con imán. La mecánica básica (base giratoria + servo + imán) ya funciona en el entorno `prueba`. Falta arreglar el sensor de línea, unir todo en el programa principal y hacer la página web por WiFi.

Cómo funciona:

1. Las ruedas mueven el robot a lo largo de la cajonera.
2. Un sensor infrarrojo mira el piso: cada franja negra marca que el robot está frente a un cajón.
3. Una base giratoria orienta el brazo hacia el lado izquierdo o derecho.
4. Un micro servo lleva el brazo hacia adelante hasta que el imán se pega a la tuerca del cajón.
5. El servo vuelve hacia atrás despacio y tira del cajón para abrirlo.

Cada cajón tiene una tuerca de metal pegada en el frente. La cajonera es de 3 × 3 cajones y en el cartón del piso hay 3 franjas negras.

## Hardware y cableado

Todos los pines están definidos en `include/pines.h`. Si cambian un cable, cambian ese archivo y nada más.

| Función | GPIO | Notas |
| --- | --- | --- |
| L298N IN1 / IN2 | 27 / 26 | En `prueba` mueven la base giratoria. ENA con jumper |
| L298N IN3 / IN4 | 32 / 33 | Sin uso en `prueba` (quedan en LOW). ENB con jumper |
| Sensor IR (señal AO) | 34 | ADC1, solo entrada. Sensor alimentado a 3V3 |
| Micro servo (señal) | 25 | `SERVO_PIN`. Rojo a 5 V, marrón a GND común |

Componentes:

- **Placa:** ESP32 NodeMCU-32S (ESP32-D0WD-V3), por USB en COM9 (chip CP210x).
- **Puente H L298N:** ENA y ENB tienen el jumper puesto, así que no hay control de velocidad. Los motores solo van con HIGH/LOW, a velocidad máxima. Freno activo: los dos IN de un motor en HIGH.
- **Ruedas:** motor TT amarillo con reductora.
- **Base giratoria:** motor azul con reductora, con topes mecánicos a cada lado. No tiene encoder ni fines de carrera, se mueve por pulsos de tiempo.
- **Brazo:** micro servo (tipo SG90) con un imán en la punta del vástago.
- **Sensor:** TCRT5000 suelto (LED infrarrojo azul + fototransistor negro) con resistencias soldadas a mano. Ver la sección del sensor.

Reglas de alimentación:

- El GND de la fuente de los motores, el del L298N, el del servo y el del ESP32 tienen que estar unidos.
- El servo va a 5 V, nunca a 3V3. Con USB solo puede reiniciar la placa: conviene una fuente externa de 5 V (por ejemplo, la salida +5V del L298N con batería).
- `motorPinesSeguros()` pone IN1–IN4 en OUTPUT/LOW. Hay que llamarla primero en `setup()` para que ningún motor arranque solo.

## Sensor IR (TCRT5000)

El sensor todavía no funciona: lee casi 0 siempre (piso = 0, cinta negra ≈ 58 sobre 4095). La causa más probable es que falta la resistencia de 10 kΩ entre P34 y 3V3. GPIO34 no tiene pull-up interno, así que sin esa resistencia el fototransistor lleva el pin a GND y nada lo hace subir.

Circuito correcto:

```
3V3 ──┬── R1 (150–220 Ω) ── ánodo LED (azul) ── cátodo LED ──┐
      │                                                       │
      └── R2 (10k–47k) ──┬── colector fototransistor (negro)  │
                         │      emisor fototransistor ────────┤
                         │                                    │
                       P34                                   GND
```

- R1 (chica) va con el LED: marrón-verde-marrón (150 Ω) o rojo-rojo-marrón (220 Ω).
- R2 (grande) va con el fototransistor: marrón-negro-naranja (10 kΩ).
- Si están cambiadas, el LED casi no prende y el receptor casi no da señal.

Cómo verificarlo:

1. Prueba rápida: poner una resistencia de 10 kΩ entre P34 y 3V3 y mirar el modo `r`. Sobre negro o al aire tiene que subir a 2000–4000; a unos 5 mm del cartón tiene que bajar mucho.
2. Mirar el LED azul con la cámara del celular: tiene que verse un punto violeta o blanco.
3. Multímetro entre P34 y GND: cerca de 3 V sobre negro, menos de 1 V sobre el cartón.
4. Si con la resistencia sigue en 0: revisar corto entre P34 y GND (modo continuidad), o probar el fototransistor dado vuelta.

Para que lea bien:

- Altura del sensor al piso: 3–8 mm.
- La cinta negra brillante puede reflejar como si fuera blanca: usar cinta aisladora mate o marcador.
- Aislar las patitas peladas con termocontraíble: si se tocan, dan lecturas raras.
- El trimmer de los módulos comerciales solo ajusta la salida digital DO, no la AO.

## Software

El proyecto usa PlatformIO con el framework Arduino, core **2.0.17**. Eso significa API vieja de PWM (`ledcSetup` / `ledcAttachPin`), no la 3.x. El código y los mensajes por serie están en español.

| Entorno | Archivo | Para qué |
| --- | --- | --- |
| `nodemcu-32s` | `src/main.cpp` | Control del motor (L298N) + lectura del sensor de línea |
| `calibracion` | `src/calibracion.cpp` | Calibrar el sensor IR |
| `servo_test` | `src/servo_test.cpp` | Prueba suelta de un servo en GPIO25 (LEDC directo) |
| `prueba` | `src/prueba.cpp` | Prueba de la mecánica: base + servo + imán |

Cómo está organizado:

- Cada entorno compila un solo `.cpp`, elegido con `build_src_filter` en `platformio.ini`. Es necesario porque cada archivo tiene su propio `setup()` y `loop()`.
- `nodemcu-32s` usa `+<*>` y excluye a los demás. **Si agregan un entorno nuevo, tienen que excluir su archivo ahí.**
- `include/pines.h`: todos los pines y `motorPinesSeguros()`.
- `include/sensor_linea.h`: lógica compartida del sensor (lectura, umbral, polaridad).
- La calibración se guarda en flash con `Preferences`, namespace `linea`.
- Librería del servo: `madhephaestus/ESP32Servo@^3.0.5`.
- Todo el código de control es no bloqueante (`millis()`, sin `delay()` largos), para que la parada de emergencia funcione siempre y para que después pueda correr junto al servidor web.

## Compilar, subir y abrir el monitor

Suban siempre **un solo entorno**, con `-e`. Sin `-e` (por ejemplo, el botón de subir de VS Code sin entorno elegido) se suben los cuatro, uno detrás del otro.

Usen la terminal de PlatformIO dentro de VS Code: fuera de ahí, `pio` no está en el PATH.

```
pio run -e prueba                                  # solo compilar
pio run -e prueba -t upload --upload-port COM9     # compilar y subir
pio device monitor -e prueba                       # abrir el monitor (115200)
```

Cambien `prueba` por el entorno que quieran (`calibracion`, `nodemcu-32s`, `servo_test`).

También se puede hacer desde la barra lateral de PlatformIO: Project Tasks → el entorno → Build / Upload / Monitor.

Antes de subir:

- **Cierren el monitor serie** (Ctrl+C en la terminal, o Stop en el Serial Monitor de VS Code). Si está abierto, la subida falla con "Acceso denegado".
- Si se queda en `Connecting....`, mantengan apretado **BOOT** y suéltenlo cuando empiece a escribir.

Si usan el Serial Monitor de VS Code (en vez de `pio device monitor`): puerto **COM9** (no COM3, que es Bluetooth), 115200 baudios, y Start Monitoring. Los comandos se escriben en la caja de abajo y se mandan con Enter.

## Calibración del sensor (entorno `calibracion`)

La calibración hay que rehacerla cuando el sensor quede bien soldado, porque los valores guardados ahora no sirven. Un borrado total de la flash (`-t erase`) también la pierde.

| Comando | Qué hace |
| --- | --- |
| `w` | Promedia 200 lecturas con el sensor sobre el piso (sin cinta) |
| `b` | Promedia 200 lecturas con el sensor sobre la cinta negra |
| `r` | Lecturas crudas continuas, para probar altura y circuito |
| `l` | Modo en vivo: muestra `LINEA` o `-----` |
| `s` | Muestra los valores guardados |

Con `w` y `b`, el programa calcula el umbral (promedio entre negro y blanco), el contraste y la polaridad (si el negro da más alto o más bajo que el blanco), y los guarda en flash. También muestra mínimo, máximo, promedio y desvío.

Avisos:

- **Contraste menor a 200** (`CONTRASTE_MIN`): poco contraste. Revisar altura, cinta y circuito.
- **Desvío alto** (umbral 50, ajustable al inicio de `calibracion.cpp`): la lectura es inestable o el pin está flotando.
- **Valores pegados a 0 o 4095**: sensor desconectado o mal armado.

Orden para calibrar: `w` sobre el piso, `b` sobre la cinta, `s` para verificar, `l` pasando el sensor por las franjas. Un sensor bien armado debería dar algo como 300 en blanco y más de 2500 en negro.

## Entorno `prueba` (mecánica)

Es lo último que se trabajó. Prueba la base giratoria y el brazo, sin ruedas ni sensor.

Secuencia del comando `t`:

1. Servo a reposo.
2. Base: 7 pulsos hacia la derecha.
3. Servo hacia adelante hasta que el imán toca la tuerca.
4. Servo hacia atrás, despacio: tira del cajón y lo abre.
5. Base: 6 pulsos hacia la izquierda.
6. Todo en LOW. Imprime `PRUEBA TERMINADA`.

Constantes al principio de `src/prueba.cpp` (ya calibradas a mano):

| Constante | Valor | Qué es |
| --- | --- | --- |
| `ANG_ADELANTE` | 90 | Ángulo de contacto con la tuerca |
| `ANG_ATRAS` | 40 | Cajón abierto / reposo |
| Servo ida | 3° cada 15 ms | Hacia la tuerca |
| Servo vuelta | 1° cada 30 ms | Tira del cajón; más lento para que no se despegue el imán |
| `GIRO_MANUAL_MS` | 50 ms | Duración de un pulso de la base (≈ medio giro del motor) |
| `BASE_FRENO_MS` | 100 ms | Freno después de cada pulso |
| `BASE_PAUSA_PULSO_MS` | 150 ms | Pausa entre pulsos |
| `GIRO_BASE_INVERTIDO` | false | Invierte el sentido de la base |
| `T_CONTACTO_MS` | 500 ms | Espera con el imán pegado |
| `T_ABIERTO_MS` | 500 ms | Espera con el cajón abierto |
| `PAUSA_ENTRE_PASOS_MS` | 300 ms | Pausa entre pasos |

Comandos por serie (115200). Se ejecutan con o sin Enter: si pasan 50 ms sin más caracteres, se procesa lo recibido.

| Comando | Qué hace |
| --- | --- |
| `t` | Corre la secuencia completa |
| `x` | Parada de emergencia, instantánea (sin Enter) |
| `s<ang>` | Mueve el servo a ese ángulo (ej. `s90`) |
| `g+` / `g-` | Un pulso de la base a izquierda / derecha |
| `?` | Muestra las constantes y el ángulo actual del servo |

Para ajustar el ángulo de contacto: subir con `s` de a pocos grados hasta que el imán apenas toque la tuerca y sumar 2–3°. Si el servo zumba quieto, está forzando: bajar un par de grados. Los ángulos cambian según el lado: del otro lado (izquierdo) funcionan `s10` y `s50`, así que el código necesita ángulos separados por lado.

## Problemas conocidos y soluciones

| Síntoma | Causa | Solución |
| --- | --- | --- |
| "Acceso denegado" en COM9 | Hay un monitor serie abierto | Cerrarlo (Ctrl+C) antes de subir |
| Se queda en `Connecting....` o `Wrong boot mode detected (0x13)` | No entró en modo de grabación | Mantener BOOT apretado y soltarlo al empezar a escribir |
| Subidas que se cortan ("Packet content transfer stopped", "chip stopped responding") | Cable USB defectuoso o alimentación con servo/L298N conectados | Cambiar el cable USB; desconectar servo y L298N mientras se graba. `prueba` usa `upload_speed = 115200` |
| Bucle `flash read err, 988` | Mismo problema de cable o alimentación | Igual que arriba |
| La placa se reinicia al mover el servo | Servo alimentado por USB | Fuente externa de 5 V con GND común |
| `pio` no se reconoce como comando | Terminal fuera de PlatformIO | Usar la terminal de PlatformIO en VS Code |
| Se suben los cuatro entornos | Se subió sin `-e` | Siempre `pio run -e <entorno> -t upload` |
| `platformio.ini` da "File contains no section headers" | Texto extra antes de la primera línea (pasó con un "pri" tipeado sin querer) | La primera línea tiene que empezar con `;` o con `[` |
| El sensor lee siempre cerca de 0 | Falta la resistencia de 10 kΩ entre P34 y 3V3 | Ver la sección del sensor |

## Lo que aprendimos sobre los pines del ESP32

Tengan esto en cuenta antes de cambiar un pin:

| Pines | Qué tener en cuenta |
| --- | --- |
| ADC2 (0, 2, 4, 12–15, 25–27) | Las lecturas analógicas dejan de funcionar con WiFi activo. Para sensores, usar ADC1 |
| ADC1 (32–39) | Funcionan con WiFi. Son los indicados para sensores analógicos |
| 34, 35, 36 (VP), 39 (VN) | Solo entrada y sin pull-up interno: ideales para sensores, pero necesitan resistencia externa |
| 12 | Pin de arranque: si está en alto al encender, el ESP32 no arranca o no se puede grabar. Por eso se movió IN1 de ahí |
| 15 | Pin de arranque y ADC2. Se descartó para el sensor |
| 14 | Genera pulsos al arrancar: un motor conectado puede dar un tirón |
| 25, 26, 27 | Son ADC2, pero como salidas digitales (servo, IN del L298N) funcionan bien con WiFi |

Otras dos lecciones:

- **Un pin analógico sin nada conectado no lee 0.** Flota y cambia cuando acercan la mano. Al principio el código leía 32/33, que no tenían el sensor, y parecía un sensor con poco contraste.
- **Antes de buscar errores en el código, verifiquen que el cableado real coincida con `pines.h`.** Varias veces el problema fue esa diferencia.

## Pendientes y próximos pasos

Cosas a resolver antes de unir todo:

- [ ] **Definir qué salida del L298N mueve las ruedas y cuál la base.** En `prueba` la base está en IN1/IN2 (27/26), pero en `main.cpp` esos pines mueven las ruedas. Dejar en `pines.h` nombres como `BASE_IN_A` / `BASE_IN_B` y `RUEDAS_IN_A` / `RUEDAS_IN_B`, y que todos los entornos los usen.
- [ ] **Que la secuencia abra cajones de los dos lados.** Hoy `t` abre solo del lado derecho y vuelve.
- [ ] **Revisar la asimetría de la base** (7 pulsos de ida, 6 de vuelta). Correr `t` unas 10 veces seguidas y ver si la base termina siempre en el mismo lugar. Si se corre, darle un pulso de más en cada sentido para que siempre quede contra el tope.
- [ ] **Soltar el imán después de abrir.** Con el cajón abierto, el imán sigue pegado y al girar la base puede arrastrar el cajón. Ver qué pasa y, si hace falta, agregar un paso para despegarlo.
- [ ] **Arreglar el sensor:** soldar la resistencia de 10 kΩ entre P34 y 3V3 y recalibrar.
- [ ] **Alimentación del servo:** pasar a fuente externa de 5 V con GND común.

Próximos pasos, en orden:

1. Terminar la prueba mecánica: los dos lados, pulsos que se resetean contra el tope y soltar el imán.
2. Probar el conteo de franjas con las ruedas y el sensor ya arreglado.
3. Unir todo en el programa principal: una máquina de estados no bloqueante con una API pública `abrirCajon(lado, n)`, `girarBase(lado)`, `irAHome()`, `parar()` y `estado()`, separada del Serial.
4. Hacer la página web por WiFi que llame a esa API. El sensor en GPIO34 (ADC1) sigue funcionando con WiFi.

Preguntas abiertas:

- ¿Cuántos cajones se abren por lado: solo la fila a la altura del brazo, o también otras filas?
- ¿Dónde arranca el robot (posición home): antes de la primera franja?

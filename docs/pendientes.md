# Pendientes

Lista de lo que falta decidir antes de poder cerrar la propuesta inicial y empezar la construcción.

## Grupo / cátedra

- [ ] Nombres de los integrantes del grupo.
- [ ] Confirmar el microcontrolador (ESP32 propuesto).
- [ ] Lista de materiales y presupuesto.
- [ ] Restricciones de tamaño, tiempo de impresión y materiales indicadas por la cátedra para la pieza 3D.

## Diseño mecánico

- [x] Tipo de motor a usar en cada eje — resuelto: DC + reductora + driver L298N para X (ruedas) y Z (brazo), servo para la orientación de línea.
- [ ] Dimensionamiento del imán (a calibrar con prototipo).
- [ ] Cantidad real de líneas de cajones de la cajonera final (hoy el firmware asume 2 como placeholder, ver `ANGULOS_LINEA` en `firmware/src/motores.cpp`).
- [ ] Ángulos del servo para cada línea (a calibrar en el banco).

## Electrónica

- [ ] Confirmar si el L298N elegido alcanza para ambos motores (ruedas y brazo) o hace falta un driver por eje.
- [ ] Dimensionamiento de la fuente de alimentación.
- [ ] Diagrama de conexiones eléctricas definitivo (falta confirmar pines reales).
- [ ] Pines reales de conexión (hoy son placeholder en `firmware/src/pines.h` / `firmware/pines.md`): `PIN_X_IN1/IN2`, `PIN_Z_IN1/IN2`, `PIN_SERVO_ROTACION`, `PIN_SENSOR_COLUMNA`, `PIN_BTN_PEDIDO`, `PIN_BTN_EMERGENCIA`.
- [ ] Confirmar si el sensor óptico de columna tiene comparador integrado (salida digital, no requiere cambios) o es un fotodiodo crudo (necesitaría pasar `sensores.cpp` de `digitalRead()` a `analogRead()` + umbral).
- [ ] Confirmar si los jumpers ENA/ENB del L298N quedan puenteados a habilitado fijo (sin PWM de velocidad) o si se va a controlar velocidad por software.

## Software (a cargo de Delfina)

- [x] Diseño visual/UX de la interfaz web del modo manual — versión funcional simple, ver [modo-manual.md](modo-manual.md).
- [x] Definir si el ESP32 actúa como access point o se conecta a una red existente — Access Point propio (`CajonesBot`).
- [x] Contraseña definitiva del AP (`AbreCajon26`, ver `firmware/pines.md`).
- [x] Máquina de estados de la rutina autónoma + modo manual vía web, rediseñada para el mecanismo real de 3 motores (ruedas X, servo de línea, brazo Z) — ver [diagrama-flujo-estados.md](diagrama-flujo-estados.md), compilado y verificado con `arduino-cli` (incluye la librería `ESP32Servo`, instalar con `arduino-cli lib install "ESP32Servo"`).
- [x] Tests automáticos de la máquina de estados, sin necesidad de placa (`test/`, ver [test/README.md](../test/README.md)) — 30/30 en verde con la secuencia nueva.
- [x] Revisión de seguridad: el botón físico de pedido se lee por flanco, no por nivel, para que el robot no arranque solo al resetear una emergencia (ver `firmware/README.md`).
- [ ] Reemplazar los pines placeholder de `pines.h` por el cableado real una vez armada la electrónica definitiva, y resolver el punto de digital-vs-analógico del sensor óptico (ver arriba).

# Pendientes

Lista de lo que falta decidir/resolver antes de poder cerrar la propuesta inicial y tener el robot funcionando de punta a punta. Fusiona los pendientes de la guía de Coty con los del software; actualizada con sus respuestas del 2026-10-21.

## Grupo / cátedra

- [ ] Nombres de los integrantes del grupo.
- [ ] Confirmar el microcontrolador (ESP32 NodeMCU-32S, ya en uso).
- [ ] Lista de materiales y presupuesto.
- [ ] Restricciones de tamaño, tiempo de impresión y materiales indicadas por la cátedra para la pieza 3D.
- [x] ¿Cuántos cajones se abren por lado? — la cajonera tiene 3 (columnas), a los que se llega con la rueda de traslación + el sensor de línea. No hay filas adicionales: el "lado" (izquierda/derecha) ya cubre el tercer GDL, confirmado por Coty.
- [x] ¿Dónde arranca el robot (home)? — arranca al frente del primer cajón, es decir ya sobre una marca negra. Columna 0 = la primera, donde arranca el robot.

## Hardware — resuelto por Coty (2026-10-21)

- [x] Resistencia de 10 kΩ entre GPIO34 y 3V3 — soldada, y la calibración del sensor ya está guardada.
- [x] Alimentación del servo — conectado a la salida de 5V del puente H (L298N), no a USB.
- [x] Asimetría de pulsos de la base (7 ida / 6 vuelta) — confirmada repetible en pruebas sucesivas; no es un bug, es el comportamiento calibrado.
- [x] El imán sí se suelta solo después de abrir — con los ángulos actuales del repo (`ANG_ADELANTE=90`, `ANG_ATRAS=40`) funciona bien. El pendiente anterior sobre esto está desactualizado.
- [x] El L298N alcanza con margen para los 2 motores (ruedas + base), confirmado por Coty.

## Hardware — pendiente sin resolver

- [ ] **Contradicción en el mapeo de pines BASE/RUEDAS, a chequear hoy en el banco.** `src/prueba.cpp` (sin tocar) mueve la base por GPIO27/26, y con eso la base giró en las pruebas ya hechas. Pero Coty confirmó por escrito que el cableado real es al revés (base en GPIO32/33, ruedas en GPIO27/26, no usadas en esa prueba). Se adoptó la confirmación de Coty en `pines.h`, pero falta el chequeo visual: correr `t` en el entorno `prueba` y mirar qué pieza gira realmente. Si es la base, hay que volver a invertir los 4 valores.
- [ ] **Calibración de pulsos solo existe para un lado.** No se probó ir hacia el lado opuesto a propósito, para evitar que se enreden los cables — así que abrir cajones del otro lado queda fuera de alcance por ahora. La demo va a mostrar un solo lado.
- [ ] No hay botón físico de emergencia ni de pedido todavía — hoy la parada es el comando Serial `x`. La cátedra pide un mecanismo de parada segura; un botón físico dedicado sería más robusto para la demo.
- [ ] Dimensionamiento del imán (a calibrar con el prototipo, con el cajón cargado con la pieza más pesada prevista) — aunque ya funciona, no está formalmente dimensionado.
- [ ] Dimensionamiento de la fuente de alimentación general.

## Software (a cargo de Delfina)

- [x] Diseño visual/UX de la interfaz web del modo manual — versión funcional simple, ver [modo-manual.md](modo-manual.md).
- [x] Definir si el ESP32 actúa como access point o se conecta a una red existente — Access Point propio (`CajonesBot`).
- [x] Contraseña definitiva del AP (`AbreCajon26`, ver `firmware/pines.md`).
- [x] Migración del firmware a PlatformIO sobre el hardware real armado por Coty — máquina de estados no bloqueante con API pública (`abrirCajon`, `girarBase`, `irAHome`, `parar`, `estado`), reutilizando las constantes ya calibradas en `src/prueba.cpp`. Los 4 entornos (`nodemcu-32s`, `calibracion`, `servo_test`, `prueba`) compilan limpio.
- [x] Tests automáticos de la máquina de estados, sin necesidad de placa (`test/`, ver [test/README.md](../test/README.md)) — 36/36 en verde. Encontraron un bug real: el conteo de franjas de `VOLVIENDO_HOME` podía perderse la primera marca por un flanco de sensor no reiniciado al entrar al estado.
- [ ] Confirmar en el banco la contradicción del mapeo BASE/RUEDAS (ver arriba) y, si hace falta, invertir los 4 valores en `pines.h`.
- [ ] Abrir cajones del lado opuesto queda fuera de alcance mientras no haya calibración de pulsos para ese lado (ver arriba) — no quitar la opción del código, pero no mostrarla/promoverla en la demo.
- [ ] Agregar un botón físico de emergencia dedicado cuando el grupo decida cablearlo — hoy `parar()` ya existe como función, solo falta conectarla a una entrada física además del comando Serial.

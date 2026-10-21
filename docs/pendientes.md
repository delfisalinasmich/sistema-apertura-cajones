# Pendientes

Lista de lo que falta decidir/resolver antes de poder cerrar la propuesta inicial y tener el robot funcionando de punta a punta. Fusiona los pendientes de la guía de Coty con los del software.

## Grupo / cátedra

- [ ] Nombres de los integrantes del grupo.
- [ ] Confirmar el microcontrolador (ESP32 NodeMCU-32S, ya en uso).
- [ ] Lista de materiales y presupuesto.
- [ ] Restricciones de tamaño, tiempo de impresión y materiales indicadas por la cátedra para la pieza 3D.
- [ ] ¿Cuántos cajones se abren por lado: solo la fila a la altura del brazo, o también otras filas? (pregunta abierta de Coty)
- [ ] ¿Dónde arranca el robot (posición home): antes de la primera franja? (pregunta abierta de Coty)

## Hardware (pendientes reales de Coty, sin resolver todavía)

- [ ] Falta la resistencia de 10 kΩ entre GPIO34 y 3V3 — sin eso el sensor de línea lee cerca de 0 siempre.
- [ ] Verificar que la asimetría de pulsos de la base (7 ida / 6 vuelta) sea repetible: correr la rutina varias veces seguidas y ver si la base siempre termina en el mismo lugar contra el tope.
- [ ] La calibración de pulsos de la base solo se probó yendo hacia un lado — confirmar que los mismos valores sirven para el lado opuesto (hoy el firmware los reutiliza simétricos, sin calibración propia).
- [ ] El imán no se suelta solo después de abrir el cajón — falta un paso mecánico o de software para despegarlo.
- [ ] Alimentación del servo: pasar a fuente externa de 5V con GND común (con USB se reinicia la placa al moverse).
- [ ] Confirmar que `BASE_IN_A/B` (GPIO27/26) y `RUEDAS_IN_A/B` (GPIO32/33) coinciden con el cableado físico actual (ver [pines.md](../firmware/pines.md) — el mapeo quedó adoptado de `src/prueba.cpp`, el entorno más probado, pero no está confirmado contra el robot armado hoy).
- [ ] No hay botón físico de emergencia ni de pedido todavía — hoy la parada es el comando Serial `x`. La cátedra pide un mecanismo de parada segura; un botón físico dedicado sería más robusto para la demo.
- [ ] Dimensionamiento del imán (a calibrar con el prototipo, con el cajón cargado con la pieza más pesada prevista).
- [ ] Confirmar si el L298N alcanza para ambos motores (ruedas y base) con margen, o si hace falta un driver por eje.
- [ ] Dimensionamiento de la fuente de alimentación general.

## Software (a cargo de Delfina)

- [x] Diseño visual/UX de la interfaz web del modo manual — versión funcional simple, ver [modo-manual.md](modo-manual.md).
- [x] Definir si el ESP32 actúa como access point o se conecta a una red existente — Access Point propio (`CajonesBot`).
- [x] Contraseña definitiva del AP (`AbreCajon26`, ver `firmware/pines.md`).
- [x] Migración del firmware a PlatformIO sobre el hardware real armado por Coty — máquina de estados no bloqueante con API pública (`abrirCajon`, `girarBase`, `irAHome`, `parar`, `estado`), reutilizando las constantes ya calibradas en `src/prueba.cpp`. Los 4 entornos (`nodemcu-32s`, `calibracion`, `servo_test`, `prueba`) compilan limpio.
- [x] Resolver el pendiente de Coty "qué salida del L298N mueve las ruedas y cuál la base" en el código — nombres separados `BASE_IN_A/B` / `RUEDAS_IN_A/B` en `pines.h` (falta confirmar contra el cableado físico, ver arriba).
- [x] Tests automáticos de la máquina de estados, sin necesidad de placa (`test/`, ver [test/README.md](../test/README.md)) — 36/36 en verde. Encontraron un bug real: el conteo de franjas de `VOLVIENDO_HOME` podía perderse la primera marca por un flanco de sensor no reiniciado al entrar al estado.
- [ ] Servidor web para que la secuencia abra cajones de los dos lados — el firmware ya soporta `lado` como parámetro en toda la cadena (`abrirCajon`, `/pedido?lado=...`), pero la calibración real de pulsos solo existe para un lado (ver arriba).
- [ ] Agregar un botón físico de emergencia dedicado cuando el grupo decida cablearlo — hoy `parar()` ya existe como función, solo falta conectarla a una entrada física además del comando Serial.
- [ ] Confirmar si el sensor de línea necesita volver a calibrarse una vez soldada la resistencia de 10kΩ (ver hardware arriba) — la calibración guardada hoy en flash no sirve sin esa resistencia.

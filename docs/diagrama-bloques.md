# Diagrama de bloques del sistema

```mermaid
flowchart TB
    subgraph Alimentacion["Fuente de alimentación (TODO: dimensionar)"]
        FA[Fuente]
    end

    subgraph Control["ESP32 NodeMCU-32S"]
        MCU[ESP32]
    end

    subgraph Actuadores
        MX[Motor X<br/>ruedas]
        BASE[Base giratoria<br/>izquierda/derecha]
        SV[Servo del brazo<br/>abre el cajón]
    end

    subgraph Sensores
        SC[IR TCRT5000<br/>analógico, franjas de columna]
        SM["Contacto/corriente imán<br/>(no implementado)"]
    end

    subgraph Interfaz
        WEB[Servidor web<br/>modo manual + endpoint /pedido]
    end

    FA --> MCU
    FA --> MX
    FA --> BASE
    FA -->|"5V, GND común"| SV

    MCU -->|"driver L298N"| MX
    MCU -->|"driver L298N"| BASE
    MCU -->|"PWM"| SV
    SC -->|"analogRead, ADC1"| MCU
    SM -.-> MCU
    MCU <--> WEB
```

## Pendiente de definir

- Resistencia de 10 kΩ entre GPIO34 y 3V3, sin la cual el sensor lee cerca de 0 siempre.
- Dimensionamiento de la fuente de alimentación.
- Fuente externa de 5V para el servo (con USB se reinicia la placa al moverse).
- Confirmar que `BASE_IN_A/B` (GPIO27/26) y `RUEDAS_IN_A/B` (GPIO32/33) coinciden con el cableado físico actual — ver [pines.md](../firmware/pines.md).

Ver [pendientes.md](pendientes.md) para la lista completa de decisiones abiertas.

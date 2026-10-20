# Diagrama de bloques del sistema

```mermaid
flowchart TB
    subgraph Alimentacion["Fuente de alimentación (TODO: dimensionar)"]
        FA[Fuente]
    end

    subgraph Control["ESP32 (microcontrolador principal)"]
        MCU[ESP32]
    end

    subgraph Actuadores
        MX[Motor X<br/>ruedas]
        SV[Servo de orientación<br/>línea]
        MZ[Motor Z<br/>brazo adelante/atrás]
    end

    subgraph Sensores
        SC[Óptico de columna<br/>IR]
        SM["Contacto/corriente imán<br/>(opcional)"]
    end

    subgraph Interfaz
        WEB[Servidor web<br/>modo manual + endpoint /pedido]
    end

    FA --> MCU
    FA --> MX
    FA --> MZ

    MCU -->|"driver L298N"| MX
    MCU -->|"PWM"| SV
    MCU -->|"driver L298N"| MZ
    SC --> MCU
    SM --> MCU
    MCU <--> WEB
```

## Pendiente de definir

- Confirmar si las 2 ruedas comparten un solo canal del L298N (control como un único eje X) o van por canales separados.
- Dimensionamiento de la fuente de alimentación.
- Si el electroimán se alimenta directo desde la fuente o mediante un driver/transistor controlado por el ESP32.
- Si el sensor óptico de columna tiene comparador integrado (salida digital) o es un fotodiodo crudo (necesitaría `analogRead()` + umbral) — ver [pendientes.md](pendientes.md).

Ver [pendientes.md](pendientes.md) para la lista completa de decisiones abiertas.

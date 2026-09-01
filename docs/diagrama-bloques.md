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
        MX[Motor riel X]
        MV[Motor vástago<br/>extensión + rotación]
    end

    subgraph Sensores
        SX[Fin de carrera X]
        ST[Fin de carrera<br/>transición ranura]
        SM["Contacto/corriente imán<br/>(opcional)"]
    end

    subgraph Interfaz
        WEB[Servidor web<br/>modo manual + endpoint /pedido]
    end

    FA --> MCU
    FA --> MX
    FA --> MV

    MCU -->|"driver de motor (TODO: elegir)"| MX
    MCU -->|"driver de motor (TODO: elegir)"| MV
    SX --> MCU
    ST --> MCU
    SM --> MCU
    MCU <--> WEB
```

## Pendiente de definir

- Driver/etapa de potencia para cada motor (tipo de motor todavía no elegido: paso a paso, DC con reductora, o servo).
- Dimensionamiento de la fuente de alimentación.
- Si el electroimán se alimenta directo desde la fuente o mediante un driver/transistor controlado por el ESP32.

Ver [pendientes.md](pendientes.md) para la lista completa de decisiones abiertas.

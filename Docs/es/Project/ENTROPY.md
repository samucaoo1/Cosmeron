# Versionado por Entropy

Cosmeron utiliza **Entropy** como medida de los cambios acumulados en la biblioteca.

> **Entropy mide los cambios acumulados, no el tamaño del código.**

Un estado publicado se escribe así:

```text
Entropy: <H absoluto><banda griega>
```

Por ejemplo: `Entropy: 18866β`.

## Alcance canónico

El alcance del código fuente de Cosmeron es:

```text
Codespace/Cosmeron/**
```

Las pruebas, la documentación, las traducciones, la configuración de CI, los badges generados y los metadatos del repositorio no afectan a Entropy.

## Origen

La primera versión canónica de Cosmeron establece la etiqueta Git inmutable `entropy-origin`. Cada línea rastreada en `Codespace/Cosmeron/**` en ese momento aporta exactamente **1 H**:

```text
H₀ = líneas en Codespace/Cosmeron/** en entropy-origin
```

El historial anterior al origen de Cosmeron no se reconstruye en el nuevo valor público de Entropy. El desarrollo histórico sigue disponible en el repositorio predecesor y en el historial de Git; la serie de Cosmeron comienza desde su propio estado canónico de código fuente.

## Cambios posteriores al origen

Para cada commit del historial canónico de primer padre:

```text
ΔH = adiciones + eliminaciones
Hₙ = H₀ + ΣΔH
```

Por tanto:

- línea añadida: **+1 H**;
- línea eliminada: **+1 H**;
- línea reescrita: **+2 H**.

Una línea reescrita aporta dos veces porque Git la representa como un estado eliminado y otro introducido. Una refactorización puede reducir el tamaño del código fuente y aun así aumentar Entropy.

Entropy se acumula commit a commit. Cambiar una línea y después restaurar el texto original sigue aumentando Entropy porque la biblioteca pasó por estados adicionales.

El cálculo canónico sigue el historial de **primer padre**, por lo que los pull requests integrados mediante squash cuentan la transición de estado que realmente entró en la línea principal.

## Bandas griegas

Cada **10.000 H** selecciona una banda griega:

```text
0–9.999 H          α
10.000–19.999 H    β
20.000–29.999 H    γ
...
230.000–239.999 H  ω
```

Después de `ω`, los símbolos continúan como columnas de una hoja de cálculo mediante un alfabeto biyectivo de base 24: `ω → αα → αβ → αγ → ...`. El número absoluto nunca se reinicia.

## Reproducibilidad

La implementación pertenece a `.github/entropy/` y debe derivar la base de `entropy-origin`, sumar las adiciones y eliminaciones de `git diff --numstat` en el historial de primer padre y mantener el estado generado fuera del alcance canónico del código fuente.

`.gitattributes` fija el árbol de código fuente en LF para evitar que las conversiones CRLF/LF aumenten artificialmente Entropy.

Las pruebas automatizadas del algoritmo deben cubrir la instantánea inicial, adiciones, eliminaciones, reescrituras, archivos fuera del alcance, renombrados, reversiones, cambios de banda griega y el formato público de visualización.

## Identidad de las versiones

Cosmeron no utiliza versionado semántico para las versiones canónicas. Una versión se identifica por el estado de Entropy en el que se publica.

Consulta [RELEASES.md](RELEASES.md) para ver el registro de versiones.

# Cosmeron — Registro de versiones

Cosmeron **no** utiliza versionado semántico para sus versiones canónicas.

Las versiones públicas se identifican mediante la **Entropy** acumulada de la biblioteca:

~~~text
Entropy: <H absoluto><banda griega>
~~~

El valor numérico representa cambios acumulados en **H**; el sufijo griego identifica la banda actual. Consulta [ENTROPY.md](ENTROPY.md) para conocer el modelo completo.

## Estado actual

**Todavía no se ha publicado ninguna versión canónica de Cosmeron.**

El repositorio se está preparando a partir del último estado del proyecto anterior a la migración. La primera versión pública de Cosmeron establecerá la etiqueta `entropy-origin` y su valor inicial de entropía `H₀`.

El proyecto predecesor utilizaba el nombre **Congro;Library**. Sus registros históricos de versiones y desarrollo siguen siendo contexto histórico; la serie pública de Cosmeron comienza con el primer estado de código fuente publicado bajo la identidad Cosmeron.

## Numeración de versiones

El esquema canónico no utiliza números de versión `1.0.0`, `1.1.0` o `2.0.0`.

Ejemplos:

~~~text
18866β  estado de versión de ejemplo
19124β  estado posterior
20037γ  estado posterior tras un cambio de banda
~~~

El número absoluto de entropía nunca se reinicia.

> Cada versión tiene una historia. Cosmeron mide cuánta historia se ha acumulado.

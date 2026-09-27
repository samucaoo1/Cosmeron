<p align="center">
  <img src="../../Assets/Brand/Cosmeron.png" alt="Cosmeron" width="220">
</p>

# Cosmeron

**Cosmeron es una biblioteca modular C11, orientada a headers, para programación de sistemas portable, experimentación, educación y componentes reutilizables de bajo nivel.**

Cosmeron continúa el proyecto anteriormente desarrollado como **Congro;Library**, preservando su arquitectura y sus principios en una estructura de repositorio más limpia y bajo una nueva identidad pública.

[Documentación](../../README.md) · [Roadmap](../../Project/ROADMAP.md) · [Pattern](../../Reference/PATTERN.md) · [Entropía](../../Project/ENTROPY.md) · [¿Por qué EUPL?](Governance/LICENSING.md) · [Código de Conducta](Governance/CODE_OF_CONDUCT.md) · [Créditos](Governance/AUTHORS.md) · [English](../../../README.md) · [Português (Brasil)](../Portugues-Brasil/README.md)

## ¿Qué es Cosmeron?

Cosmeron es una biblioteca de sistemas en C11 construida como un conjunto de componentes pequeños y comprensibles, no como una caja negra. El código fuente debe ser útil tanto **como biblioteca** como **código que vale la pena estudiar**.

Sus principios centrales son:

- **Legibilidad** — las abstracciones deben hacer más clara la intención.
- **Modularidad** — los componentes deben seguir siendo útiles de forma independiente.
- **Estandarización** — nombres, errores, ownership y APIs generadas siguen reglas comunes.
- **Portabilidad** — los conceptos específicos del sistema operativo quedan fuera de la API pública siempre que sea posible.
- **Educación** — la implementación debe seguir siendo lo bastante comprensible como para enseñar.

> **Una lib para científicos locos:** explorá sistemas de bajo nivel sin convertir el código en un sitio arqueológico.

## Arquitectura

```text
Codespace/Cosmeron/
├── Core/
└── Modules/
```

La biblioteca sigue una arquitectura orientada a headers. Las páginas de implementación como los archivos `.impl` son incluidas por los headers y no requieren unidades de traducción separadas por parte del usuario.

Cuando resulta viable, Cosmeron también sigue una filosofía **zero-link**, evitando flags obligatorias como `-lm`, `-pthread` y `-lws2_32`.

## Estilo de API

Las funciones pueden llamarse mediante la forma generada por macro o directamente:

```c
BIT_FUNC(32, Popcount)(flags);
Bit_32_Popcount(flags);
```

Las operaciones que pueden fallar devuelven `OPSTATUS`; los resultados principales se producen mediante parámetros de salida explícitos.

## Estado

**Experimental / en desarrollo activo.** Las APIs y la estructura todavía pueden cambiar antes de la primera release canónica de Cosmeron.

## Licencia

Cosmeron está licenciada bajo la **Licencia Pública de la Unión Europea 1.2 solamente (EUPL-1.2)**.

---

> Mantené la curiosidad. Rompé cosas. Aprendé.

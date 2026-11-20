# Cómo contribuir a Cosmeron

Las contribuciones son bienvenidas cuando preservan los objetivos centrales de Cosmeron: C legible, comportamiento explícito, arquitectura modular, portabilidad y código que siga siendo útil para aprender.

Antes de modificar APIs públicas, lee el [Patrón Cosmeron](../Reference/PATTERN.md).

## Reglas básicas

Cosmeron tiene como objetivo **C11**. Las APIs públicas deben seguir siendo portables y evitar exponer vocabulario específico de sistemas operativos, salvo que la API esté orientada explícitamente a funciones nativas.

Sigue las convenciones establecidas de nombres, espacios de nombres, propiedad, estados y parámetros de salida. Prefiere los auxiliares de preprocesador existentes en lugar de recrear por cuenta propia la concatenación de tokens o los mecanismos de espacios de nombres.

Siempre que sea viable, conserva la filosofía **zero-link**: el usuario no debería necesitar una nueva opción obligatoria del enlazador cuando una función pueda implementarse sin ella.

## Cambios

Mantén los cambios enfocados. Si una refactorización modifica un patrón establecido en varios módulos, documenta primero la regla o actualiza el Patrón en el mismo cambio.

Los nuevos comportamientos públicos deben incluir:

- pruebas que cubran los casos de éxito, límites y errores;
- documentación de la API pública;
- ejemplos cuando el uso previsto no sea evidente;
- consideración de GCC, Clang, MinGW, MSVC y Clang en macOS, cuando corresponda.

## Pruebas

El proyecto utiliza la compilación estricta de C11 como referencia:

```text
-std=c11 -Wall -Wextra -Wpedantic -Werror
```

Las pruebas de los módulos deben organizarse por módulo y ejercitar todas las funciones públicas afectadas por el cambio.

No reduzcas las advertencias solo para conseguir que un cambio compile.

## Pull requests

Un pull request debe explicar **qué cambió**, **por qué el cambio pertenece a Cosmeron** y cuáles son sus consecuencias para la portabilidad o la compatibilidad.

Los pull requests pequeños y coherentes son más fáciles de revisar que los cambios no relacionados agrupados.

## Documentación

La documentación canónica del proyecto se encuentra en `Docs/`.

- gobernanza: `Docs/Governance/`
- dirección del proyecto y versiones: `Docs/Project/`
- reglas técnicas transversales: `Docs/Reference/`
- documentación de módulos: `Docs/Modules/`
- traducciones: `Docs/pt/` y `Docs/es/`

Reserva la raíz del repositorio para archivos con funciones a nivel del repositorio o de descubrimiento automático.

## Seguridad

No abras issues públicos para vulnerabilidades que requieran detalles confidenciales. Sigue la [Política de Seguridad](SECURITY.md) y utiliza la función de reporte privado de vulnerabilidades de GitHub.

## Conducta

La participación se rige por el [Código de Conducta](CODE_OF_CONDUCT.md).

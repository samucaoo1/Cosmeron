# SQLite integrado y política de enlazado sin bibliotecas adicionales

Cosmeron incorpora SQLite como una única unidad de traducción de implementación vendorizada: define `DATABASE_SQLITE_IMPLEMENTATION` en un archivo C (por ejemplo, el `Codespace/Tests/Database/Backend.c` suministrado) y después incluye `Cosmeron/Modules/Database/SQLite/SQLite.h`. No se enlaza ninguna biblioteca SQLite externa.

## Elección de threading

| Objetivo | Valor predeterminado de `DATABASE_SQLITE_THREADSAFE` | Comportamiento de enlazado de la biblioteca |
| --- | --- | --- |
| FreeBSD / OpenBSD / NetBSD / DragonFly | `0` | Backend SQLite sin mutexes, pensado para evitar `-pthread` |
| Linux / macOS / Windows | `1` | Se conserva el backend SQLite serializado/thread-safe existente |
| Opt-in explícito en BSD | `1` | SQLite thread-safe; puede requerir `-pthread` externo |

`DATABASE_SQLITE_THREADSAFE=0` **desactiva la capa de mutexes de SQLite**.
**No** llames a SQLite simultáneamente desde distintos hilos, incluso si utilizas conexiones de base de datos diferentes. Un bucle de eventos serial o un único worker de base de datos es un uso apropiado. No interpretes la presencia del módulo Concurrency de Cosmeron como permiso para acceder a SQLite concurrentemente en este modo. Esto cambia las garantías del **build integrado de SQLite**, no las del formato de archivo SQL.

Para volver a habilitar los mutexes internos de SQLite, define la macro como `1` antes de incluir el header de implementación y proporciona los flags de enlazado específicos del sistema operativo si la toolchain los requiere:

```c
#define DATABASE_SQLITE_THREADSAFE 1
#define DATABASE_SQLITE_IMPLEMENTATION
#include "Cosmeron/Modules/Database/SQLite/SQLite.h"
```

La selección se realiza en tiempo de compilación, se aplica a toda la aplicación y debe ser idéntica a la de la única unidad de implementación. No compiles dos implementaciones vendorizadas con configuraciones incompatibles. La regresión `SQLiteThreadMode.c` llama a la entrada `sqlite3_threadsafe()` de SQLite integrado para confirmar el modo compilado; las pruebas de persistencia y multi-TU verifican la API pública.

**Advertencia:** evitar un flag explícito de enlazado no equivale a eliminar todas las dependencias de runtime del sistema operativo. Otros backends (Audio, Concurrency) todavía pueden requerir bibliotecas nativas compartidas en BSD.

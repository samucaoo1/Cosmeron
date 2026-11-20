# SQLite incorporado e política de linkedição sem bibliotecas extras

O Cosmeron incorpora o SQLite como uma única unidade de tradução de implementação vendorizada: defina `DATABASE_SQLITE_IMPLEMENTATION` em um arquivo C (por exemplo, o `Codespace/Tests/Database/Backend.c` fornecido) e, em seguida, inclua `Cosmeron/Modules/Database/SQLite/SQLite.h`. Nenhuma biblioteca SQLite externa é vinculada.

## Escolha de threading

| Alvo | Padrão de `DATABASE_SQLITE_THREADSAFE` | Comportamento de linkedição da biblioteca |
| --- | --- | --- |
| FreeBSD / OpenBSD / NetBSD / DragonFly | `0` | Backend SQLite sem mutexes, destinado a evitar `-pthread` |
| Linux / macOS / Windows | `1` | Backend SQLite serializado/thread-safe existente é mantido |
| Opt-in explícito no BSD | `1` | SQLite thread-safe; pode exigir `-pthread` externo |

`DATABASE_SQLITE_THREADSAFE=0` **desativa a camada de mutexes do SQLite**.
**Não** chame o SQLite simultaneamente a partir de threads diferentes, mesmo usando conexões de banco de dados distintas. Um loop de eventos serial ou um único worker de banco de dados é um uso apropriado. Não interprete a presença do módulo Concurrency do Cosmeron como permissão para acessar o SQLite concorrentemente nesse modo. Isso altera as garantias do **build incorporado do SQLite**, não do formato de arquivo SQL.

Para reativar os mutexes internos do SQLite, defina a macro como `1` antes de incluir o header de implementação e forneça as flags de linkedição específicas do sistema operacional, caso a toolchain exija:

```c
#define DATABASE_SQLITE_THREADSAFE 1
#define DATABASE_SQLITE_IMPLEMENTATION
#include "Cosmeron/Modules/Database/SQLite/SQLite.h"
```

A seleção ocorre em tempo de compilação, vale para toda a aplicação e deve ser idêntica à da unidade de implementação única. Não compile duas implementações vendorizadas com configurações conflitantes. A regressão `SQLiteThreadMode.c` chama a entrada `sqlite3_threadsafe()` do SQLite incorporado para confirmar o modo compilado; os testes de persistência e multi-TU verificam a API pública.

**Ressalva:** evitar uma flag explícita de linkedição não equivale a remover todas as dependências de runtime do sistema operacional. Outros backends (Audio, Concurrency) ainda podem exigir bibliotecas nativas compartilhadas no BSD.

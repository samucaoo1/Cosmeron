# Concorrência nativa e política de linkagem BSD

`Concurrency/Cooperative.h` expõe atomics, coroutines e tarefas executadas diretamente sem criar threads do sistema operacional. Não fornece paralelismo, joins, mutexes nem um thread pool.

`Concurrency/Native.h` expõe as funções nativas existentes de Thread, Mutex, RWLock, Semaphore, Condition, Barrier, Future e ThreadPool. A implementação POSIX atualmente utiliza `pthread_*` e pode exigir `-pthread` ou um runtime específico da plataforma. O Windows utiliza suas APIs nativas.

`Concurrency/Concurrency.h` continua sendo um cabeçalho agregado compatível para o perfil nativo. Esses cabeçalhos preservam intencionalmente os nomes de funções e tipos visíveis ao usuário. Não substituímos silenciosamente mutexes por atomics nem atribuímos semântica paralela a tarefas cooperativas.

A CI realiza três verificações separadas no FreeBSD, OpenBSD, NetBSD e DragonFly BSD:

1. **Cooperative** — deve compilar, linkar e executar sem flags adicionais fornecidas pelo usuário.
2. **Native pthread** — tenta uma verificação estrita sem flags adicionais; se falhar, executa a funcionalidade real com as flags nativas necessárias e registra explicitamente essa limitação.
3. **C11 thrd_create/thrd_join** — uma verificação independente de threads reais sem `-pthread` ou `-lstdthreads`; indica viabilidade, mas não certifica equivalência direta com os tipos de sincronização existentes baseados em pthread.

Um backend nativo alternativo deve preservar joins, semântica de detach, sincronização e segurança multi-TU sob contenção. Alterar nomes de símbolos ou utilizar `dlopen` não demonstra, por si só, portabilidade real sem links adicionais.

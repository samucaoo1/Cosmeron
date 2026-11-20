# Concurrencia nativa y política de enlazado BSD

`Concurrency/Cooperative.h` expone atomics, coroutines y tareas ejecutadas directamente sin crear hilos del sistema operativo. No proporciona paralelismo, joins, mutexes ni un thread pool.

`Concurrency/Native.h` expone las funciones nativas existentes de Thread, Mutex, RWLock, Semaphore, Condition, Barrier, Future y ThreadPool. La implementación POSIX utiliza actualmente `pthread_*` y puede requerir `-pthread` o un runtime específico de la plataforma. Windows utiliza sus API nativas.

`Concurrency/Concurrency.h` sigue siendo un encabezado agregado compatible para el perfil nativo. Estos encabezados preservan intencionalmente los nombres de funciones y tipos visibles para el usuario. No sustituimos silenciosamente mutexes por atomics ni atribuimos semántica paralela a las tareas cooperativas.

CI realiza tres comprobaciones separadas en FreeBSD, OpenBSD, NetBSD y DragonFly BSD:

1. **Cooperative** — debe enlazar y ejecutarse sin flags adicionales del usuario.
2. **Native pthread** — intenta una prueba estricta sin flags adicionales; si falla, ejecuta la funcionalidad real con los flags nativos necesarios y registra explícitamente esa limitación.
3. **C11 thrd_create/thrd_join** — una prueba independiente de hilos reales sin `-pthread` ni `-lstdthreads`; indica viabilidad, pero no certifica equivalencia directa con los tipos de sincronización existentes basados en pthread.

Un backend nativo alternativo debe preservar joins, semántica de detach, sincronización y seguridad multi-TU bajo contención. Cambiar nombres de símbolos o utilizar `dlopen` no demuestra por sí solo portabilidad real sin enlaces adicionales.

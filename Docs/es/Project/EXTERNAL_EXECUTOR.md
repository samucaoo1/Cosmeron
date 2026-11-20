# Contrato de External Executor (C11)

Incluye `Cosmeron/Modules/Concurrency/Task/Executor.h` o `Concurrency/Cooperative.h`. Un executor es un callback `submit(userData, TTask)` y un contexto. `Executor_Submit` no asigna memoria ni inicia hilos. El host puede programar tareas con pthread, SDL, otro runtime o de forma síncrona.

`TTask` se pasa por valor. Un executor diferido debe copiarlo en su propia cola, y el llamador debe garantizar que `task.argument` siga vivo hasta que termine la ejecución; programar la tarea no transfiere la propiedad del argumento.

La sincronización de memoria compartida y el cierre del executor son responsabilidad del host. `Executor_RunInline` invoca una tarea inmediatamente, sin concurrencia real.

Esta es una extensión de integración, no un sustituto de la creación completa de hilos o mutexes, ni una prueba de que los backends nativos con hilos cumplan el requisito de cero enlaces adicionales en BSD.

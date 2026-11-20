# Contrato de External Executor (C11)

Inclua `Cosmeron/Modules/Concurrency/Task/Executor.h` ou `Concurrency/Cooperative.h`. Um executor é um callback `submit(userData, TTask)` e um contexto. `Executor_Submit` não aloca memória nem inicia threads. O host pode agendar tarefas com pthread, SDL, outro runtime ou de forma síncrona.

`TTask` é passado por valor. Um executor adiado deve copiá-lo para sua própria fila, e o chamador deve garantir que `task.argument` permaneça válido até o término da execução; agendar uma tarefa não transfere a propriedade do argumento.

A sincronização de memória compartilhada e o encerramento do executor são responsabilidade do host. `Executor_RunInline` invoca uma tarefa imediatamente, sem concorrência real.

Esta é uma extensão de integração, não um substituto para criação completa de threads ou mutexes, nem uma prova de que os backends nativos com threads atendam ao requisito de zero links adicionais no BSD.

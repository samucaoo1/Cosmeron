# Cosmeron — matriz de portabilidade e CI

_Atualizado: 2026-10-11. C11, orientada a headers, sem flags obrigatórias de linkedição para o usuário._

Este documento distingue o que foi **testado** do que é apenas **plausível**. Uma compilação cruzada bem-sucedida não equivale a um teste de execução em um dispositivo real. Todos os resultados devem ser verificados no GitHub Actions; a existência de um workflow não significa que ele tenha sido aprovado.

## Verificação automatizada

| Alvo | Ambiente | Workflow | Escopo |
| --- | --- | --- | --- |
| Linux GCC/Clang | Runners nativos do GitHub | [By Operating System](../../.github/workflows/03-by-operating-system.yml) / [Simple Test](../../.github/workflows/01-simple-test.yml) | Suítes existentes de integração dos módulos |
| Windows MinGW/MSVC (10 e 11) | Runners nativos do GitHub | [By Operating System](../../.github/workflows/03-by-operating-system.yml) | Suítes existentes de integração dos módulos |
| macOS Clang (Apple Silicon e Intel) | Runners nativos do GitHub | [By Operating System](../../.github/workflows/03-by-operating-system.yml) | Suítes existentes de integração dos módulos |
| FreeBSD | VM convidada FreeBSD nativa via vmactions | [By Operating System](../../.github/workflows/03-by-operating-system.yml) | Suítes nativas portáveis e estendidas; relatórios por módulo distinguem perfis sem linkedição adicional |
| OpenBSD / NetBSD / DragonFly BSD | VMs convidadas nativas via vmactions | [By Operating System](../../.github/workflows/03-by-operating-system.yml) | Suítes nativas portáveis e estendidas; relatórios por módulo distinguem perfis sem linkedição adicional |
| Fedora / Arch / Debian / Ubuntu / openSUSE / Gentoo / Alpine | Contêineres nativos de espaço de usuário Linux | [By Operating System](../../.github/workflows/03-by-operating-system.yml) | Suítes portáveis e nativas em diferentes implementações de libc |
| Void glibc / Void musl | Contêineres nativos de espaço de usuário Linux | [By Operating System](../../.github/workflows/03-by-operating-system.yml) | Suítes portáveis; cobertura de glibc e musl |
| Toolchain Nix / VM NixOS | Derivação Nix fixada no Ubuntu e VM convidada NixOS real | [By Operating System](../../.github/workflows/03-by-operating-system.yml) | C11 estrito e portável aprovado como pacote e dentro da VM NixOS ([execução](https://github.com/samucaoo1/Cosmeron/actions/runs/38141169787)); não é certificação completa de hardware |
| Proxy ChromeOS Crostini | Contêiner Debian no Ubuntu | [Experimental](../../.github/workflows/04-experimental.yml) | Suítes portáveis e nativas em uma aproximação do contêiner Crostini; **não** é um Chromebook real |

A suíte de verificação portável é [test-portable-core.sh](../../.github/scripts/test-portable-core.sh). Ela compila e executa os fontes de teste existentes no repositório com `-std=c11 -Wall -Wextra -Wpedantic -Werror` e sem bibliotecas extras explícitas na linkedição. O [executor estendido BSD](../../.github/scripts/test-bsd-extended.sh) executa Network, Concurrency, SQLite, Compression, Audio, Input e Terminal/Canvas não interativo. Ele gera um **resumo de job por módulo no CI**, distinguindo compilação, execução em runtime, ausência de linkedição extra e escopo de dispositivo.

O SQLite incorporado usa, por padrão nos BSDs, uma implementação **single-threaded sem mutexes** e passou nos testes sem linkedição adicional nos quatro ambientes BSD ([projeto do SQLite](SQLITE_LINKING.md)). Ele não pode ser acessado simultaneamente por várias threads.

Audio e Concurrency possuem subconjuntos independentes e puros, sem linkedição adicional:
[Audio/Offline.h](../../Codespace/Cosmeron/Modules/Audio/Offline.h) fornece PCM, síntese, sequências e renderização de beep sem I/O de dispositivo ou threads do sistema operacional;
[Concurrency/Cooperative.h](../../Codespace/Cosmeron/Modules/Concurrency/Cooperative.h) fornece atomics, coroutines e tasks diretas **sem threading nativo**.
Esses perfis não equivalem aos backends Audio/Device de desktop ou ao ThreadPool baseado em pthread. Em alguns BSDs, os backends nativos de áudio e threading ainda exigem `-pthread` e/ou `-lm`; portanto, seus testes de compatibilidade funcional **não** são certificações sem linkedição adicional. Dispositivos físicos e terminais interativos ainda não foram testados.

## Viabilidade adicional de plataformas

### NixOS

**Verificado para o subconjunto selecionado de testes C11 portáveis em uma VM NixOS real.**
`flake.nix` e `flake.lock` fixam o Nixpkgs, expõem uma derivação/verificação e inicializam uma VM convidada NixOS em QEMU que executa os testes empacotados. Tanto o pacote fixado quanto a VM NixOS real [passaram no CI](https://github.com/samucaoo1/Cosmeron/actions/runs/38141169787). Hardware específico do sistema operacional, permissões e drivers continuam sem verificação.

### ChromeOS

**Viável por meio do Crostini, com restrições.** O ambiente de desenvolvimento Linux do Google expõe o Debian dentro de uma VM/contêiner gerenciado. Os módulos portáveis devem se comportar como seus equivalentes Linux. O job Debian do CI aproxima esse espaço de usuário, mas **não** exercita o kernel/permissões de dispositivos do ChromeOS.

Existe um script de verificação em um Chromebook real em [Tools/crostini/check.sh](../../Tools/crostini/check.sh); ele ainda não foi executado em um Chromebook. Aplicativos nativos do ChromeOS são um alvo diferente; o acesso pelo terminal a HID, evdev, joysticks, USB e captura de GUI/entrada pode ser mediado ou bloqueado. Valide Input, Network, Audio e Terminal em hardware real de Chromebook. Considere separadamente builds Android no ChromeOS; o job Crostini não é um teste do Android NDK.

Referências oficiais:
- https://developers.google.com/chromeos/app-development/develop
- https://developers.google.com/chromeos/app-development/develop/setup
- https://www.chromium.org/chromium-os/developer-library/guides/containers/containers-and-vms/

### OpenHarmony

**A compilação cruzada C11 nativa para ARM64 com SDK 6.0 e a linkedição de múltiplas unidades de tradução passaram no CI**:
[execução real do SDK OpenHarmony](https://github.com/samucaoo1/Cosmeron/actions/runs/38141169787).
As macros e a triple do alvo são preservadas nos artefatos do workflow.
A prova de compilador em [Tools/ohos](../../Tools/ohos/README.md) **não** executa o código. O runtime OpenHarmony, APIs vinculadas ao sistema operacional, dispositivos LiteOS, Huawei HarmonyOS NEXT e permissões de hardware continuam sem verificação.

Etapas:
1. Obter e fixar um SDK/toolchain nativo OpenHarmony confiável.
2. Verificar todos os headers públicos com o Clang do alvo e o sysroot OHOS; acompanhar explicitamente `__OHOS__` e outras macros predefinidas do alvo emitidas pelo compilador **real**.
3. Começar por Core/Bit/Struct/Type/Math/Sort e memória/contêineres puros.
4. Adicionar sondagens de compatibilidade POSIX para Chronometry, Random, File, Concurrency, Network e Terminal; escrever backends de plataforma somente quando necessário.
5. Validar a inclusão header-only em múltiplas unidades de tradução e o contrato sem linkedição adicional; depois executar em um alvo representativo do sistema padrão OHOS.
6. Tratar produtos mínimos baseados em LiteOS como um perfil distinto do OpenHarmony padrão; um build ARM64 de espaço de usuário bem-sucedido não certifica microcontroladores pequenos.

Referências:
- https://github.com/openharmony/docs/blob/master/en/device-dev/subsystems/subsys-build-all.md
- https://gitee.com/openharmony/build/wikis/pages/export?doc_id=3015289&type=pdf

### Huawei HarmonyOS / HarmonyOS NEXT

**Certificação de plataforma separada; não é sinônimo de OpenHarmony.** A Huawei documenta o desenvolvimento de aplicações HarmonyOS por meio do DevEco Studio/HarmonyOS SDK, ArkTS/ArkUI e recursos nativos. Algoritmos C reutilizáveis podem ser incorporados por uma ponte nativa, se houver suporte no SDK/runtime do alvo. APIs de dispositivo/permissão, empacotamento de aplicativos e compatibilidade com versões específicas do HarmonyOS exigem validação na toolchain da Huawei.

Não trate macros Linux, disponibilidade de POSIX, compatibilidade com ABI Android ou uma compilação cruzada OpenHarmony como prova de suporte ao HarmonyOS NEXT. Prefira um pequeno exemplo de ponte ArkTS/nativa e testes em dispositivo quando houver um SDK compatível disponível.

Portal oficial de desenvolvimento:
- https://developer.huawei.com/consumer/en/harmonyos/develop/

## Implantação em etapas

1. **Obrigatório:** dez suítes C11 portáveis nos quatro ambientes BSD; Fedora/Arch/Void glibc/Void musl; compilador fornecido pelo Nix; proxy Debian Crostini.
2. **Integração BSD mais ampla:** CLI, Parsing, Compression, Database, File, Audio, Terminal/Canvas, Concurrency e Network; diagnosticar falhas pelos headers, chamadas de sistema e comportamento de linkedição reais do BSD, não desativando avisos.
3. **Cobertura adicional:** variantes de libc, Chromebook real, dispositivo OpenHarmony, backends BSD completos sem linkedição adicional e testes de runtime em hardware. A VM NixOS e a compilação cruzada OHOS ARM64 já estão implementadas e possuem execuções aprovadas.
4. **Certificação por módulo:** publicar resultados e limitações dos backends. Distinguir estados compilado, testado por unidade, integrado e testado em hardware.

Esta é uma biblioteca educacional: algoritmos/módulos deliberadamente incomuns permanecem, mas as verificações de build não podem passar apenas porque os testes foram silenciosamente ignorados.

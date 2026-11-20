# Perfis sem linkedição adicional — C11

O Cosmeron distingue as flags de linkedição exigidas pela aplicação dos recursos disponíveis em runtime. Usar um carregador dinâmico de bibliotecas não elimina a dependência **em runtime** dessa biblioteca; da mesma forma, uma compilação C11 bem-sucedida não prova que um backend de dispositivo funcione.

## Audio (síntese offline)

```c
#include "Cosmeron/Modules/Audio/Offline.h"

int main(void) {
  AUDIO_PCM_TYPE(TBuffer) tone = {0};
  if (AUDIO_BEEP_FUNC(RenderTone)(440.0, 100, 0.2f, 48000, &tone)
      != STATUS_CONST(SUCCESS)) return 1;
  AUDIO_PCM_FUNC(Destroy)(&tone);
  return 0;
}
```

Funciona sem backends de áudio de terceiros. O namespace e as assinaturas das funções não mudam. `Audio.h` continua sendo o agregador completo para desktop; definir `AUDIO_NO_DESKTOP` ainda seleciona o comportamento reduzido anterior. Testes: `Synthesis`, `OfflineStandalone`, `OfflineCompat` e `OfflineMultiTU` em `Codespace/Tests/Audio/`.

Os decodificadores puros C11 de WAV e FLAC estão disponíveis separadamente por `Offline.h` / `Codec/WAV.h` / `Codec/FLAC.h`.
A reprodução em dispositivos desktop e o Player usam integração nativa de runtime do sistema operacional sem bibliotecas extras na linkedição.

## Concurrency (somente cooperativo)

```c
#include "Cosmeron/Modules/Concurrency/Cooperative.h"
```

Fornece Atomics lock-free (quando compatível), Coroutine sem pilha e operações Task executadas diretamente. Esse ponto de entrada não fornece paralelismo real, Mutex, ThreadPool, Join ou threads worker. É um perfil de API separado, **não** um substituto para o backend de threading nativo.
`Codespace/Tests/Concurrency/Cooperative.c` verifica o contrato C11 estrito sem bibliotecas extras na linkedição.

## SQLite incorporado

No FreeBSD, OpenBSD, NetBSD e DragonFly, a implementação padrão usa `DATABASE_SQLITE_THREADSAFE=0` (sem mutexes internos do SQLite). Ela passou no teste sem linkedição adicional nas quatro VMs convidadas. Chamadas concorrentes ao SQLite são **não suportadas** nessa configuração, mesmo por conexões distintas. Defina `DATABASE_SQLITE_THREADSAFE=1` na única unidade de tradução de implementação para optar pelos mutexes; uma dependência nativa de linkedição `-pthread` pode voltar a ser necessária.

Consulte [SQLITE_LINKING.md](SQLITE_LINKING.md) para a configuração exata.

## Reprodução manual de áudio no BSD

Consulte [AUDIO_MANUAL_BSD.md](AUDIO_MANUAL_BSD.md). Inclua `Cosmeron/Modules/Audio/Device/Manual.h` para usar a API orientada a callback `Manual_Open`, `Manual_Start`, `Manual_Pump`, `Manual_Stop` e `Manual_Close`. Cada chamada a pump renderiza PCM float intercalado, converte para S16LE e grava usando a thread chamadora do host. Esse header não cria threads automaticamente nem depende de vendors.

FreeBSD / DragonFly: OSS nativo; NetBSD: audio(4); OpenBSD: sndio resolvido por `dlopen`. OpenBSD **não** exige a flag explícita `-lsndio`, mas **exige** a biblioteca compartilhada sndio e um sndiod/dispositivo de áudio funcional em runtime. Esses backends são testados em compilação/linkedição em VMs BSD reais; a reprodução física ainda **não** está certificada.

## Codec WAV sem vendor

Inclua `Cosmeron/Modules/Audio/Codec/WAV.h` ou `Audio/Offline.h`.
`Audio_Codec_WAV_DecodeMemory` oferece suporte a RIFF/WAVE PCM sem sinal de 8 bits, PCM com sinal de 16/24/32 bits e float IEEE de 32 bits (em hosts IEEE binary32), decodificando para um buffer PCM float intercalado com ownership. `Audio_Codec_WAV_WriteFile` grava PCM16.
Nenhuma das funções importa vendors externos, threads ou libm. WAVE comprimido/Extensible, RF64 e RIFX big-endian não são suportados; as entradas são limitadas pelos comprimentos RIFF e dos chunks de dados. Os programas de teste `NativeWAV`, `WAVRobustness` e `WAVFormats` verificam os formatos suportados e rejeitam arquivos malformados.

## Executor de tasks fornecido pelo usuário

Inclua `Cosmeron/Modules/Concurrency/Task/Executor.h` ou `Concurrency/Cooperative.h`. Um callback `Executor_Submit` pode copiar e agendar um `TTask` no pool de workers já existente na aplicação. O adaptador não cria nenhuma thread nativa automaticamente. Isso **não** transforma as implementações nativas de `Thread`, `Mutex` ou `ThreadPool` em backends sem linkedição adicional. Consulte [EXTERNAL_EXECUTOR.md](EXTERNAL_EXECUTOR.md).

## Perfil nativo somente de decodificação (sem threads de dispositivo)

Inclua `Cosmeron/Modules/Audio/CodecMinimal.h` para manter a API pública de decodificação `Audio_Codec_*`, incluindo suporte nativo a WAV, MP3 e FLAC, excluindo I/O de dispositivo, threading nativo e componentes de player.

```c
#include "Cosmeron/Modules/Audio/CodecMinimal.h"

int main(void) {
  AUDIO_CODEC_TYPE(TDecoder) decoder = {0};
  /* Audio_Codec_OpenFile(&decoder, "sample.flac");
     ... Audio_Codec_Close(&decoder); */
  return 0;
}
```

Isso **não** é o mesmo que o decodificador WAV sem vendor descrito acima.
O perfil de codecs BSD resolve os símbolos matemáticos necessários
(`sin`, `exp`, `log`, `sqrt`, `pow`) da `libm` do host em runtime,
usando `dlopen`/`dlsym` e um inicializador de execução única com atomics C11. A implementação continua **dependente de uma libm compartilhada compatível instalada no sistema de destino**. Uma aplicação vinculada estaticamente precisa de validação separada.

`CodecMinimal.c` e `CodecMath.c` são compilados **e executados** sem flags adicionais de linkedição em cada VM BSD. Uma sondagem bem-sucedida é registrada como `AUDIO_CODEC_MINIMAL_ZERO_LINK=PASS`; uma falha permanece `NOT_SUPPORTED`. O backend desktop Device/Player usa carregamento dinâmico sem bibliotecas extras na linkedição.

Uma unidade de tradução que seleciona `CodecMinimal.h` compila decodificadores independentes sem threads de dispositivo de reprodução.
Os recursos nativos de decodificação (WAV, FLAC, MP3) continuam cobertos por seus respectivos testes.

O adaptador matemático Linux usa `pthread_once` para inicializar símbolos matemáticos de runtime quando a resolução matemática dinâmica é necessária.
Essa configuração não afirma evitar **todas** as dependências possíveis de runtime; ela apenas evita novas flags de linkedição `-pthread` do usuário onde foi testada.

## Variantes de libc Linux e runtime somente de decodificação

O decodificador mínimo agora é testado por smoke tests em contêineres reais **Fedora, Arch, Void glibc, Void musl e Debian**, usando a suíte C11 `test-linux-codec-minimal.sh`. A suíte decodifica WAV, MP3 e FLAC em memória e exercita as cinco operações matemáticas sem flags extras de linkedição. Em glibc, a resolução matemática dinâmica tenta primeiro `libm.so.6`; para outros layouts de libc, tenta `libm.so` e depois `RTLD_DEFAULT` (os símbolos do processo em execução). Uma implementação matemática de runtime ausente retorna `NOT_AVAILABLE` na construção do decodificador, em vez de chamar sem verificação um ponteiro de função nulo. **Isso não representa independência do runtime matemático do sistema operacional.** Uma aplicação vinculada estaticamente precisa de validação separada.

### Header somente de decodificação entre unidades de tradução

`CodecMinimalTU_A.c` e `CodecMinimalTU_B.c` incluem `CodecMinimal.h` de forma independente, exercitam funcionalidades de decodificação e matemática e são vinculados a uma terceira unidade de tradução. O executável é testado sob C11 estrito, sem `-lm` ou `-pthread` adicionais, em cada VM BSD e contêiner de libc Linux. Isso verifica o comportamento real de linkedição multi-TU para o modo mínimo header-only; as dependências matemáticas/dinâmicas de runtime permanecem conforme documentado acima.

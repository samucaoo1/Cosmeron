# Reprodução manual de áudio: backends BSD nativos

**Ponto de entrada:** `Cosmeron/Modules/Audio/Device/Manual.h`.

Ao contrário dos workers de dispositivo em segundo plano, esta API não possui thread worker:
`Manual_Open` configura o dispositivo, `Manual_Start` habilita o estado,
`Manual_Pump` renderiza e grava PCM **bloqueante** com sinal de 16 bits e little-endian,
e `Manual_Stop` / `Manual_Close` liberam os recursos.

- FreeBSD e DragonFly BSD: OSS `/dev/dsp` e ioctls SNDCTL_DSP_*
- NetBSD: `/dev/sound` nativo e `AUDIO_SETINFO` / `AUDIO_GETINFO`
- OpenBSD: sndio carregado **em runtime** por meio de `libsndio.so` e sua API documentada `sio_*`. Não se usa `-lsndio`, mas a biblioteca/servidor é necessária **em runtime**. Se estiver ausente, `Manual_Open` retorna `NOT_AVAILABLE`.
- Linux/macOS/Windows: retorna `NOT_SUPPORTED`.

Não são necessárias flags de linkedição `-lm`/`-pthread`/`-lsndio` para compilar e vincular este módulo. `sndio.h` é necessário no OpenBSD. O backend usa a libc e o driver do sistema operacional selecionado; não é independente dos serviços de runtime do sistema.

```c
#include "Cosmeron/Modules/Audio/Device/Manual.h"

static void render(void *ctx, float *dst, size_t frames, uint32_t channels) {
  /* Preenche dst com frames*channels amostras intercaladas em [-1,1]. */
  (void)ctx;
  memset(dst, 0, frames*channels*sizeof(*dst));
}
int main(void) {
  AUDIO_DEVICE_TYPE(TManual) device = {0};
  AUDIO_DEVICE_TYPE(TManualConfig) config = {
    48000, 2, render, NULL, NULL
  };
  if (AUDIO_DEVICE_FUNC(Manual_Open)(&device, &config) != STATUS_CONST(SUCCESS))
    return 1;
  (void)AUDIO_DEVICE_FUNC(Manual_Start)(&device);
  (void)AUDIO_DEVICE_FUNC(Manual_Pump)(&device, 512);
  (void)AUDIO_DEVICE_FUNC(Manual_Stop)(&device);
  AUDIO_DEVICE_FUNC(Manual_Close)(&device);
  return 0;
}
```

Os testes de CI fornecidos verificam **somente compilação, conversão de inteiros e linkedição**, sem abrir um dispositivo de áudio. Uma execução em hardware real é necessária para certificar negociação de taxa, agendamento, underruns e permissões. O callback deve ser rápido e não deve presumir a existência de uma thread de áudio dedicada.

## Sink PCM simulado e testes de ponta a ponta

`Manual_OpenSink(device, config, writeSink, userData)` abre um fluxo manual sem acessar hardware de áudio em qualquer sistema operacional. O callback do sink recebe PCM little-endian com sinal de 16 bits e informa o número de bytes aceitos. Escritas parciais são suportadas, mas retornar zero bytes com status de sucesso é tratado como erro de I/O. O buffer não pode ser retido após o retorno do callback; a sincronização e o contexto do sink são responsabilidade do chamador.

`Codespace/Tests/Audio/ManualSink.c` chama a função real `Manual_Pump` para 513 frames estéreo, força escritas parciais de 37 bytes, verifica cada amostra de saída e valida o ciclo de vida e os casos de erro. A aprovação dessa suíte não certifica nenhum dispositivo de áudio físico.

## Teste manual de aceitação em hardware (não executado pelo CI)

O arquivo opcional `ManualHardware.c` é compilado sob C11 estrito como parte da suíte de testes Audio. Ele **não** é executado automaticamente pelo CI. Em um host BSD com dispositivo de áudio acessível, execute:

```sh
make -C Codespace/Tests/Audio hardware-test CC=cc
# Ou selecione explicitamente o dispositivo:
Codespace/Tests/Audio/.build/ManualHardware /dev/dsp
```

Ele solicita um tom de 440 Hz em volume baixo por meio segundo, verifica as chamadas do sistema operacional, fecha os recursos e pede à pessoa que o executa para confirmar se o áudio foi ouvido sem underruns. Isso não é apropriado para runners de CI não assistidos sem alto-falantes/permissões. O argumento opcional do OpenBSD indica um nome de servidor/dispositivo sndio, não um caminho `/dev/dsp`.

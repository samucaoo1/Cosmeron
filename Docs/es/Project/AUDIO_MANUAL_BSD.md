# Reproducción manual de audio: backends BSD nativos

**Punto de entrada:** `Cosmeron/Modules/Audio/Device/Manual.h`.

A diferencia de los workers de dispositivo en segundo plano, esta API no tiene un hilo worker:
`Manual_Open` configura el dispositivo, `Manual_Start` habilita el estado,
`Manual_Pump` renderiza y escribe PCM **bloqueante** con signo de 16 bits y little-endian,
y `Manual_Stop` / `Manual_Close` liberan los recursos.

- FreeBSD y DragonFly BSD: OSS `/dev/dsp` e ioctls SNDCTL_DSP_*
- NetBSD: `/dev/sound` nativo y `AUDIO_SETINFO` / `AUDIO_GETINFO`
- OpenBSD: sndio cargado **en runtime** mediante `libsndio.so` y su API documentada `sio_*`. No se usa `-lsndio`, pero la biblioteca/servidor es necesario **en runtime**. Si falta, `Manual_Open` devuelve `NOT_AVAILABLE`.
- Linux/macOS/Windows: devuelve `NOT_SUPPORTED`.

No se necesitan flags de enlazado `-lm`/`-pthread`/`-lsndio` para compilar y enlazar este módulo. `sndio.h` es necesario en OpenBSD. El backend utiliza la libc y el driver del sistema operativo seleccionado; no es independiente de los servicios de runtime del sistema.

```c
#include "Cosmeron/Modules/Audio/Device/Manual.h"

static void render(void *ctx, float *dst, size_t frames, uint32_t channels) {
  /* Rellena dst con frames*channels muestras intercaladas en [-1,1]. */
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

Las pruebas de CI suministradas verifican **solo compilación, conversión de enteros y enlazado**, sin abrir un dispositivo de audio. Se necesita una ejecución en hardware real para certificar la negociación de frecuencia, la planificación, los underruns y los permisos. El callback debe ser rápido y no debe asumir que existe un hilo de audio dedicado.

## Sink PCM simulado y pruebas de extremo a extremo

`Manual_OpenSink(device, config, writeSink, userData)` abre un flujo manual sin acceder al hardware de audio en ningún sistema operativo. El callback del sink recibe PCM little-endian con signo de 16 bits e informa del número de bytes aceptados. Se admiten escrituras parciales, pero devolver cero bytes con un estado de éxito se trata como un error de I/O. El buffer no puede conservarse después de que el callback retorne; la sincronización y el contexto del sink son responsabilidad del llamador.

`Codespace/Tests/Audio/ManualSink.c` llama a la función real `Manual_Pump` para 513 frames estéreo, fuerza escrituras parciales de 37 bytes, comprueba cada muestra de salida y verifica el ciclo de vida y los casos de error. Que esta suite pase no certifica ningún dispositivo de audio físico.

## Prueba manual de aceptación en hardware (no ejecutada por CI)

El archivo opcional `ManualHardware.c` se compila bajo C11 estricto como parte de la suite de pruebas Audio. **No** se ejecuta automáticamente mediante CI. En un host BSD con un dispositivo de audio accesible, ejecuta:

```sh
make -C Codespace/Tests/Audio hardware-test CC=cc
# O selecciona explícitamente el dispositivo:
Codespace/Tests/Audio/.build/ManualHardware /dev/dsp
```

Solicita un tono de 440 Hz a bajo volumen durante medio segundo, comprueba las llamadas al sistema operativo, cierra los recursos y pide a la persona que lo ejecuta que confirme si oyó el audio sin underruns. Esto no es apropiado para runners de CI desatendidos sin altavoces/permisos. El argumento opcional de OpenBSD indica un nombre de servidor/dispositivo sndio, no una ruta `/dev/dsp`.

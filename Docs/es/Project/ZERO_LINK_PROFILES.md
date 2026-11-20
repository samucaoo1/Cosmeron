# Perfiles sin enlazado adicional — C11

Cosmeron distingue los flags de enlazado que necesita la aplicación de las capacidades disponibles en runtime. Usar un cargador dinámico de bibliotecas no elimina la dependencia **en runtime** de esa biblioteca; del mismo modo, una compilación C11 exitosa tampoco demuestra que funcione un backend de dispositivo.

## Audio (síntesis offline)

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

Funciona sin backends de audio de terceros. El namespace y las firmas de las funciones no cambian. `Audio.h` sigue siendo el agregador completo para escritorio; definir `AUDIO_NO_DESKTOP` todavía selecciona el comportamiento reducido anterior. Pruebas: `Synthesis`, `OfflineStandalone`, `OfflineCompat` y `OfflineMultiTU` en `Codespace/Tests/Audio/`.

Los decodificadores puros C11 de WAV y FLAC están disponibles por separado mediante `Offline.h` / `Codec/WAV.h` / `Codec/FLAC.h`.
La reproducción en dispositivos de escritorio y Player utilizan integración nativa de runtime del sistema operativo sin bibliotecas adicionales en el enlazado.

## Concurrency (solo cooperativo)

```c
#include "Cosmeron/Modules/Concurrency/Cooperative.h"
```

Proporciona Atomics lock-free (cuando es compatible), Coroutine sin pila y operaciones Task ejecutadas directamente. Este punto de entrada no proporciona paralelismo real, Mutex, ThreadPool, Join ni hilos worker. Es un perfil de API separado, **no** un sustituto del backend de threading nativo.
`Codespace/Tests/Concurrency/Cooperative.c` verifica el contrato C11 estricto sin bibliotecas adicionales en el enlazado.

## SQLite integrado

En FreeBSD, OpenBSD, NetBSD y DragonFly, la implementación predeterminada utiliza `DATABASE_SQLITE_THREADSAFE=0` (sin mutexes internos de SQLite). Pasó la prueba sin enlazado adicional en las cuatro VMs invitadas. Las llamadas concurrentes a SQLite **no están soportadas** en esta configuración, ni siquiera mediante conexiones distintas. Define `DATABASE_SQLITE_THREADSAFE=1` en la única unidad de traducción de implementación para optar por los mutexes; puede volver a ser necesaria una dependencia nativa de enlazado `-pthread`.

Consulta [SQLITE_LINKING.md](SQLITE_LINKING.md) para la configuración exacta.

## Reproducción manual de audio en BSD

Consulta [AUDIO_MANUAL_BSD.md](AUDIO_MANUAL_BSD.md). Incluye `Cosmeron/Modules/Audio/Device/Manual.h` para utilizar la API basada en callbacks `Manual_Open`, `Manual_Start`, `Manual_Pump`, `Manual_Stop` y `Manual_Close`. Cada llamada a pump renderiza PCM float intercalado, lo convierte a S16LE y escribe usando el hilo llamador del host. Este header no crea hilos automáticamente ni depende de vendors.

FreeBSD / DragonFly: OSS nativo; NetBSD: audio(4); OpenBSD: sndio resuelto mediante `dlopen`. OpenBSD **no** requiere el flag explícito `-lsndio`, pero **sí requiere** la biblioteca compartida sndio y un sndiod/dispositivo de audio funcional en runtime. Estos backends se prueban en compilación/enlazado en VMs BSD reales; la reproducción física todavía **no** está certificada.

## Codec WAV sin vendor

Incluye `Cosmeron/Modules/Audio/Codec/WAV.h` o `Audio/Offline.h`.
`Audio_Codec_WAV_DecodeMemory` admite RIFF/WAVE PCM sin signo de 8 bits, PCM con signo de 16/24/32 bits y float IEEE de 32 bits (en hosts IEEE binary32), y decodifica a un buffer PCM float intercalado con ownership. `Audio_Codec_WAV_WriteFile` escribe PCM16.
Ninguna de estas funciones importa vendors externos, hilos ni libm. WAVE comprimido/Extensible, RF64 y RIFX big-endian no están soportados; las entradas están limitadas por las longitudes RIFF y de los chunks de datos. Los programas de prueba `NativeWAV`, `WAVRobustness` y `WAVFormats` verifican los formatos admitidos y rechazan archivos malformados.

## Executor de tasks proporcionado por el usuario

Incluye `Cosmeron/Modules/Concurrency/Task/Executor.h` o `Concurrency/Cooperative.h`. Un callback `Executor_Submit` puede copiar y programar un `TTask` en el pool de workers ya existente en la aplicación. El adaptador no crea ningún hilo nativo automáticamente. Esto **no** convierte las implementaciones nativas de `Thread`, `Mutex` o `ThreadPool` en backends sin enlazado adicional. Consulta [EXTERNAL_EXECUTOR.md](EXTERNAL_EXECUTOR.md).

## Perfil nativo solo de decodificación (sin hilos de dispositivo)

Incluye `Cosmeron/Modules/Audio/CodecMinimal.h` para conservar la API pública de decodificación `Audio_Codec_*`, incluido el soporte nativo para WAV, MP3 y FLAC, excluyendo I/O de dispositivo, threading nativo y componentes de player.

```c
#include "Cosmeron/Modules/Audio/CodecMinimal.h"

int main(void) {
  AUDIO_CODEC_TYPE(TDecoder) decoder = {0};
  /* Audio_Codec_OpenFile(&decoder, "sample.flac");
     ... Audio_Codec_Close(&decoder); */
  return 0;
}
```

Esto **no** es lo mismo que el decodificador WAV sin vendor descrito arriba.
El perfil de codecs BSD resuelve los símbolos matemáticos necesarios
(`sin`, `exp`, `log`, `sqrt`, `pow`) desde la `libm` del host en runtime,
usando `dlopen`/`dlsym` y un inicializador de una sola ejecución con atomics C11. La implementación sigue **dependiendo de una libm compartida compatible instalada en el sistema de destino**. Una aplicación enlazada estáticamente necesita una validación independiente.

`CodecMinimal.c` y `CodecMath.c` se compilan **y ejecutan** sin flags adicionales de enlazado en cada VM BSD. Una prueba exitosa se registra como `AUDIO_CODEC_MINIMAL_ZERO_LINK=PASS`; un fallo permanece como `NOT_SUPPORTED`. El backend de escritorio Device/Player utiliza carga dinámica sin bibliotecas adicionales en el enlazado.

Una unidad de traducción que selecciona `CodecMinimal.h` compila decodificadores independientes sin hilos de dispositivos de reproducción.
Las funciones nativas de decodificación (WAV, FLAC, MP3) siguen cubiertas por sus respectivas pruebas.

El adaptador matemático de Linux usa `pthread_once` para inicializar símbolos matemáticos de runtime cuando se necesita resolución matemática dinámica.
Esta configuración no afirma evitar **todas** las dependencias posibles de runtime; solo evita nuevos flags de enlazado `-pthread` del usuario en los entornos probados.

## Variantes de libc Linux y runtime solo de decodificación

El decodificador mínimo ahora se prueba mediante smoke tests en contenedores reales **Fedora, Arch, Void glibc, Void musl y Debian**, usando la suite C11 `test-linux-codec-minimal.sh`. La suite decodifica WAV, MP3 y FLAC en memoria y ejercita las cinco operaciones matemáticas sin flags adicionales de enlazado. En glibc, la resolución matemática dinámica intenta primero `libm.so.6`; para otras disposiciones de libc, intenta `libm.so` y luego `RTLD_DEFAULT` (los símbolos del proceso en ejecución). Si falta una implementación matemática de runtime, la construcción del decodificador devuelve `NOT_AVAILABLE` en lugar de llamar sin comprobar un puntero de función nulo. **Esto no implica independencia del runtime matemático del sistema operativo.** Una aplicación enlazada estáticamente necesita una validación independiente.

### Header solo de decodificación entre unidades de traducción

`CodecMinimalTU_A.c` y `CodecMinimalTU_B.c` incluyen `CodecMinimal.h` de forma independiente, ejercitan funciones de decodificación y matemáticas y se enlazan con una tercera unidad de traducción. El ejecutable se prueba bajo C11 estricto, sin `-lm` ni `-pthread` adicionales, en cada VM BSD y contenedor de libc Linux. Esto comprueba el comportamiento real de enlazado multi-TU para el modo mínimo header-only; las dependencias matemáticas/dinámicas de runtime permanecen según lo documentado.

# Cosmeron — matriz de portabilidad y CI

_Actualizado: 2026-10-11. C11, orientada a headers, sin flags obligatorias de enlazado para el usuario._

Este documento distingue lo que ha sido **probado** de lo que es solo **plausible**. Una compilación cruzada exitosa no equivale a una prueba de ejecución en un dispositivo real. Todos los resultados deben comprobarse en GitHub Actions; que exista un workflow no significa que haya pasado.

## Verificación automatizada

| Objetivo | Entorno | Workflow | Alcance |
| --- | --- | --- | --- |
| Linux GCC/Clang | Runners nativos de GitHub | [By Operating System](../../.github/workflows/03-by-operating-system.yml) / [Simple Test](../../.github/workflows/01-simple-test.yml) | Suites existentes de integración de módulos |
| Windows MinGW/MSVC (10 y 11) | Runners nativos de GitHub | [By Operating System](../../.github/workflows/03-by-operating-system.yml) | Suites existentes de integración de módulos |
| macOS Clang (Apple Silicon e Intel) | Runners nativos de GitHub | [By Operating System](../../.github/workflows/03-by-operating-system.yml) | Suites existentes de integración de módulos |
| FreeBSD | VM invitada FreeBSD nativa mediante vmactions | [By Operating System](../../.github/workflows/03-by-operating-system.yml) | Suites nativas portables y extendidas; los informes por módulo distinguen perfiles sin enlazado adicional |
| OpenBSD / NetBSD / DragonFly BSD | VMs invitadas nativas mediante vmactions | [By Operating System](../../.github/workflows/03-by-operating-system.yml) | Suites nativas portables y extendidas; los informes por módulo distinguen perfiles sin enlazado adicional |
| Fedora / Arch / Debian / Ubuntu / openSUSE / Gentoo / Alpine | Contenedores nativos de espacio de usuario Linux | [By Operating System](../../.github/workflows/03-by-operating-system.yml) | Suites portables y nativas en diversas implementaciones de libc |
| Void glibc / Void musl | Contenedores nativos de espacio de usuario Linux | [By Operating System](../../.github/workflows/03-by-operating-system.yml) | Suites portables; cobertura de glibc y musl |
| Toolchain Nix / VM NixOS | Derivación Nix fijada en Ubuntu y VM invitada NixOS real | [By Operating System](../../.github/workflows/03-by-operating-system.yml) | C11 estricto y portable aprobado como paquete y dentro de la VM NixOS ([ejecución](https://github.com/samucaoo1/Cosmeron/actions/runs/38141169787)); no es una certificación completa de hardware |
| Proxy ChromeOS Crostini | Contenedor Debian en Ubuntu | [Experimental](../../.github/workflows/04-experimental.yml) | Suites portables y nativas en una aproximación al contenedor Crostini; **no** es un Chromebook real |

La suite de verificación portable es [test-portable-core.sh](../../.github/scripts/test-portable-core.sh). Compila y ejecuta los fuentes de prueba existentes del repositorio con `-std=c11 -Wall -Wextra -Wpedantic -Werror` y sin bibliotecas adicionales explícitas en el enlazado. El [ejecutor extendido BSD](../../.github/scripts/test-bsd-extended.sh) ejecuta Network, Concurrency, SQLite, Compression, Audio, Input y Terminal/Canvas no interactivo. Genera un **resumen de job de CI por módulo** que distingue compilación, ejecución en runtime, ausencia de enlazado adicional y alcance de dispositivo.

El SQLite integrado utiliza, de forma predeterminada en BSD, una implementación **single-threaded sin mutexes** y pasó las pruebas sin enlazado adicional en los cuatro entornos BSD ([diseño de SQLite](SQLITE_LINKING.md)). No debe accederse a él simultáneamente desde varios hilos.

Audio y Concurrency tienen subconjuntos puros e independientes sin enlazado adicional:
[Audio/Offline.h](../../Codespace/Cosmeron/Modules/Audio/Offline.h) proporciona PCM, síntesis, secuencias y renderizado de beep sin I/O de dispositivo ni hilos del sistema operativo;
[Concurrency/Cooperative.h](../../Codespace/Cosmeron/Modules/Concurrency/Cooperative.h) proporciona atomics, coroutines y tasks directas **sin threading nativo**.
Estos perfiles no equivalen a los backends Audio/Device de escritorio o al ThreadPool basado en pthread. En algunos BSD, los backends nativos de audio y threading todavía requieren `-pthread` y/o `-lm`; por tanto, sus pruebas de compatibilidad funcional **no** son certificaciones sin enlazado adicional. Los dispositivos físicos y las terminales interactivas aún no se han probado.

## Viabilidad adicional de plataformas

### NixOS

**Verificado para el subconjunto seleccionado de pruebas C11 portables en una VM NixOS real.**
`flake.nix` y `flake.lock` fijan Nixpkgs, exponen una derivación/verificación e inician una VM invitada NixOS en QEMU que ejecuta las pruebas empaquetadas. Tanto el paquete fijado como la VM NixOS real [pasaron CI](https://github.com/samucaoo1/Cosmeron/actions/runs/38141169787). El hardware específico del sistema operativo, los permisos y los drivers siguen sin verificarse.

### ChromeOS

**Viable mediante Crostini, con restricciones.** El entorno de desarrollo Linux de Google expone Debian dentro de una VM/contenedor administrado. Los módulos portables deberían comportarse como sus equivalentes Linux. El job Debian de CI aproxima ese espacio de usuario, pero **no** ejercita el kernel ni los permisos de dispositivos de ChromeOS.

Existe un script de verificación en un Chromebook real en [Tools/crostini/check.sh](../../Tools/crostini/check.sh); todavía no se ha ejecutado en un Chromebook. Las aplicaciones nativas de ChromeOS son un objetivo distinto; el acceso desde la terminal a HID, evdev, joysticks, USB y captura de GUI/entrada puede estar mediado o bloqueado. Valida Input, Network, Audio y Terminal en hardware Chromebook real. Considera los builds Android en ChromeOS por separado; el job Crostini no es una prueba de Android NDK.

Referencias oficiales:
- https://developers.google.com/chromeos/app-development/develop
- https://developers.google.com/chromeos/app-development/develop/setup
- https://www.chromium.org/chromium-os/developer-library/guides/containers/containers-and-vms/

### OpenHarmony

**La compilación cruzada C11 nativa para ARM64 con SDK 6.0 y el enlazado de múltiples unidades de traducción pasaron CI**:
[ejecución real del SDK OpenHarmony](https://github.com/samucaoo1/Cosmeron/actions/runs/38141169787).
Las macros y la triple del objetivo se conservan en los artefactos del workflow.
La prueba de compilador en [Tools/ohos](../../Tools/ohos/README.md) **no** ejecuta el código. El runtime OpenHarmony, las APIs vinculadas al sistema operativo, los dispositivos LiteOS, Huawei HarmonyOS NEXT y los permisos de hardware siguen sin verificarse.

Pasos:
1. Obtener y fijar un SDK/toolchain nativo OpenHarmony de confianza.
2. Comprobar todos los headers públicos con el Clang del objetivo y el sysroot OHOS; seguir explícitamente `__OHOS__` y otras macros predefinidas del objetivo emitidas por el compilador **real**.
3. Empezar por Core/Bit/Struct/Type/Math/Sort y memoria/contenedores puros.
4. Añadir pruebas de compatibilidad POSIX para Chronometry, Random, File, Concurrency, Network y Terminal; escribir backends de plataforma solo cuando sea necesario.
5. Validar la inclusión header-only en múltiples unidades de traducción y el contrato sin enlazado adicional; después ejecutar en un objetivo representativo del sistema estándar OHOS.
6. Tratar los productos mínimos basados en LiteOS como un perfil distinto de OpenHarmony estándar; un build ARM64 de espacio de usuario exitoso no certifica microcontroladores pequeños.

Referencias:
- https://github.com/openharmony/docs/blob/master/en/device-dev/subsystems/subsys-build-all.md
- https://gitee.com/openharmony/build/wikis/pages/export?doc_id=3015289&type=pdf

### Huawei HarmonyOS / HarmonyOS NEXT

**Certificación de plataforma separada; no es sinónimo de OpenHarmony.** Huawei documenta el desarrollo de aplicaciones HarmonyOS mediante DevEco Studio/HarmonyOS SDK, ArkTS/ArkUI y recursos nativos. Los algoritmos C reutilizables pueden integrarse mediante un puente nativo, si el SDK/runtime del objetivo lo admite. Las APIs de dispositivo/permisos, el empaquetado de aplicaciones y la compatibilidad con versiones concretas de HarmonyOS requieren validación con la toolchain de Huawei.

No consideres las macros Linux, la disponibilidad de POSIX, la compatibilidad ABI de Android ni una compilación cruzada OpenHarmony como prueba de soporte de HarmonyOS NEXT. Es preferible un pequeño ejemplo de puente ArkTS/nativo y pruebas en dispositivo cuando haya un SDK compatible.

Portal oficial para desarrolladores:
- https://developer.huawei.com/consumer/en/harmonyos/develop/

## Despliegue por etapas

1. **Obligatorio:** diez suites C11 portables en los cuatro entornos BSD; Fedora/Arch/Void glibc/Void musl; compilador proporcionado por Nix; proxy Debian Crostini.
2. **Integración BSD más amplia:** CLI, Parsing, Compression, Database, File, Audio, Terminal/Canvas, Concurrency y Network; diagnosticar fallos según los headers, llamadas al sistema y comportamiento de enlazado reales de BSD, no desactivando advertencias.
3. **Cobertura adicional:** variantes de libc, Chromebook real, dispositivo OpenHarmony, backends BSD completos sin enlazado adicional y pruebas de runtime en hardware. La VM NixOS y la compilación cruzada OHOS ARM64 ya están implementadas y tienen ejecuciones aprobadas.
4. **Certificación por módulo:** publicar resultados y limitaciones de los backends. Distinguir los estados compilado, probado por unidad, integrado y probado en hardware.

Esta es una biblioteca educativa: se mantienen algoritmos/módulos deliberadamente inusuales, pero las verificaciones de build no pueden pasar simplemente porque las pruebas se hayan omitido silenciosamente.

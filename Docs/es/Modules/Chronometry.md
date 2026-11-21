# Chronometry

El módulo **Chronometry** proporciona primitivas de medición de tiempo, duraciones con signo, relojes monotónicos y de pared, temporizadores, calendarios históricos y modernos, conversiones de épocas (incluido TAI $\leftrightarrow$ UTC con segundos intercalares), zonas horarias IANA con reglas de horario de verano (DST), catálogo país $\leftrightarrow$ zona y formateo/parsing ISO 8601 sin dependencia de `<stdio.h>`.

## Encabezados

~~~c
#include "Cosmeron/Modules/Chronometry/Chronometry.h"
~~~

El encabezado agregado incluye todos los subpaquetes del módulo:

- `Modules/Chronometry/Duration/Duration.h`
- `Modules/Chronometry/Clock/Clock.h`
- `Modules/Chronometry/Timer/Timer.h`
- `Modules/Chronometry/Instant/Instant.h`
- `Modules/Chronometry/Calendar/Calendar.h`
- `Modules/Chronometry/Timestamp/Timestamp.h`
- `Modules/Chronometry/Epoch/Epoch.h`
- `Modules/Chronometry/Timezone/Timezone.h`
- `Modules/Chronometry/Format/Format.h`

---

# Visión general

| Subpaquete | Tipos POD principales | Responsabilidad |
| --- | --- | --- |
| `Duration` | `TDuration` | Intervalos con signo en nanosegundos (`int64_t`) con aritmética protegida contra desbordamiento. |
| `Clock` | `TClockTimePoint` | Lectura del reloj monotónico del SO y pausa (`Sleep`). |
| `Timer` | `TTimer`, `TDelta`, `TFrameLimiter` | Cronometraje, delta entre cuadros y limitador de FPS. |
| `Instant` | `TInstant` | Instantes absolutos en segundos + nanosegundos UTC (`CLOCK_REALTIME` / `FILETIME`). |
| `Calendar` | `TDate`, `TIsoWeekDate`, `TCalendarPolicy` | Calendarios Gregoriano, Juliano y reformas históricas; aritmética y semana ISO 8601. |
| `Timestamp` | `TTimeOfDay`, `TTimestamp` | Fecha + hora civil (precisión de milisegundos) sujeta a una `CalendarPolicy`. |
| `Epoch` | — | Conversiones JDN, Rata Die, Unix y TAI $\leftrightarrow$ UTC (27 segundos intercalares). |
| `Timezone` | `TUtcOffset`, `TOffsetTimestamp`, `TDstTransitionRule`, `TTimezoneRule` | Base IANA (`tzdata 2026f`, 344 zonas, 253 enlaces, 249 países), desambiguación DST y desplazamiento local del SO. |
| `Format` | — | Formateo y parsing estricto ISO 8601 (`ToISO` / `FromISO`), 100% freestanding. |

Todos los tipos expuestos son **POD puros** (`STRUCT`), sin asignación dinámica ni punteros de `FunctionTable` incrustados. La emisión de `FunctionTable` de paquetes puede deshabilitarse en tiempo de compilación mediante `CHRONOMETRY_DISABLE_FUNCTION_TABLE` (módulo completo) o `CHRONOMETRY_<SUBPAQUETE>_DISABLE_FUNCTION_TABLE` (por subpaquete).

---

# Referencia de la API

## Duration

`MODULE_TYPE(Chronometry, Duration)` almacena `int64_t nanoseconds`.

- **Constantes (`Unit`):** `NANOSECONDS_PER_MICROSECOND`, `NANOSECONDS_PER_MILLISECOND`, `NANOSECONDS_PER_SECOND`, `MICROSECONDS_PER_MILLISECOND`, `MICROSECONDS_PER_SECOND`, `MILLISECONDS_PER_SECOND`, `SECONDS_PER_MINUTE`, `MINUTES_PER_HOUR`, `HOURS_PER_DAY`, `MINUTES_PER_DAY`, `SECONDS_PER_HOUR`, `SECONDS_PER_DAY`.
- **`VARIANT(From, ...)`:** `FromNanoseconds` (infalible), `FromMicroseconds`, `FromMilliseconds`, `FromSeconds`, `FromMinutes`, `FromHours`, `FromDays` (devuelven `OPSTATUS`, detectando `ARITHMETIC_OVERFLOW`).
- **`VARIANT(To, ...)`:** `ToNanoseconds`, `ToMicroseconds`, `ToMilliseconds`, `ToSeconds`.
- **Predicados y aritmética:** `IsZero`, `IsNegative`, `Add`, `Subtract`, `Compare`, `Absolute`, `Negate`, `Scale`.

~~~c
MODULE_TYPE(Chronometry, Duration) delay = {0};
OPSTATUS status = MODULE_FUNC(Chronometry, Duration, FromMilliseconds)(16, &delay);
~~~

## Clock y Timer

- **`Clock`:**
  - `Now(&outTimePoint)`: lee el reloj monotónico (`CLOCK_MONOTONIC` / `QueryPerformanceCounter` / `mach_absolute_time`).
  - `DurationBetween(start, end, &outDuration)`: calcula la duración entre dos `ClockTimePoint`.
  - `Sleep(duration)`: espera una duración no negativa (`nanosleep` reintentando en `EINTR` / `Sleep` en Windows).
- **`Timer`:**
  - `Start(&timer)`, `Elapsed(&timer, &outElapsed)`, `Restart(&timer, &outElapsed)`.
  - `SPACE(Delta)`: `Delta_Start(&delta)`, `Delta_Update(&delta, &outElapsed)`.
  - `SPACE(FrameLimiter)`: `Create(target, &limiter)`, `FromFPS(fps, &limiter)`, `Begin(&limiter)`, `Remaining(&limiter, &outRemaining)`, `Wait(&limiter)`. `Remaining` y `Wait` devuelven `INVALID_ARGUMENT` si aún no se ha llamado a `Begin`.

## Instant

`MODULE_TYPE(Chronometry, Instant)` almacena `int64_t seconds` y `uint32_t nanoseconds` (`0 <= nanoseconds < 10^9`).

- `Now(&outInstant)`: lee el reloj de pared UTC del sistema (`CLOCK_REALTIME` / `GetSystemTimeAsFileTime`).
- `IsValid(instant)`, `Compare(left, right)`, `AddDuration(instant, duration, &outResult)`, `DurationBetween(start, end, &outResult)`.

## Calendar

- **Políticas y sistemas:** `GregorianPolicy()`, `SystemPolicy(system, &outPolicy)`, `ReformPolicy(reformId, &outPolicy)` (`CATHOLIC_1582`, `BRITISH_1752`, `RUSSIAN_1918`).
- **Utilidades:** `IsLeapYear(system, year)`, `DaysInMonth(system, year, month, &outDays)`, `DayOfYear(policy, date, &outDoy)`, `DayOfYearCumulative(system, date, &outDoy)`.
- **`SPACE(Date)`:**
  - `Compare`, `IsValid`, `ToDayOfYear`, `ToOrdinal`, `FromOrdinal`, `Difference`.
  - `DayOfWeek(policy, date, &outWeekday)` (`MONDAY = 1` .. `SUNDAY = 7`).
  - `FromNthWeekday(system, year, month, occurrence, weekday, &outDate)`.
  - `FromWeekdayAnchor(system, year, month, anchorKind, anchorDay, weekday, &outDate)` (`WEEKDAY_EXACT`, `WEEKDAY_ON_OR_AFTER`, `WEEKDAY_ON_OR_BEFORE`, `WEEKDAY_LAST`).
  - `AddDays`, `AddMonths`, `AddYears` (con ajuste de fin de mes en años comunes y bisiestos).
  - `ToIsoWeek(policy, date, &outIsoWeek)` y `FromIsoWeek(policy, isoWeek, &outDate)`.

## Timestamp

`MODULE_TYPE(Chronometry, Timestamp)` combina `Date date` y `TimeOfDay time` (`hour`, `minute`, `second`, `millisecond`).

- `IsValid(policy, value)`, `Compare(left, right)`, `AddDuration(policy, value, duration, &outResult)`, `Difference(policy, left, right, &outResult)`.
- `SPACE(Time)`: `Time_IsValid`, `Time_Compare`.

## Epoch

Organizado en familias `VARIANT(...)`:

- **`VARIANT(DateTo)`:** `DateToJulianDayNumber(policy, date, &jdn)`, `DateToRataDie(date, &rataDie)`.
- **`VARIANT(DateFrom)`:** `DateFromJulianDayNumber(policy, jdn, &date)`, `DateFromRataDie(rataDie, &date)`.
- **`VARIANT(TimestampTo)`:** `TimestampToUnix(timestamp, &instant)`.
- **`VARIANT(TimestampFrom)`:** `TimestampFromUnix(instant, &timestamp)`.
- **`VARIANT(TAI)`:** `TAIMinusUtcSeconds(utc, &outSeconds)`, `TAIFromUtc(utc, &outTai)`, `TAIToUtc(tai, &outUtc)` (usando la tabla oficial de 27 segundos intercalares de `tzdata`, válida desde `1972-01-01T00:00:00Z`).

## Timezone

- **Conversiones UTC:** `ToUTC(offsetTimestamp, &instant)`, `FromUTC(instant, offset, &offsetTimestamp)`, `FromUTCWithRule(instant, &rule, &offsetTimestamp)`, `FromUTCWithZone(instant, zoneId, &offsetTimestamp)`.
- **Zona horaria local del SO:** `SystemOffsetAt(instant, &outOffset, &outIsDst)`, `SystemNow(&outOffsetTimestamp)`.
- **`SPACE(Offset)`:** `IsValid(offset)` (`[-1439, +1439]` minutos), `FromHoursMinutes(hours, minutes, &outOffset)`.
- **`SPACE(Zone)`:**
  - `DataVersion()` (devuelve `"2026f"`).
  - `FromId(zoneId, &outRule)`, `FromName(ianaName, &outRule)` (búsqueda binaria en 344 zonas canónicas y 253 alias de `backward`).
  - `IsRuleValid(&rule)`, `IsDstAtLocal(&rule, local, &outIsDst)`.
  - `ClassifyLocal(&rule, local, &outStatus, &outEarlier, &outLater)` (`LOCAL_TIME_UNIQUE`, `LOCAL_TIME_AMBIGUOUS`, `LOCAL_TIME_NONEXISTENT`).
  - `ResolveLocal(&rule, local, &outResolved)` y `ResolveLocalWithPolicy(&rule, local, policy, &outResolved)` (`DISAMBIGUATE_COMPATIBLE`, `DISAMBIGUATE_EARLIER`, `DISAMBIGUATE_LATER`, `DISAMBIGUATE_REJECT`).
- **`SPACE(Country)`:** `NameFromCode(code, &outName)`, `ZoneCount(code, &outCount)`, `ZoneIdAt(code, index, &outZoneId)`, `ZoneRuleAt(code, index, &outRule)`.

### Actualización de tablas `tzdata`

Los archivos `IanaZones.inc` y `LeapSeconds.inc` se generan desde `Codespace/Cosmeron/Asset/TimeZone/` mediante:

~~~sh
python3 .github/scripts/generate-tzdata.py
~~~

## Format

- **Capacidades (`BufferCapacity`):** `ISO_DATE_BUFFER_SIZE` (`16`), `ISO_TIMESTAMP_BUFFER_SIZE` (`32`), `ISO_OFFSET_TIMESTAMP_BUFFER_SIZE` (`40`), `ISO_INSTANT_BUFFER_SIZE` (`32`), `ISO_DURATION_BUFFER_SIZE` (`48`).
- **`VARIANT(ToISO)`:** `ToISODate`, `ToISOTimestamp`, `ToISOOffsetTimestamp`, `ToISOInstant`, `ToISODuration`.
- **`VARIANT(FromISO)`:** `FromISODate`, `FromISOTimestamp`, `FromISOOffsetTimestamp`, `FromISOInstant`, `FromISODuration`.
- **Reglas ISO 8601:** los años en `[0000, 9999]` se emiten sin signo `+`; los años fuera de ese rango emiten signo explícito (`-0044-03-15`, `+10000-01-01`). El parsing es estricto (rechaza espacios y campos sin ceros a la izquierda) y no depende de `<stdio.h>`.


## Semántica, límites y uso práctico

### Cómo elegir la representación temporal correcta

Chronometry separa intencionalmente la medición de intervalos del tiempo civil:

- **ClockTimePoint / Clock:** contador monotónico destinado a medir tiempo transcurrido. Su origen no está especificado; trate el valor como opaco, no lo serialice como una fecha de calendario y no lo compare con un Instant.
- **Instant:** instante absoluto de reloj de pared, representado por segundos Unix y nanosegundos. Es adecuado para marcas de tiempo e intercambio, pero el reloj del sistema puede ajustarse; no lo utilice para medir intervalos.
- **Duration:** intervalo con signo en nanosegundos. Úselo para tiempo transcurrido, operaciones aritméticas y esperas.
- **Timestamp:** fecha y hora civil con precisión de milisegundos. No tiene zona horaria ni desplazamiento UTC por sí mismo. Una CalendarPolicy determina cómo se interpreta su fecha.
- **OffsetTimestamp:** Timestamp local asociado a un desplazamiento UTC numérico y a un indicador de horario de verano. El desplazamiento es un dato, no una zona horaria con nombre.

Utilice Clock o Timer para temporización de fotogramas, timeouts y mediciones de rendimiento. Utilice Instant, Timestamp y Timezone para fechas de reloj de pared, agendas y conversiones entre hora local y UTC.

### Política de calendario

Una Date no es intrínsecamente gregoriana ni juliana. Pase la misma CalendarPolicy a la validación, conversión ordinal, aritmética y operaciones con timestamps cuando esos valores pertenezcan a un sistema de calendario concreto. GregorianPolicy() es la opción práctica para fechas civiles modernas. SystemPolicy() selecciona un sistema gregoriano o juliano puro; ReformPolicy() modela una de las transiciones históricas integradas y sus fechas civiles suprimidas.

AddMonths() y AddYears() ajustan el día cuando el mes de destino es más corto (por ejemplo, al mover una fecha del día 31 a un mes de 30 días). Las operaciones que atraviesan un salto de reforma o salen del rango admitido devuelven un estado en lugar de inventar silenciosamente una fecha.

### Zonas horarias y transiciones de horario de verano

El catálogo de zonas se genera a partir de los datos IANA incluidos en el repositorio. TimezoneRule es un perfil compacto de reglas: desplazamiento UTC estándar, ajuste DST y reglas recurrentes de inicio/fin de las transiciones. No es una base TZif histórica completa. No presuponga que esta representación reproduce todos los cambios políticos históricos ni todas las transiciones pasadas de una localidad.

Una hora civil local puede ser:

- LOCAL_TIME_UNIQUE: corresponde a exactamente un instante UTC.
- LOCAL_TIME_AMBIGUOUS: el reloj se retrasó y esa hora local ocurrió dos veces.
- LOCAL_TIME_NONEXISTENT: el reloj avanzó y esa hora fue omitida.

ClassifyLocal() informa la clasificación y los candidatos anterior/posterior en OffsetTimestamp. ResolveLocal() utiliza la política compatible: selecciona la ocurrencia anterior en una superposición y el candidato posterior en un salto. ResolveLocalWithPolicy() permite elegir explícitamente DISAMBIGUATE_EARLIER, DISAMBIGUATE_LATER o DISAMBIGUATE_REJECT; la política de rechazo devuelve OUT_OF_RANGE cuando la hora local no es única. Prefiera una política explícita al procesar agendas introducidas por usuarios.

FromUTC() con un desplazamiento numérico realiza una conversión de desplazamiento fijo. FromUTCWithRule() y FromUTCWithZone() aplican el perfil de reglas seleccionado. SystemOffsetAt() y SystemNow() consultan la configuración de zona horaria local del sistema operativo; no modifican esa configuración.

### Entrada y salida ISO 8601

El paquete Format escribe en buffers proporcionados por el llamador y no asigna memoria ni depende de <stdio.h>. Las constantes de capacidad incluyen el carácter nulo terminador. Pase siempre la capacidad real del buffer y compruebe el OPSTATUS devuelto; la capacidad insuficiente se informa como OUT_OF_RANGE.

- Una fecha se interpreta usando el calendario gregoriano.
- Un timestamp es una fecha-hora local (date + T + hora) y no tiene desplazamiento.
- Un timestamp con desplazamiento incluye Z o un desplazamiento numérico.
- Un instante se interpreta mediante un timestamp con desplazamiento y se convierte a UTC.
- Una duración utiliza los componentes de día/tiempo admitidos (D, H, M, S) y segundos fraccionarios opcionales; los meses y años del calendario no son unidades de duración de longitud fija.

El parsing es estricto: toda la entrada debe coincidir con el formato esperado. Se rechazan los espacios iniciales/finales, los campos sin padding y los caracteres sobrantes. FromISODate() y FromISOTimestamp() utilizan la política gregoriana; para calendarios históricos o julianos, utilice directamente la API Calendar.

### Gestión de errores y propiedad

Las operaciones que pueden fallar devuelven OPSTATUS y escriben los resultados mediante parámetros de salida. Compruebe el estado antes de consumir el valor de salida. Los tipos públicos del módulo son estructuras POD: el llamador controla su almacenamiento y los valores temporales centrales no requieren asignación dinámica. Las cadenas devueltas por las funciones de consulta de zonas y países hacen referencia a datos estáticos integrados; el llamador no debe liberarlas.


## Contratos detallados

### Duration

TDuration.nanoseconds es un int64_t con signo. Los constructores de unidades validan los punteros de salida y detectan desbordamientos de multiplicación; FromNanoseconds es un constructor directo. ToMicroseconds, ToMilliseconds y ToSeconds utilizan división entera, descartan fracciones y truncan hacia cero. No redondean. Absolute y Negate devuelven ARITHMETIC_OVERFLOW para INT64_MIN. Add, Subtract y Scale también detectan desbordamientos en lugar de permitir wraparound.

### Ciclo de vida de Clock y Timer

Clock.Now devuelve una lectura monotónica en nanosegundos. Su época no está especificada; las lecturas solo tienen sentido al calcular diferencias dentro del mismo dominio de reloj. DurationBetween tiene signo y devuelve un valor negativo cuando end precede a start.

Clock.Sleep rechaza duraciones negativas con INVALID_ARGUMENT; cero es una operación exitosa sin espera. El sistema operativo puede reanudar el hilo después del tiempo solicitado; Sleep no garantiza planificación en tiempo real.

Timer.Start establece el inicio. Elapsed consulta el intervalo sin modificarlo; Restart devuelve el intervalo y mueve el inicio a la nueva lectura. Timer.Delta.Start establece la lectura anterior; Delta.Update devuelve el intervalo desde la actualización anterior y avanza esa lectura.

Utilice el limitador de fotogramas en este orden: Create o FromFPS; Begin al inicio de cada fotograma; ejecute el trabajo; después Wait o Remaining. Wait no inicia automáticamente el siguiente fotograma. Llame a Begin nuevamente. Remaining y Wait devuelven INVALID_ARGUMENT antes de la primera llamada exitosa a Begin. FromFPS calcula nanosegundos enteros por fotograma (1.000.000.000 / fps), por lo que las tasas que no dividen mil millones utilizan un intervalo truncado.

### Instant y conversiones de época

Instant se normaliza como segundos más nanosegundos en el intervalo [0, 1.000.000.000). Los instantes negativos utilizan la misma representación; medio segundo antes de la época Unix es {-1, 500000000}.

Instant.Now lee el reloj de pared del sistema operativo, que puede ajustarse por sincronización, cambios administrativos o virtualización. DurationBetween calcula la diferencia entre los instantes proporcionados; no garantiza que el reloj de pared avance monotónicamente.

Epoch.TimestampToUnix interpreta Timestamp como fecha-hora civil UTC en el calendario gregoriano. Timestamp no contiene zona horaria: no utilice esta conversión para una hora local sin convertirla primero a UTC. TimestampFromUnix realiza la conversión inversa UTC/gregoriana.

DateToJulianDayNumber y DateFromJulianDayNumber requieren una política de calendario explícita. Rata Die utiliza la convención gregoriana de esta implementación y no recibe una política.

TAI utiliza la tabla integrada de segundos intercalares. TAIMinusUtcSeconds devuelve OUT_OF_RANGE antes del período cubierto por la tabla. La tabla no se obtiene ni actualiza durante la ejecución. TAI utiliza el mismo contenedor de segundos/nanosegundos que Instant, pero representa otra escala temporal; no trate TAI como una marca de tiempo Unix UTC.

### Políticas de calendario y rango de fechas

CalendarPolicy es una regla de interpretación, no metadatos almacenados en Date. Mantenga la misma política en operaciones relacionadas de validación, aritmética, ordinal y día de la semana. GregorianPolicy devuelve la política gregoriana por valor; SystemPolicy crea una política gregoriana o juliana pura; ReformPolicy crea una de las reformas históricas predefinidas.

Las reformas representan una discontinuidad entre la última fecha del sistema antiguo y la primera del nuevo. Las fechas del intervalo suprimido son inválidas bajo esa política. El modelo cubre solo las tres transiciones predefinidas; no es una base histórica de calendarios por país.

AddMonths limita el día original al último día del mes de destino. AddYears utiliza aritmética de meses; mover el 29 de febrero a un año no bisiesto produce el último día de febrero. FromNthWeekday admite FIRST_WEEKDAY a FOURTH_WEEKDAY y LAST_WEEKDAY. FromWeekdayAnchor ofrece coincidencia exacta, en el día o después, en el día o antes y última coincidencia relativa a un día ancla.

Las fechas de semana ISO utilizan el lunes como día 1 y el domingo como día 7. El año ISO puede diferir del año civil cerca de Año Nuevo. La política es necesaria para convertir entre fecha civil y fecha de semana ISO.

La implementación admite años civiles desde -4799 hasta 4.194.303. Las operaciones de calendario rechazan valores fuera de este rango, aunque el campo de bits del año de Date puede codificar un rango con signo más amplio. Este límite es una frontera de la biblioteca, no una afirmación sobre la validez histórica de las fechas.

### Modelo de reglas de zona horaria

UtcOffset.minutesEastOfUtc es el desplazamiento con signo en minutos al este de UTC: UTC−03:00 equivale a -180 y UTC+05:30 a 330. El rango aceptado es de -1439 a +1439 minutos. FromHoursMinutes recibe componentes de hora y minuto con signo; utilice el mismo signo para offsets negativos.

TimezoneRule contiene un desplazamiento estándar, un ajuste DST y dos reglas recurrentes de transición. Cada regla registra mes, ancla del día de la semana, día de la semana, hora/minuto local y base temporal (TIME_BASE_WALL, TIME_BASE_STANDARD o TIME_BASE_UTC). Es un modelo compacto, no un historial TZif completo. No representa todos los cambios históricos, cambios políticos no recurrentes ni secuencias arbitrarias de transiciones.

FromName resuelve nombres IANA canónicos y alias integrados. FromId resuelve el ZoneId generado. ianaName apunta a almacenamiento estático y no debe liberarse. Las consultas de países también devuelven cadenas estáticas y registros de zonas integrados. Los códigos e índices no válidos devuelven un estado de error.

FromUTC aplica un desplazamiento numérico fijo. FromUTCWithRule y FromUTCWithZone aplican un perfil a un instante absoluto. SystemOffsetAt consulta el desplazamiento local del sistema operativo en el instante indicado; SystemNow combina el instante actual con el desplazamiento local. Estas funciones dependen de la configuración de zonas del sistema, que puede diferir del catálogo integrado.

Para una hora local inexistente, ClassifyLocal devuelve candidatos a ambos lados de la transición hacia delante. Para una hora ambigua, devuelve las ocurrencias anterior y posterior. Para una hora única, ambas salidas son iguales. ResolveLocalWithPolicy rechaza horas no únicas con DISAMBIGUATE_REJECT; EARLIER y LATER seleccionan el candidato correspondiente. COMPATIBLE elige la ocurrencia anterior en una superposición y el candidato posterior en un salto.

### Contratos ISO 8601

Format es un formateador/parser con buffers limitados. No asigna memoria ni utiliza <stdio.h>. Las funciones de salida requieren un buffer no nulo y capacidad distinta de cero; el parsing requiere entrada y punteros de salida no nulos. La capacidad debe incluir el byte nulo terminador. Un buffer insuficiente devuelve OUT_OF_RANGE.

- ToISODate y FromISODate utilizan el calendario gregoriano.
- ToISOTimestamp y FromISOTimestamp operan sobre fecha-hora civil local sin desplazamiento.
- ToISOOffsetTimestamp y FromISOOffsetTimestamp conservan un desplazamiento UTC numérico. El indicador DST no se codifica en el texto ISO.
- ToISOInstant siempre formatea UTC con Z. FromISOInstant acepta un timestamp con desplazamiento y lo convierte a UTC.
- ToISODuration emite una duración ISO firmada de día/tiempo. El formateador normaliza los días completos en horas; el parser acepta componentes de día y tiempo.

El parsing consume toda la entrada. Se rechazan espacios iniciales/finales, campos incompletos, fechas/horas inválidas y caracteres sobrantes. Las fechas civiles ISO utilizan el calendario gregoriano, independientemente de las políticas históricas usadas en otras partes del módulo. El parser/formateador de fechas admite años extendidos con signo.

### Resolución de los relojes y concurrencia

Los campos en nanosegundos son una unidad de representación, no una garantía de precisión de nanosegundos del reloj ni de planificación. `Clock.Now` convierte el contador de la plataforma a nanosegundos: en Windows utiliza QueryPerformanceCounter y su frecuencia informada; en macOS utiliza mach_absolute_time y la base de tiempo de Mach; en plataformas POSIX utiliza clock_gettime(CLOCK_MONOTONIC). La resolución efectiva, la estabilidad y el coste de lectura dependen de la plataforma y del entorno de ejecución. No deduzca la resolución física del reloj a partir de la unidad entera utilizada.

`Instant.Now` lee el reloj civil del sistema mediante GetSystemTimeAsFileTime en Windows o clock_gettime(CLOCK_REALTIME) en plataformas POSIX. Su precisión fraccionaria también está limitada por el reloj subyacente. Los valores FILETIME de Windows utilizan unidades de 100 nanosegundos, pero esa unidad no garantiza una precisión de 100 nanosegundos. Los ajustes del reloj civil pueden hacer que las lecturas sucesivas avancen o retrocedan.

`Clock.Sleep` y `FrameLimiter.Wait` solicitan un retraso mínimo, no una garantía de plazo. La carga del planificador, la granularidad de los temporizadores y las políticas del sistema operativo pueden prolongar la espera real. Por tanto, el limitador de fotogramas ayuda a controlar la cadencia, pero no es un planificador de tiempo real.

Chronometry almacena el estado de los temporizadores en estructuras propiedad del llamador y sus tablas de zonas horarias integradas son datos estáticos. La API no proporciona sincronización para una estructura modificada simultáneamente por varios hilos: utilice una instancia propia de Timer, Delta o FrameLimiter por hilo, o sincronice el acceso compartido. El uso de solo lectura de valores inmutables y de los datos integrados de consulta no requiere inicialización gestionada por la biblioteca. Las consultas de zona horaria del sistema siguen la configuración del host; los cambios concurrentes en la configuración global de zona horaria del proceso quedan fuera del contrato de sincronización de Chronometry.

## Ejemplos prácticos

### Medir un intervalo

~~~c
#include "Cosmeron/Modules/Chronometry/Chronometry.h"

MODULE_TYPE(Chronometry, Timer) timer = {0};
MODULE_TYPE(Chronometry, Duration) elapsed = {0};

if (MODULE_FUNC(Chronometry, Timer, Start)(&timer) != STATUS_CONST(SUCCESS)) {
    return 1;
}

/* Ejecute aquí la operación que desea medir. */

if (MODULE_FUNC(Chronometry, Timer, Elapsed)(&timer, &elapsed) !=
    STATUS_CONST(SUCCESS)) {
    return 1;
}
~~~

### Aritmética de calendario

~~~c
MODULE_TYPE(Chronometry, CalendarPolicy) gregorian =
    MODULE_FUNC(Chronometry, Calendar, GregorianPolicy)();
MODULE_TYPE(Chronometry, Date) nextMonth = {0};

OPSTATUS status = MODULE_FUNC(Chronometry, Calendar, Date, AddMonths)(
    &gregorian, (MODULE_TYPE(Chronometry, Date)){2024, 1, 31}, 1, &nextMonth);

/* Si tiene éxito, nextMonth será 2024-02-29. */
~~~

### Resolver una superposición de horario de verano

~~~c
MODULE_TYPE(Chronometry, TimezoneRule) newYork = {0};
MODULE_TYPE(Chronometry, Timestamp) local = {
    .date = {2026, 11, 1},
    .time = {.hour = 1, .minute = 30}
};
MODULE_TYPE(Chronometry, OffsetTimestamp) resolved = {0};

OPSTATUS status = MODULE_FUNC(Chronometry, Timezone, Zone, FromName)(
    "America/New_York", &newYork);

if (status == STATUS_CONST(SUCCESS)) {
    status = MODULE_FUNC(Chronometry, Timezone, Zone,
                         ResolveLocalWithPolicy)(
        &newYork, local,
        MODULE_CONST(Chronometry, Timezone, DISAMBIGUATE_LATER),
        &resolved);
}
~~~

### Formatear un instante

~~~c
MODULE_TYPE(Chronometry, Instant) instant = {0};
char text[MODULE_CONST(Chronometry, Format, ISO_INSTANT_BUFFER_SIZE)];

OPSTATUS status = MODULE_FUNC(Chronometry, Instant, Now)(&instant);
if (status == STATUS_CONST(SUCCESS)) {
    status = MODULE_FUNC(Chronometry, Format, ToISOInstant)(
        instant, text, sizeof(text));
}
~~~

## Cabeceras públicas y mapa de implementación

| Subpaquete | Cabecera pública | Implementación |
| --- | --- | --- |
| Duration | Duration/Duration.h | Duration/Impl/Duration.impl |
| Clock | Clock/Clock.h | Clock/Impl/Clock.impl |
| Timer | Timer/Timer.h | Timer/Impl/Timer.impl |
| Instant | Instant/Instant.h | Instant/Impl/Instant.impl |
| Calendar | Calendar/Calendar.h | Calendar/Impl/Calendar.impl |
| Timestamp | Timestamp/Timestamp.h | Timestamp/Impl/Timestamp.impl |
| Epoch | Epoch/Epoch.h | Epoch/Impl/Epoch.impl y Epoch/Impl/LeapSeconds.inc |
| Timezone | Timezone/Timezone.h | Timezone/Impl/Timezone.impl y Timezone/Impl/IanaZones.inc |
| Format | Format/Format.h | Format/Impl/Format.impl |

La prueba de integración está en Codespace/Tests/Chronometry/Chronometry.c. Comprueba conversiones, reformas de calendario, semanas ISO, TAI/UTC, saltos y superposiciones DST, alias de zonas, consulta de países y formateo/parsing.
---

# Portabilidad

- `Chronometry/Impl/Common.inc` define `_POSIX_C_SOURCE 200809L` si aún no está definido, permitiendo compilación limpia con `-std=c11` estricto en hosts POSIX.
- Toda detección de plataforma utiliza `Detect.h` (`OS_WINDOWS`, `OS_MAC`, POSIX).

## Módulos relacionados

- [Module](Module.md) — DSL declarativo (`MODULE`, `PACKAGE`, `VARIANT`, `STRUCT`, `ENUM_DEF_DIRECT`).
- [Condition](Condition.md) — contrato de retorno `OPSTATUS`.
- [Algorithm](Algorithm.md) — contrato `CMPOUT` de comparación.

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
---

# Portabilidad

- `Chronometry/Impl/Common.inc` define `_POSIX_C_SOURCE 200809L` si aún no está definido, permitiendo compilación limpia con `-std=c11` estricto en hosts POSIX.
- Toda detección de plataforma utiliza `Detect.h` (`OS_WINDOWS`, `OS_MAC`, POSIX).

## Módulos relacionados

- [Module](../Modules/Module.md) — DSL declarativo (`MODULE`, `PACKAGE`, `VARIANT`, `STRUCT`, `ENUM_DEF_DIRECT`).
- [Condition](../Modules/Condition.md) — contrato de retorno `OPSTATUS`.
- [Algorithm](../Modules/Algorithm.md) — contrato `CMPOUT` de comparación.

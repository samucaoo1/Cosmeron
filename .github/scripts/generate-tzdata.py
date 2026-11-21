#!/usr/bin/env python3
"""Generate Chrono X-Macro tables from IANA tzdata text files."""

from __future__ import annotations

import argparse
import calendar
import os
import re
from dataclasses import dataclass

ZONE_FILES = (
    "africa",
    "antarctica",
    "asia",
    "australasia",
    "europe",
    "northamerica",
    "southamerica",
    "etcetera",
    "backward",
)

MONTH_NAMES = {
    "jan": ("JANUARY", 1),
    "feb": ("FEBRUARY", 2),
    "mar": ("MARCH", 3),
    "apr": ("APRIL", 4),
    "may": ("MAY", 5),
    "jun": ("JUNE", 6),
    "jul": ("JULY", 7),
    "aug": ("AUGUST", 8),
    "sep": ("SEPTEMBER", 9),
    "oct": ("OCTOBER", 10),
    "nov": ("NOVEMBER", 11),
    "dec": ("DECEMBER", 12),
}

WEEKDAY_NAMES = {
    "mon": "MONDAY",
    "tue": "TUESDAY",
    "wed": "WEDNESDAY",
    "thu": "THURSDAY",
    "fri": "FRIDAY",
    "sat": "SATURDAY",
    "sun": "SUNDAY",
}


@dataclass(frozen=True)
class Transition:
    month: str
    anchor_kind: str
    anchor_day: int
    weekday: str
    hour: int
    minute: int
    time_base: str


@dataclass(frozen=True)
class ZoneEntry:
    enum_id: str
    iana_name: str
    std_offset_min: int
    dst_savings_min: int
    start: Transition
    end: Transition


@dataclass(frozen=True)
class LinkEntry:
    alias_name: str
    target_enum_id: str


@dataclass(frozen=True)
class LeapEntry:
    unix_seconds: int
    tai_minus_utc: int


@dataclass(frozen=True)
class CountryEntry:
    code: str
    name: str


@dataclass(frozen=True)
class CountryZoneEntry:
    code: str
    zone_enum_id: str


DEFAULT_TRANSITION = Transition(
    month="JANUARY",
    anchor_kind="WEEKDAY_EXACT",
    anchor_day=1,
    weekday="MONDAY",
    hour=0,
    minute=0,
    time_base="TIME_BASE_WALL",
)


def parse_offset_minutes(token: str) -> int:
    if token == "-":
        return 0
    sign = -1 if token.startswith("-") else 1
    body = token[1:] if token[0] in "+-" else token
    parts = body.split(":")
    hours = int(parts[0])
    minutes = int(parts[1]) if len(parts) > 1 else 0
    return sign * (hours * 60 + minutes)


def parse_at(token: str) -> tuple[int, int, str]:
    base = "TIME_BASE_WALL"
    if token.endswith(("s", "g", "u", "z", "w")):
        suffix = token[-1]
        token = token[:-1]
        if suffix == "s":
            base = "TIME_BASE_STANDARD"
        elif suffix in ("g", "u", "z"):
            base = "TIME_BASE_UTC"
    parts = token.split(":")
    hour = int(parts[0])
    minute = int(parts[1]) if len(parts) > 1 else 0
    return hour, minute, base


def parse_on(token: str) -> tuple[str, int, str]:
    low = token.lower()
    if low.startswith("last"):
        wd = WEEKDAY_NAMES[low[4:7]]
        return "WEEKDAY_LAST", 1, wd
    m = re.fullmatch(r"([a-z]{3})(>=|<=)(\d+)", low)
    if m:
        wd = WEEKDAY_NAMES[m.group(1)]
        kind = "WEEKDAY_ON_OR_AFTER" if m.group(2) == ">=" else "WEEKDAY_ON_OR_BEFORE"
        return kind, int(m.group(3)), wd
    day = int(token)
    return "WEEKDAY_EXACT", day, "MONDAY"


def parse_month(token: str) -> str:
    return MONTH_NAMES[token[:3].lower()][0]


def sanitize_enum_id(iana_name: str) -> str:
    ident = iana_name.replace("+", "_PLUS_").replace("-", "_MINUS_")
    ident = re.sub(r"[^A-Za-z0-9]+", "_", ident).strip("_").upper()
    ident = re.sub(r"_+", "_", ident)
    if ident[0].isdigit():
        ident = "ZONE_" + ident
    return ident


def parse_tzdata(input_dir: str) -> tuple[str, list[ZoneEntry], list[LinkEntry]]:
    version_path = os.path.join(input_dir, "version")
    version = "unknown"
    if os.path.isfile(version_path):
        with open(version_path, "r", encoding="utf-8") as handle:
            version = handle.read().strip()

    zones: dict[str, list[list[str]]] = {}
    rules: dict[str, list[list[str]]] = {}
    raw_links: list[tuple[str, str]] = []
    cur_zone: str | None = None

    for filename in ZONE_FILES:
        path = os.path.join(input_dir, filename)
        if not os.path.isfile(path):
            continue
        with open(path, "r", encoding="utf-8") as handle:
            for raw in handle:
                line = raw.split("#", 1)[0].rstrip()
                if not line.strip():
                    continue
                if line[0] in (" ", "\t") and cur_zone is not None:
                    zones[cur_zone].append(line.split())
                else:
                    cur_zone = None
                    parts = line.split()
                    if parts[0] == "Rule":
                        rules.setdefault(parts[1], []).append(parts[2:])
                    elif parts[0] == "Zone":
                        cur_zone = parts[1]
                        zones.setdefault(cur_zone, []).append(parts[2:])
                    elif parts[0] == "Link":
                        raw_links.append((parts[1], parts[2]))

    entries: list[ZoneEntry] = []
    seen_ids: set[str] = set()
    name_to_id: dict[str, str] = {}

    if "UTC" not in zones and not any(alias == "UTC" for _, alias in raw_links):
        entries.append(
            ZoneEntry(
                enum_id="UTC",
                iana_name="UTC",
                std_offset_min=0,
                dst_savings_min=0,
                start=DEFAULT_TRANSITION,
                end=DEFAULT_TRANSITION,
            )
        )
        seen_ids.add("UTC")
        name_to_id["UTC"] = "UTC"

    for zone_name, rows in sorted(zones.items()):
        last = rows[-1]
        std_offset = parse_offset_minutes(last[0])
        rule_ref = last[1]
        dst_savings = 0
        start_tr = DEFAULT_TRANSITION
        end_tr = DEFAULT_TRANSITION

        if rule_ref in rules:
            active = [r for r in rules[rule_ref] if r[1] == "max"]
            dst_rows = [r for r in active if parse_offset_minutes(r[6]) != 0]
            std_rows = [r for r in active if parse_offset_minutes(r[6]) == 0]
            if dst_rows and std_rows:
                dst_row = dst_rows[0]
                std_row = std_rows[0]
                dst_savings = parse_offset_minutes(dst_row[6])
                s_kind, s_day, s_wd = parse_on(dst_row[4])
                s_hr, s_min, s_base = parse_at(dst_row[5])
                start_tr = Transition(
                    month=parse_month(dst_row[3]),
                    anchor_kind=s_kind,
                    anchor_day=s_day,
                    weekday=s_wd,
                    hour=s_hr,
                    minute=s_min,
                    time_base=s_base,
                )
                e_kind, e_day, e_wd = parse_on(std_row[4])
                e_hr, e_min, e_base = parse_at(std_row[5])
                end_tr = Transition(
                    month=parse_month(std_row[3]),
                    anchor_kind=e_kind,
                    anchor_day=e_day,
                    weekday=e_wd,
                    hour=e_hr,
                    minute=e_min,
                    time_base=e_base,
                )
        elif rule_ref != "-":
            dst_savings = parse_offset_minutes(rule_ref)

        enum_id = sanitize_enum_id(zone_name)
        if enum_id in seen_ids:
            raise ValueError(f"Duplicate enum ID {enum_id} for {zone_name}")
        seen_ids.add(enum_id)
        name_to_id[zone_name] = enum_id

        entries.append(
            ZoneEntry(
                enum_id=enum_id,
                iana_name=zone_name,
                std_offset_min=std_offset,
                dst_savings_min=dst_savings,
                start=start_tr,
                end=end_tr,
            )
        )

    # Resolve transitive links
    link_map = {alias: target for target, alias in raw_links}
    links: list[LinkEntry] = []
    for alias in sorted(link_map):
        if alias in name_to_id:
            continue
        target = link_map[alias]
        while target in link_map and target not in name_to_id:
            target = link_map[target]
        if target in name_to_id:
            links.append(LinkEntry(alias_name=alias, target_enum_id=name_to_id[target]))

    entries.sort(key=lambda e: e.iana_name)
    links.sort(key=lambda l: l.alias_name)
    return version, entries, links


def parse_leapseconds(input_dir: str) -> list[LeapEntry]:
    path = os.path.join(input_dir, "leapseconds")
    leaps: list[LeapEntry] = []
    if not os.path.isfile(path):
        return leaps

    tai_minus_utc = 10
    with open(path, "r", encoding="utf-8") as handle:
        for raw in handle:
            line = raw.split("#", 1)[0].strip()
            if not line:
                continue
            parts = line.split()
            if parts[0] != "Leap":
                continue
            year = int(parts[1])
            month_num = MONTH_NAMES[parts[2][:3].lower()][1]
            day = int(parts[3])
            hms = [int(x) for x in parts[4].split(":")]
            corr = parts[5]
            # The transition takes effect immediately after 23:59:60 (i.e. +1s from 23:59:59)
            base_ts = calendar.timegm((year, month_num, day, hms[0], hms[1], 59)) + 1
            tai_minus_utc += 1 if corr == "+" else -1
            leaps.append(LeapEntry(unix_seconds=base_ts, tai_minus_utc=tai_minus_utc))
    return leaps


def parse_countries_and_zones(
    input_dir: str, entries: list[ZoneEntry], links: list[LinkEntry]
) -> tuple[list[CountryEntry], list[CountryZoneEntry]]:
    name_to_id = {e.iana_name: e.enum_id for e in entries}
    for link in links:
        name_to_id.setdefault(link.alias_name, link.target_enum_id)

    countries: list[CountryEntry] = []
    iso_path = os.path.join(input_dir, "iso3166.tab")
    if os.path.isfile(iso_path):
        with open(iso_path, "r", encoding="utf-8") as handle:
            for raw in handle:
                if raw.startswith("#") or not raw.strip():
                    continue
                parts = raw.rstrip("\r\n").split("\t", 1)
                if len(parts) == 2:
                    countries.append(CountryEntry(code=parts[0].strip(), name=parts[1].strip()))
    countries.sort(key=lambda c: c.code)

    country_zones: list[CountryZoneEntry] = []
    seen_pairs: set[tuple[str, str]] = set()
    z1970_path = os.path.join(input_dir, "zone1970.tab")
    if os.path.isfile(z1970_path):
        with open(z1970_path, "r", encoding="utf-8") as handle:
            for raw in handle:
                if raw.startswith("#") or not raw.strip():
                    continue
                parts = raw.rstrip("\r\n").split("\t")
                if len(parts) < 3:
                    continue
                codes = [c.strip() for c in parts[0].split(",") if c.strip()]
                zone_name = parts[2].strip()
                if zone_name not in name_to_id:
                    continue
                zone_id = name_to_id[zone_name]
                for code in codes:
                    key = (code, zone_id)
                    if key not in seen_pairs:
                        seen_pairs.add(key)
                        country_zones.append(CountryZoneEntry(code=code, zone_enum_id=zone_id))
    country_zones.sort(key=lambda cz: cz.code)
    return countries, country_zones


def format_zones_inc(
    version: str,
    entries: list[ZoneEntry],
    links: list[LinkEntry],
    countries: list[CountryEntry],
    country_zones: list[CountryZoneEntry],
) -> str:
    lines = [
        "#pragma once",
        "",
        f"/* Auto-generated from IANA tzdata ({version}) by .github/scripts/generate-tzdata.py. */",
        "/* Sorted lexicographically by name/code for binary search. */",
        "",
        f'#define CHRONO_TIMEZONE_TZDATA_VERSION "{version}"',
        "",
        "#define CHRONO_TIMEZONE_IANA_TABLE(X) \\",
    ]
    for i, e in enumerate(entries):
        suffix = " \\" if i + 1 < len(entries) else ""
        row = (
            f"  X(({e.enum_id}, \"{e.iana_name}\", {e.std_offset_min}, {e.dst_savings_min}, "
            f"{e.start.month}, {e.start.anchor_kind}, {e.start.anchor_day}, "
            f"{e.start.weekday}, {e.start.hour}, {e.start.minute}, {e.start.time_base}, "
            f"{e.end.month}, {e.end.anchor_kind}, {e.end.anchor_day}, "
            f"{e.end.weekday}, {e.end.hour}, {e.end.minute}, {e.end.time_base})){suffix}"
        )
        lines.append(row)

    lines.append("")
    lines.append("#define CHRONO_TIMEZONE_LINK_TABLE(X) \\")
    for i, link in enumerate(links):
        suffix = " \\" if i + 1 < len(links) else ""
        lines.append(f'  X(("{link.alias_name}", {link.target_enum_id})){suffix}')

    lines.append("")
    lines.append("#define CHRONO_TIMEZONE_COUNTRY_TABLE(X) \\")
    for i, c in enumerate(countries):
        suffix = " \\" if i + 1 < len(countries) else ""
        escaped_name = c.name.replace("\\", "\\\\").replace('"', '\\"')
        lines.append(f'  X(("{c.code}", "{escaped_name}")){suffix}')

    lines.append("")
    lines.append("#define CHRONO_TIMEZONE_COUNTRY_ZONE_TABLE(X) \\")
    for i, cz in enumerate(country_zones):
        suffix = " \\" if i + 1 < len(country_zones) else ""
        lines.append(f'  X(("{cz.code}", {cz.zone_enum_id})){suffix}')

    lines.append("")
    lines.append("/* EOF */")
    lines.append("")
    return "\n".join(lines)


def format_leapseconds_inc(version: str, leaps: list[LeapEntry]) -> str:
    lines = [
        "#pragma once",
        "",
        f"/* Auto-generated from IANA tzdata ({version}) leapseconds by .github/scripts/generate-tzdata.py. */",
        "",
        "#define CHRONO_CLOCK_LEAP_SECOND_TABLE(X) \\",
    ]
    for i, leap in enumerate(leaps):
        suffix = " \\" if i + 1 < len(leaps) else ""
        lines.append(f"  X(({leap.unix_seconds}LL, {leap.tai_minus_utc})){suffix}")
    lines.append("")
    lines.append("/* EOF */")
    lines.append("")
    return "\n".join(lines)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--input",
        default="Codespace/Cosmeron/Asset/TimeZone",
        help="Path to tzdata text directory",
    )
    parser.add_argument(
        "--zones-output",
        default="Codespace/Cosmeron/Modules/Chrono/Timezone/Impl/IanaZones.inc",
        help="Output .inc file path for zones and links",
    )
    parser.add_argument(
        "--leaps-output",
        default="Codespace/Cosmeron/Modules/Chrono/Clock/Impl/LeapSeconds.inc",
        help="Output .inc file path for leap seconds",
    )
    args = parser.parse_args()

    version, entries, links = parse_tzdata(args.input)
    countries, country_zones = parse_countries_and_zones(args.input, entries, links)
    with open(args.zones_output, "w", encoding="utf-8") as handle:
        handle.write(
            format_zones_inc(version, entries, links, countries, country_zones)
        )

    leaps = parse_leapseconds(args.input)
    with open(args.leaps_output, "w", encoding="utf-8") as handle:
        handle.write(format_leapseconds_inc(version, leaps))

    print(
        f"Generated tzdata {version}: {len(entries)} zones, {len(links)} links, "
        f"{len(countries)} countries, {len(country_zones)} country-zone mappings -> {args.zones_output}; "
        f"{len(leaps)} leap seconds -> {args.leaps_output}"
    )


if __name__ == "__main__":
    main()

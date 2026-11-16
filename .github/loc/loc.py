#!/usr/bin/env python3
"""Lines of Code (LOC) counter and SVG badge generator for Cosmeron."""

import argparse
import json
from pathlib import Path

EXTENSIONS = {".h", ".c", ".def", ".inc", ".impl"}


def count_lines(scope_dir: Path) -> dict:
    total_lines = 0
    code_lines = 0
    file_count = 0

    if scope_dir.exists():
        for path in sorted(scope_dir.rglob("*")):
            if path.is_file() and path.suffix in EXTENSIONS:
                file_count += 1
                try:
                    lines = path.read_text(encoding="utf-8", errors="replace").splitlines()
                except OSError:
                    continue
                total_lines += len(lines)
                code_lines += sum(1 for line in lines if line.strip())

    return {
        "scope": str(scope_dir),
        "files": file_count,
        "total_lines": total_lines,
        "code_lines": code_lines,
    }


def format_count(n: int) -> str:
    if n >= 1_000_000:
        return f"{n / 1_000_000:.1f}M"
    if n >= 1_000:
        return f"{n / 1_000:.1f}k"
    return str(n)


def render_badge(label: str, value: str, color: str = "#007ec6") -> str:
    label_w = max(40, len(label) * 7 + 12)
    value_w = max(40, len(value) * 7 + 12)
    total_w = label_w + value_w
    label_x = label_w / 2
    value_x = label_w + value_w / 2
    return f"""<svg xmlns="http://www.w3.org/2000/svg" width="{total_w}" height="20" role="img" aria-label="{label}: {value}">
  <linearGradient id="s" x2="0" y2="100%">
    <stop offset="0" stop-color="#bbb" stop-opacity=".1"/>
    <stop offset="1" stop-opacity=".1"/>
  </linearGradient>
  <clipPath id="r">
    <rect width="{total_w}" height="20" rx="3" fill="#fff"/>
  </clipPath>
  <g clip-path="url(#r)">
    <rect width="{label_w}" height="20" fill="#555"/>
    <rect x="{label_w}" width="{value_w}" height="20" fill="{color}"/>
    <rect width="{total_w}" height="20" fill="url(#s)"/>
  </g>
  <g fill="#fff" text-anchor="middle" font-family="Verdana,Geneva,DejaVu Sans,sans-serif" text-rendering="geometricPrecision" font-size="11">
    <text x="{label_x:.1f}" y="14">{label}</text>
    <text x="{value_x:.1f}" y="14">{value}</text>
  </g>
</svg>
"""


def main() -> int:
    parser = argparse.ArgumentParser(description="Count Cosmeron lines of code and generate SVG badge.")
    parser.add_argument("--scope", default="Codespace/Cosmeron", help="Directory to scan")
    parser.add_argument("--state", default=".github/loc/state.json", help="Output state JSON path")
    parser.add_argument("--badge", default=".github/badges/loc.svg", help="Output SVG badge path")
    args = parser.parse_args()

    stats = count_lines(Path(args.scope))
    state_path = Path(args.state)
    state_path.parent.mkdir(parents=True, exist_ok=True)
    state_path.write_text(json.dumps(stats, indent=2) + "\n", encoding="utf-8")

    badge_path = Path(args.badge)
    badge_path.parent.mkdir(parents=True, exist_ok=True)
    badge_svg = render_badge("lines of code", format_count(stats["code_lines"]))
    badge_path.write_text(badge_svg, encoding="utf-8")

    print(f"LOC: {stats['code_lines']} non-empty lines ({stats['total_lines']} total) across {stats['files']} files.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

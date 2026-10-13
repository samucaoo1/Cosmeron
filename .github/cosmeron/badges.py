#!/usr/bin/env python3
"""Cosmeron Badge Generator.

Renders high-quality SVG badges for documentation and READMEs, matching the
clean GitHub badge style.
"""
from __future__ import annotations

import html
from dataclasses import dataclass
from pathlib import Path


@dataclass(frozen=True)
class BadgeSpec:
    label: str
    message: str
    color: str = "#2ea44f"  # GitHub green
    label_color: str = "#555555"


# Curated palette for consistent Cosmeron badges
PALETTE = {
    "green": "#2ea44f",
    "blue": "#0969da",
    "purple": "#6f42c1",
    "orange": "#bf8700",
    "red": "#cf222e",
    "gray": "#57606a",
    "dark": "#24292f",
}


def estimate_text_width(text: str) -> int:
    """Approximate width in pixels of text rendered at 11px Verdana."""
    width = 0
    for char in text:
        if char in "ijl|!.,:;' ":
            width += 4
        elif char in "frtI":
            width += 5
        elif char in "mwMW@":
            width += 10
        elif char.isupper():
            width += 8
        else:
            width += 7
    return max(width, 0)


def render_badge(spec: BadgeSpec) -> str:
    """Render an SVG badge."""
    pad_x = 7
    label_w = estimate_text_width(spec.label) + (pad_x * 2)
    message_w = estimate_text_width(spec.message) + (pad_x * 2)
    total_w = label_w + message_w
    height = 20

    label_x = label_w / 2
    message_x = label_w + (message_w / 2)

    title = html.escape(f"{spec.label}: {spec.message}")
    escaped_label = html.escape(spec.label)
    escaped_message = html.escape(spec.message)

    return f"""<svg xmlns="http://www.w3.org/2000/svg" width="{total_w}" height="{height}" role="img" aria-label="{title}">
  <title>{title}</title>
  <linearGradient id="s" x2="0" y2="100%">
    <stop offset="0" stop-color="#bbb" stop-opacity=".1"/>
    <stop offset="1" stop-opacity=".1"/>
  </linearGradient>
  <clipPath id="r">
    <rect width="{total_w}" height="{height}" rx="3" fill="#fff"/>
  </clipPath>
  <g clip-path="url(#r)">
    <rect width="{label_w}" height="{height}" fill="{spec.label_color}"/>
    <rect x="{label_w}" width="{message_w}" height="{height}" fill="{spec.color}"/>
    <rect width="{total_w}" height="{height}" fill="url(#s)"/>
  </g>
  <g fill="#fff" text-anchor="middle" font-family="Verdana,DejaVu Sans,sans-serif" font-size="11">
    <text aria-hidden="true" x="{label_x}" y="15" fill="#010101" fill-opacity=".3">{escaped_label}</text>
    <text x="{label_x}" y="14">{escaped_label}</text>
    <text aria-hidden="true" x="{message_x}" y="15" fill="#010101" fill-opacity=".3">{escaped_message}</text>
    <text x="{message_x}" y="14">{escaped_message}</text>
  </g>
</svg>
"""


def generate_standard_badges(output_dir: Path) -> dict[str, Path]:
    """Generate all standard Cosmeron badges into output_dir."""
    output_dir.mkdir(parents=True, exist_ok=True)

    catalog = {
        "c11.svg": BadgeSpec("standard", "C11 (ISO/IEC 9899:2011)", PALETTE["blue"]),
        "license.svg": BadgeSpec("license", "EUPL-1.2", PALETTE["blue"]),
        "architecture.svg": BadgeSpec("architecture", "header-only / modular", PALETTE["purple"]),
        "security.svg": BadgeSpec("security", "CodeQL · ASan · UBSan · Fuzz", PALETTE["green"]),
        "platforms.svg": BadgeSpec("platforms", "Linux · BSD · Windows · macOS", PALETTE["green"]),
        "experimental.svg": BadgeSpec("experimental", "OHOS · Solaris · Android · iOS", PALETTE["orange"]),
        "linking.svg": BadgeSpec("dependencies", "zero extra link", PALETTE["green"]),
    }

    results: dict[str, Path] = {}
    for filename, spec in catalog.items():
        dest = output_dir / filename
        dest.write_text(render_badge(spec), encoding="utf-8")
        results[filename] = dest

    return results


def main() -> int:
    import argparse

    parser = argparse.ArgumentParser(description="Generate Cosmeron SVG badges")
    parser.add_argument("--output-dir", "-o", default=".github/badges", help="Destination directory")
    args = parser.parse_args()

    out = Path(args.output_dir)
    generated = generate_standard_badges(out)
    print(f"Generated {len(generated)} badges in {out}:")
    for name in generated:
        print(f"  - {name}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

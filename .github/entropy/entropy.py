#!/usr/bin/env python3
"""Cosmeron entropy calculator.

Entropy measures accumulated source change, not current code size.

Baseline:
    H0 = every tracked line in Codespace/Cosmeron/** at entropy-origin.

After the baseline:
    delta H = additions + deletions for every first-parent commit.

A rewritten line is represented by Git as one deletion plus one addition and
therefore contributes 2 H.
"""

from __future__ import annotations

import argparse
import html
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable

GREEK = tuple("αβγδεζηθικλμνξοπρστυφχψω")


class EntropyError(RuntimeError):
    pass


@dataclass(frozen=True)
class EntropyState:
    origin_commit: str
    ref_commit: str
    initial_entropy: int
    accumulated_change: int
    total: int
    symbol: str
    unit: str

    @property
    def display(self) -> str:
        return f"{self.total}{self.symbol}"


def run_git(repo: Path, *args: str, text: bool = True) -> str | bytes:
    command = ["git", "-C", str(repo), *args]
    completed = subprocess.run(
        command,
        check=False,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=text,
    )
    if completed.returncode != 0:
        stderr = completed.stderr if text else completed.stderr.decode("utf-8", "replace")
        raise EntropyError(f"{' '.join(command)} failed: {stderr.strip()}")
    return completed.stdout


def ref_exists(repo: Path, ref: str) -> bool:
    completed = subprocess.run(
        ["git", "-C", str(repo), "rev-parse", "--verify", "--quiet", ref],
        check=False,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )
    return completed.returncode == 0


def resolve_ref(repo: Path, ref: str) -> str:
    return str(run_git(repo, "rev-parse", f"{ref}^{{commit}}")).strip()


def line_count_bytes(data: bytes) -> int:
    if not data:
        return 0
    return data.count(b"\n") + (0 if data.endswith(b"\n") else 1)


def tracked_paths_at_ref(repo: Path, ref: str, scope: str) -> list[str]:
    raw = run_git(repo, "ls-tree", "-r", "-z", "--name-only", ref, "--", scope, text=False)
    return [
        part.decode("utf-8", "surrogateescape")
        for part in raw.split(b"\0")
        if part
    ]


def count_lines_at_ref(repo: Path, ref: str, scope: str) -> int:
    total = 0
    for path in tracked_paths_at_ref(repo, ref, scope):
        data = run_git(repo, "show", f"{ref}:{path}", text=False)
        total += line_count_bytes(data)
    return total


def first_parent_commits(repo: Path, origin: str, ref: str) -> list[str]:
    ancestry = subprocess.run(
        ["git", "-C", str(repo), "merge-base", "--is-ancestor", origin, ref],
        check=False,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )
    if ancestry.returncode != 0:
        raise EntropyError(f"{origin} is not an ancestor of {ref}")

    output = str(
        run_git(repo, "rev-list", "--first-parent", "--reverse", f"{origin}..{ref}")
    )
    return [line for line in output.splitlines() if line]


def numstat_delta(repo: Path, parent: str, commit: str, scope: str) -> int:
    output = str(
        run_git(
            repo,
            "diff",
            "--numstat",
            "--find-renames",
            parent,
            commit,
            "--",
            scope,
        )
    )
    delta = 0
    for line in output.splitlines():
        if not line:
            continue
        fields = line.split("\t", 2)
        if len(fields) < 3:
            raise EntropyError(f"unexpected numstat line: {line!r}")
        added, deleted = fields[0], fields[1]
        if added == "-" or deleted == "-":
            raise EntropyError(
                f"binary file found inside entropy scope: {fields[2]}"
            )
        delta += int(added) + int(deleted)
    return delta


def accumulated_change(repo: Path, origin: str, ref: str, scope: str) -> int:
    total = 0
    for commit in first_parent_commits(repo, origin, ref):
        parent = str(run_git(repo, "rev-parse", f"{commit}^1")).strip()
        total += numstat_delta(repo, parent, commit, scope)
    return total


def greek_symbol(total: int, block_size: int) -> str:
    if total < 0:
        raise ValueError("entropy cannot be negative")
    if block_size <= 0:
        raise ValueError("block_size must be positive")

    # Excel-style bijective base-24:
    # 0 -> α, 23 -> ω, 24 -> αα, 25 -> αβ, ...
    value = total // block_size + 1
    result = ""
    while value:
        value, remainder = divmod(value - 1, len(GREEK))
        result = GREEK[remainder] + result
    return result


def calculate(
    repo: Path,
    origin_ref: str,
    ref: str,
    scope: str,
    block_size: int,
    unit: str,
) -> EntropyState:
    origin = resolve_ref(repo, origin_ref)
    target = resolve_ref(repo, ref)
    initial = count_lines_at_ref(repo, origin, scope)
    delta = accumulated_change(repo, origin, target, scope)
    total = initial + delta
    return EntropyState(
        origin_commit=origin,
        ref_commit=target,
        initial_entropy=initial,
        accumulated_change=delta,
        total=total,
        symbol=greek_symbol(total, block_size),
        unit=unit,
    )


def provisional_state(
    repo: Path, ref: str, scope: str, block_size: int, unit: str
) -> EntropyState:
    target = resolve_ref(repo, ref)
    total = count_lines_at_ref(repo, target, scope)
    return EntropyState(
        origin_commit=target,
        ref_commit=target,
        initial_entropy=total,
        accumulated_change=0,
        total=total,
        symbol=greek_symbol(total, block_size),
        unit=unit,
    )


def render_svg(state: EntropyState) -> str:
    left = "Entropy"
    right = state.display
    left_width = 62
    right_width = max(64, 12 + len(str(state.total)) * 8 + len(state.symbol) * 10)
    width = left_width + right_width
    left_x = left_width / 2
    right_x = left_width + right_width / 2
    title = html.escape(f"Entropy: {state.display} ({state.total} {state.unit})")
    return f"""<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="20" role="img" aria-label="{title}">
<title>{title}</title>
<linearGradient id="s" x2="0" y2="100%">
  <stop offset="0" stop-color="#fff" stop-opacity=".7"/>
  <stop offset=".1" stop-color="#aaa" stop-opacity=".1"/>
  <stop offset=".9" stop-opacity=".3"/>
  <stop offset="1" stop-opacity=".5"/>
</linearGradient>
<clipPath id="r"><rect width="{width}" height="20" rx="3"/></clipPath>
<g clip-path="url(#r)">
  <rect width="{left_width}" height="20" fill="#555"/>
  <rect x="{left_width}" width="{right_width}" height="20" fill="#6f42c1"/>
  <rect width="{width}" height="20" fill="url(#s)"/>
</g>
<g fill="#fff" text-anchor="middle" font-family="Verdana,DejaVu Sans,sans-serif" font-size="11">
  <text x="{left_x}" y="15" fill="#010101" fill-opacity=".3">{left}</text>
  <text x="{left_x}" y="14">{left}</text>
  <text x="{right_x}" y="15" fill="#010101" fill-opacity=".3">{html.escape(right)}</text>
  <text x="{right_x}" y="14">{html.escape(right)}</text>
</g>
</svg>
"""


def state_json(state: EntropyState, provisional: bool) -> dict[str, object]:
    return {
        "display": state.display,
        "total": state.total,
        "symbol": state.symbol,
        "unit": state.unit,
        "origin_commit": state.origin_commit,
        "ref_commit": state.ref_commit,
        "initial_entropy": state.initial_entropy,
        "accumulated_change": state.accumulated_change,
        "provisional": provisional,
    }


def load_config(path: Path) -> dict[str, object]:
    with path.open("r", encoding="utf-8") as stream:
        return json.load(stream)


def main(argv: Iterable[str] | None = None) -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--repo", default=".")
    parser.add_argument("--ref", default="HEAD")
    parser.add_argument("--config", default=".github/entropy/config.json")
    parser.add_argument("--output-dir")
    parser.add_argument(
        "--provisional",
        action="store_true",
        help="use the selected ref itself as the baseline if entropy-origin is absent",
    )
    args = parser.parse_args(argv)

    repo = Path(args.repo).resolve()
    config = load_config(repo / args.config)
    scope = str(config["scope"])
    origin_tag = str(config["origin_tag"])
    block_size = int(config["block_size"])
    unit = str(config["unit"])

    provisional = False
    if ref_exists(repo, origin_tag):
        state = calculate(repo, origin_tag, args.ref, scope, block_size, unit)
    elif args.provisional:
        provisional = True
        state = provisional_state(repo, args.ref, scope, block_size, unit)
    else:
        raise EntropyError(
            f"{origin_tag!r} does not exist; create the immutable origin tag first"
        )

    print(f"Entropy: {state.display}")
    print(f"Initial entropy: {state.initial_entropy} {state.unit}")
    print(f"Accumulated change: +{state.accumulated_change} {state.unit}")

    if args.output_dir:
        output = Path(args.output_dir)
        output.mkdir(parents=True, exist_ok=True)
        (output / "entropy.svg").write_text(render_svg(state), encoding="utf-8")
        (output / "state.json").write_text(
            json.dumps(state_json(state, provisional), indent=2, ensure_ascii=False)
            + "\n",
            encoding="utf-8",
        )

    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except EntropyError as error:
        print(f"entropy: {error}", file=sys.stderr)
        raise SystemExit(2)

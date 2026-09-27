#!/usr/bin/env python3
"""Compile every public Cosmeron header as the first and only project include."""

from __future__ import annotations

import argparse
import os
import subprocess
import sys
import tempfile
from pathlib import Path


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--compiler", default=os.environ.get("CC", "cc"))
    parser.add_argument("--root", default=".")
    args = parser.parse_args()

    root = Path(args.root).resolve()
    headers = sorted((root / "Codespace/Cosmeron").rglob("*.h"))
    if not headers:
        print("No public headers found.", file=sys.stderr)
        return 2

    failures: list[tuple[str, str]] = []
    with tempfile.TemporaryDirectory() as directory:
        scratch = Path(directory)
        for index, header in enumerate(headers):
            relative = header.relative_to(root).as_posix()
            source = scratch / f"header_{index}.c"
            source.write_text(
                f'#include "{relative}"\nint main(void) {{ return 0; }}\n',
                encoding="utf-8",
            )
            command = [
                args.compiler,
                "-std=c11",
                "-Wall",
                "-Wextra",
                "-Wpedantic",
                "-Werror",
                "-D_POSIX_C_SOURCE=200809L",
                "-I",
                str(root),
                "-fsyntax-only",
                str(source),
            ]
            completed = subprocess.run(
                command,
                cwd=root,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                text=True,
                check=False,
            )
            if completed.returncode != 0:
                failures.append((relative, completed.stdout))
            else:
                print(f"[header] {relative}")

    if failures:
        print(f"\n{len(failures)} header(s) failed standalone compilation:", file=sys.stderr)
        for header, output in failures:
            print(f"\n--- {header} ---\n{output}", file=sys.stderr)
        return 1

    print(f"\n{len(headers)} public headers compile standalone with {args.compiler}.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

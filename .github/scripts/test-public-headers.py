#!/usr/bin/env python3
"""Compile every public Cosmeron header as the first and only project include."""

from __future__ import annotations

import argparse
import concurrent.futures
import os
import subprocess
import sys
import tempfile
from pathlib import Path


def check_header(index: int, header: Path, root: Path, scratch: Path, compiler: str) -> tuple[str, str] | None:
    relative = header.relative_to(root).as_posix()
    source = scratch / f"header_{index}.c"
    source.write_text(
        f'#include "{relative}"\nint main(void) {{ return 0; }}\n',
        encoding="utf-8",
    )
    command = [
        compiler,
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
    completed = subprocess.run(command, text=True, capture_output=True)
    if completed.returncode != 0:
        return (relative, completed.stderr or completed.stdout)
    return None


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--compiler", default=os.environ.get("CC", "cc"))
    parser.add_argument("--root", default=".")
    parser.add_argument("--jobs", type=int, default=os.cpu_count() or 4)
    args = parser.parse_args()

    root = Path(args.root).resolve()
    headers = sorted(
        h for h in (root / "Codespace/Cosmeron").rglob("*.h")
        if h.relative_to(root).as_posix() != "Codespace/Cosmeron/Modules/Container/Container.h"
    )
    if not headers:
        print("No public headers found.", file=sys.stderr)
        return 2

    failures: list[tuple[str, str]] = []
    with tempfile.TemporaryDirectory() as directory:
        scratch = Path(directory)
        with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as pool:
            futures = [
                pool.submit(check_header, idx, hdr, root, scratch, args.compiler)
                for idx, hdr in enumerate(headers)
            ]
            for fut in concurrent.futures.as_completed(futures):
                res = fut.result()
                if res is not None:
                    failures.append(res)

    if failures:
        for relative, output in sorted(failures):
            print(f"FAIL: {relative}\n{output}", file=sys.stderr)
        return 1

    print(f"PASS: compiled {len(headers)} public headers with {args.compiler}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

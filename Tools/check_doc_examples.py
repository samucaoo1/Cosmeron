#!/usr/bin/env python3
"""Compile and run *opted-in* standalone C11 examples from Docs/Modules.

Opt in by placing the exact marker below immediately before a fenced C block:
    <!-- cosmeron-c11-test -->
    ```c
    ... complete translation unit with main ...
    ```

Most API-reference examples are illustrative *fragments*. They must not be
silently treated as standalone programs by this checker.
"""
from __future__ import annotations

import argparse
import pathlib
import re
import shlex
import subprocess
import sys
import tempfile

TEST_MARKER = "<!-- cosmeron-c11-test -->"
TEST_BLOCK = re.compile(
    r"<!-- cosmeron-c11-test -->\s*```c\s*\n(.*?)\n```",
    re.DOTALL,
)


def collect_examples(folder: pathlib.Path) -> list[tuple[pathlib.Path, int, str]]:
    examples = []
    for markdown in sorted(folder.glob("*.md")):
        content = markdown.read_text(encoding="utf-8")
        matches = list(TEST_BLOCK.finditer(content))
        if content.count(TEST_MARKER) != len(matches):
            raise ValueError(
                f"{markdown}: marker without a following fenced C source block"
            )
        for index, match in enumerate(matches, start=1):
            if not re.search(r"\bint\s+main\s*\(", match.group(1)):
                raise ValueError(f"{markdown}: example {index} must define int main(...)")
            examples.append((markdown, index, match.group(1)))
    return examples


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cc", default="cc", help="C compiler (default: cc)")
    parser.add_argument("--min-tests", type=int, default=1)
    parser.add_argument(
        "--root",
        type=pathlib.Path,
        default=pathlib.Path(__file__).resolve().parents[1],
        help="Cosmeron repository root",
    )
    args = parser.parse_args()
    root = args.root.resolve()
    try:
        examples = collect_examples(root / "Docs" / "Modules")
    except (OSError, ValueError) as error:
        print(f"Documentation error: {error}", file=sys.stderr)
        return 1

    if len(examples) < args.min_tests:
        print(
            f"Only {len(examples)} tagged documentation examples found "
            f"(minimum {args.min_tests})",
            file=sys.stderr,
        )
        return 1

    compiler = shlex.split(args.cc)
    if not compiler:
        parser.error("--cc must specify a compiler")
    with tempfile.TemporaryDirectory(prefix="cosmeron-doc-test-") as work:
        temp = pathlib.Path(work)
        for number, (markdown, index, code) in enumerate(examples):
            source = temp / f"example-{number}.c"
            target = temp / f"example-{number}"
            source.write_text(code + "\n", encoding="utf-8")
            title = f"{markdown.relative_to(root)} (example {index})"
            command = compiler + [
                "-std=c11",
                "-Wall",
                "-Wextra",
                "-Wpedantic",
                "-Werror",
                "-D_POSIX_C_SOURCE=200809L",
                "-I",
                str(root / "Codespace"),
                str(source),
                "-o",
                str(target),
            ]
            print(f"Compiling {title}", flush=True)
            build = subprocess.run(command, cwd=root, check=False)
            if build.returncode != 0:
                return build.returncode
            print(f"Running   {title}", flush=True)
            result = subprocess.run([str(target)], cwd=root, check=False)
            if result.returncode != 0:
                return result.returncode

    print(f"OK: {len(examples)} C11 documentation examples compiled and ran.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

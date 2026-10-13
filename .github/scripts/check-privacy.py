#!/usr/bin/env python3
"""Reject developer-specific local paths in tracked files.

The repository is public (AGENTS.md, section 10): versioned files must not
expose absolute machine paths, home directories, removable-media mounts or
local ``file:///`` links. Vendored upstream sources are excluded because they
contain upstream documentation examples, not Cosmeron developer paths.

A line can opt out explicitly with the marker ``privacy-scan: allow``.
"""
from __future__ import annotations

import re
import subprocess
import sys
from pathlib import Path

EXCLUDED_PREFIXES = (
    # Vendored SQLite amalgamation: upstream docs use /home/fred-style examples.
    "Codespace/Cosmeron/Modules/Database/SQLite/Internal/",
)
SELF_PATH = ".github/scripts/check-privacy.py"
ALLOW_MARKER = "privacy-scan: allow"
BINARY_PROBE_BYTES = 8192

PATTERNS = (
    ("removable-media mount", re.compile(r"/run/media/[^/\s]+/")),
    ("Linux home directory", re.compile(r"/home/[^/\s\"'<>`]+/")),
    ("macOS home directory", re.compile(r"/Users/[^/\s\"'<>`]+/")),
    ("Windows user profile", re.compile(r"[A-Za-z]:[\\/]+Users[\\/]+", re.I)),
    ("local file:/// link", re.compile(r"file:///")),
)


def tracked_files(repo: Path) -> list[str]:
    output = subprocess.run(
        ["git", "-C", str(repo), "ls-files", "-z"],
        check=True,
        stdout=subprocess.PIPE,
    ).stdout
    return [name for name in output.decode("utf-8").split("\0") if name]


def is_binary(data: bytes) -> bool:
    return b"\0" in data[:BINARY_PROBE_BYTES]


def scan(repo: Path) -> list[str]:
    findings: list[str] = []
    for name in tracked_files(repo):
        if name == SELF_PATH or name.startswith(EXCLUDED_PREFIXES):
            continue
        path = repo / name
        if not path.is_file():
            continue
        data = path.read_bytes()
        if is_binary(data):
            continue
        text = data.decode("utf-8", errors="replace")
        for number, line in enumerate(text.splitlines(), start=1):
            if ALLOW_MARKER in line:
                continue
            for description, pattern in PATTERNS:
                if pattern.search(line):
                    findings.append(f"{name}:{number}: {description}")
    return findings


def main() -> int:
    repo = Path(sys.argv[1] if len(sys.argv) > 1 else ".").resolve()
    findings = scan(repo)
    for finding in findings:
        file_name, line_number, description = finding.split(":", 2)
        print(f"::error file={file_name},line={line_number}::{description.strip()}")
    if findings:
        print(f"{len(findings)} local-path leak(s) found.", file=sys.stderr)
        return 1
    print("No local machine paths found in tracked files.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

#!/usr/bin/env python3
"""Summarize GitHub Actions 'needs' without silently accepting skipped jobs."""
import json
import os
import sys

results = json.loads(os.environ["NEEDS_JSON"])
title = os.environ.get("REPORT_TITLE", "Cosmeron CI")
icons = {"success": "✅", "failure": "❌", "cancelled": "⏹️", "skipped": "➖"}
lines = [f"## Cosmeron — {title}", "", "| Job | Outcome |", "|---|---|"]
failed = []
for name, details in sorted(results.items()):
    state = details.get("result", "unknown")
    lines.append(f"| `{name}` | {icons.get(state, '❓')} {state} |")
    if state != "success":
        failed.append(f"{name}={state}")
lines += ["", f"**{len(results) - len(failed)}/{len(results)} jobs successful.**", ""]
report = "\n".join(lines)
print(report)
summary_path = os.environ.get("GITHUB_STEP_SUMMARY")
if summary_path:
    with open(summary_path, "a", encoding="utf-8") as stream:
        stream.write(report + "\n")
if failed:
    print("Non-successful checks: " + ", ".join(failed), file=sys.stderr)
    sys.exit(1)

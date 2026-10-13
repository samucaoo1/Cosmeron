#!/usr/bin/env python3
"""Summarize GitHub Actions 'needs' without silently accepting skipped jobs.

Environment:
  NEEDS_JSON     ``toJSON(needs)`` from the gate job (required).
  REPORT_TITLE   heading for the job summary.
  ALLOW_SKIPPED  comma-separated job ids that may be ``skipped`` because they
                 are scheduled/opt-in only; their failures still count.
  REPORT_STRICT  ``0`` reports outcomes without failing (non-blocking
                 workflows such as Experimental). Defaults to ``1``.
"""
import json
import os
import sys

results = json.loads(os.environ["NEEDS_JSON"])
title = os.environ.get("REPORT_TITLE", "Cosmeron CI")
allow_skipped = {
    name.strip()
    for name in os.environ.get("ALLOW_SKIPPED", "").split(",")
    if name.strip()
}
strict = os.environ.get("REPORT_STRICT", "1") != "0"
icons = {"success": "✅", "failure": "❌", "cancelled": "⏹️", "skipped": "➖"}
lines = [f"## Cosmeron — {title}", "", "| Job | Outcome |", "|---|---|"]
failed = []
for name, details in sorted(results.items()):
    state = details.get("result", "unknown")
    note = " (opt-in)" if name in allow_skipped and state == "skipped" else ""
    lines.append(f"| `{name}` | {icons.get(state, '❓')} {state}{note} |")
    if state == "success" or (state == "skipped" and name in allow_skipped):
        continue
    failed.append(f"{name}={state}")
lines += ["", f"**{len(results) - len(failed)}/{len(results)} jobs accepted.**", ""]
if not strict:
    lines += ["_Non-blocking report: outcomes are informational, not merge requirements._", ""]
report = "\n".join(lines)
print(report)
summary_path = os.environ.get("GITHUB_STEP_SUMMARY")
if summary_path:
    with open(summary_path, "a", encoding="utf-8") as stream:
        stream.write(report + "\n")
if failed:
    print("Non-successful checks: " + ", ".join(failed), file=sys.stderr)
    if strict:
        sys.exit(1)

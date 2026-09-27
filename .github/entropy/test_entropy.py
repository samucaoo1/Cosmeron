#!/usr/bin/env python3
from __future__ import annotations

import importlib.util
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

MODULE_PATH = Path(__file__).with_name("entropy.py")
SPEC = importlib.util.spec_from_file_location("cosmeron_entropy", MODULE_PATH)
entropy = importlib.util.module_from_spec(SPEC)
assert SPEC.loader is not None
sys.modules[SPEC.name] = entropy
SPEC.loader.exec_module(entropy)


def git(repo: Path, *args: str) -> None:
    subprocess.run(
        ["git", "-C", str(repo), *args],
        check=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )


def commit_all(repo: Path, message: str) -> None:
    git(repo, "add", "-A")
    git(repo, "commit", "-m", message)


class EntropyFormatTests(unittest.TestCase):
    def test_greek_excel_rollover(self) -> None:
        self.assertEqual(entropy.greek_symbol(0, 10000), "α")
        self.assertEqual(entropy.greek_symbol(9999, 10000), "α")
        self.assertEqual(entropy.greek_symbol(10000, 10000), "β")
        self.assertEqual(entropy.greek_symbol(230000, 10000), "ω")
        self.assertEqual(entropy.greek_symbol(240000, 10000), "αα")
        self.assertEqual(entropy.greek_symbol(250000, 10000), "αβ")

    def test_requested_display_format(self) -> None:
        state = entropy.EntropyState("a", "b", 18537, 0, 18537, "β", "H")
        self.assertEqual(state.display, "18537β")

    def test_line_without_final_newline_counts(self) -> None:
        self.assertEqual(entropy.line_count_bytes(b"a\nb"), 2)
        self.assertEqual(entropy.line_count_bytes(b"a\nb\n"), 2)
        self.assertEqual(entropy.line_count_bytes(b""), 0)

    def test_render_and_state_json(self) -> None:
        state = entropy.EntropyState("a", "b", 18537, 12, 18549, "β", "H")
        svg = entropy.render_svg(state)
        payload = entropy.state_json(state, provisional=False)
        self.assertIn("Entropy", svg)
        self.assertIn("18549β", svg)
        self.assertEqual(payload["display"], "18549β")
        self.assertEqual(payload["accumulated_change"], 12)
        self.assertFalse(payload["provisional"])


class EntropyGitHistoryTests(unittest.TestCase):
    def setUp(self) -> None:
        self.temp = tempfile.TemporaryDirectory()
        self.repo = Path(self.temp.name)
        git(self.repo, "init", "-b", "main")
        git(self.repo, "config", "user.name", "Entropy Test")
        git(self.repo, "config", "user.email", "entropy@example.invalid")
        (self.repo / "Codespace/Cosmeron").mkdir(parents=True)
        (self.repo / "docs").mkdir()
        (self.repo / "Codespace/Cosmeron/base.h").write_text(
            "one\ntwo\nthree\n", encoding="utf-8"
        )
        commit_all(self.repo, "baseline")
        git(self.repo, "tag", "entropy-origin")

    def tearDown(self) -> None:
        self.temp.cleanup()

    def state(self):
        return entropy.calculate(
            self.repo, "entropy-origin", "HEAD", "Codespace/Cosmeron", 10000, "H"
        )

    def test_baseline_counts_existing_lines_once(self) -> None:
        state = self.state()
        self.assertEqual(state.initial_entropy, 3)
        self.assertEqual(state.accumulated_change, 0)
        self.assertEqual(state.total, 3)

    def test_add_delete_and_rewrite(self) -> None:
        path = self.repo / "Codespace/Cosmeron/base.h"
        path.write_text("one\ntwo\nthree\nfour\n", encoding="utf-8")
        commit_all(self.repo, "add one")
        self.assertEqual(self.state().total, 4)

        path.write_text("one\nTWO\nthree\nfour\n", encoding="utf-8")
        commit_all(self.repo, "rewrite one")
        self.assertEqual(self.state().total, 6)

        path.write_text("one\nTWO\nthree\n", encoding="utf-8")
        commit_all(self.repo, "delete one")
        self.assertEqual(self.state().total, 7)

    def test_out_of_scope_change_is_zero(self) -> None:
        (self.repo / "docs/readme.md").write_text("ignored\n", encoding="utf-8")
        commit_all(self.repo, "docs only")
        self.assertEqual(self.state().total, 3)

    def test_new_file_and_deleted_file(self) -> None:
        extra = self.repo / "Codespace/Cosmeron/extra.h"
        extra.write_text("a\nb\n", encoding="utf-8")
        commit_all(self.repo, "new file")
        self.assertEqual(self.state().total, 5)

        extra.unlink()
        commit_all(self.repo, "delete file")
        self.assertEqual(self.state().total, 7)

    def test_pure_rename_is_zero(self) -> None:
        git(
            self.repo,
            "mv",
            "Codespace/Cosmeron/base.h",
            "Codespace/Cosmeron/renamed.h",
        )
        commit_all(self.repo, "rename")
        self.assertEqual(self.state().total, 3)

    def test_change_then_revert_is_still_entropy(self) -> None:
        path = self.repo / "Codespace/Cosmeron/base.h"
        path.write_text("one\nTWO\nthree\n", encoding="utf-8")
        commit_all(self.repo, "change")
        path.write_text("one\ntwo\nthree\n", encoding="utf-8")
        commit_all(self.repo, "revert")
        self.assertEqual(self.state().accumulated_change, 4)
        self.assertEqual(self.state().total, 7)

    def test_provisional_cli_writes_outputs(self) -> None:
        config = self.repo / "entropy-config.json"
        config.write_text(
            '{"scope":"Codespace/Cosmeron","origin_tag":"missing-origin",'
            '"block_size":10000,"unit":"H"}\n',
            encoding="utf-8",
        )
        output = self.repo / "out"
        result = entropy.main(
            [
                "--repo",
                str(self.repo),
                "--config",
                "entropy-config.json",
                "--provisional",
                "--output-dir",
                str(output),
            ]
        )
        self.assertEqual(result, 0)
        state = entropy.load_config(config)
        self.assertEqual(state["scope"], "Codespace/Cosmeron")
        payload = (output / "state.json").read_text(encoding="utf-8")
        badge = (output / "entropy.svg").read_text(encoding="utf-8")
        self.assertIn('"provisional": true', payload)
        self.assertIn("Entropy", badge)


if __name__ == "__main__":
    unittest.main()

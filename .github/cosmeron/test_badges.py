#!/usr/bin/env python3
"""Unit tests for Cosmeron badge generation."""
from __future__ import annotations

import tempfile
import unittest
from pathlib import Path

from badges import BadgeSpec, estimate_text_width, generate_standard_badges, render_badge


class TestBadges(unittest.TestCase):
    def test_estimate_text_width(self) -> None:
        self.assertGreater(estimate_text_width("Hello World"), 40)
        self.assertGreater(estimate_text_width("W"), estimate_text_width("i"))

    def test_render_badge(self) -> None:
        spec = BadgeSpec("test", "passing", "#2ea44f")
        svg = render_badge(spec)
        self.assertIn("<svg", svg)
        self.assertIn("test: passing", svg)
        self.assertIn("#2ea44f", svg)

    def test_generate_standard_badges(self) -> None:
        with tempfile.TemporaryDirectory() as tmpdir:
            out = Path(tmpdir)
            badges = generate_standard_badges(out)
            self.assertIn("c11.svg", badges)
            self.assertIn("license.svg", badges)
            self.assertIn("security.svg", badges)
            self.assertIn("platforms.svg", badges)
            self.assertTrue((out / "c11.svg").exists())
            content = (out / "c11.svg").read_text(encoding="utf-8")
            self.assertIn("C11", content)


if __name__ == "__main__":
    unittest.main()

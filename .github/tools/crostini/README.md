# ChromeOS / Crostini verification

This check must run **inside a real Chromebook Linux development
environment**, not an ordinary Debian Docker runner.

1. Enable Linux development environment in ChromeOS Settings > Developers.
2. Install build tools inside Crostini (`sudo apt update && sudo apt install build-essential python3`).
3. Clone Cosmeron, select the target branch, and run `sh Tools/crostini/check.sh`.
4. Save `crostini-report.txt` and the terminal output; include the ChromeOS
   version and Chromebook model with your findings.

The check executes the portable and POSIX test suites, Input headers,
Network loopback, offline Audio and noninteractive Terminal/Canvas.
It records but does **not** grant permissions to /dev/input, USB, sound
devices, GPU or host input capture. Device presence is not evidence that
a joystick, camera, microphone or terminal API works. Test those manually
with consent and document the results.

Linux Crostini, Android applications on ChromeOS, and native ChromeOS
applications are three distinct targets and must not share a certification.

## Hardware exposure inventory

On the same physical Chromebook, run:

```sh
sh Tools/crostini/check-hardware.sh
```

Attach `crostini-hardware-report.txt` alongside `crostini-report.txt` to
issue #123. Detection of Crostini-specific filesystem markers is a guard
against accidentally treating a generic Linux CI container as ChromeOS.
Device nodes and their access bits are an **inventory**, not proof of
keyboard, USB/HID/joystick, audio or GPU functionality. If a supported
Crostini release lacks the markers, document the host evidence before
updating the guard. No elevated permissions are acquired by this script.

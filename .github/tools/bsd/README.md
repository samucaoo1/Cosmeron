# BSD interactive / hardware checks

The automated FreeBSD, OpenBSD, NetBSD and DragonFly CI guests exercise
offline modules and loopback networking. They do **not** establish that a
real terminal, input device or audio output works.

On each BSD system with a real interactive terminal and a C11 compiler, run
from the repository root:

```sh
sh Tools/bsd/check-tty.sh
```

This requires stdin and stdout to be terminals and executes the existing
Canvas, UI and UIAdvanced noninteractive regression programs. It records
`bsd-tty-report.txt` with host information. A successful result only proves
the TTY was present and these regressions executed; it does **not**
exercise keyboard events, mouse capture, interactive drawing or audio
hardware.

For issue #124, attach the report and separate manually recorded
results for terminal input/output and native sound-device support,
including device model, OS version and explicit permission requirements.

The full threaded Concurrency and full desktop Audio zero-extra-link
contract remains a separate unresolved acceptance criterion. Do not infer
zero-link compliance from functional runs with `-pthread` or `-lm`.

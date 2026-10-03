#!/usr/bin/env python3
"""Import pinned miniaudio, isolating identifiers and redirecting five math calls.
Usage: python3 .github/scripts/vendor-audio.py /path/to/miniaudio.h
The upstream header and its embedded licenses are preserved except for tokens.
"""
import hashlib
import re
import sys
from pathlib import Path

source = Path(sys.argv[1]).read_bytes()
expected = '7e4f3f13c8fe66df2080ac3dd12a89193e3c2463cb7f067c798abd7331cd8ee6'
if hashlib.sha256(source).hexdigest() != expected:
    raise SystemExit('Unexpected miniaudio version/content')
text = source.decode('utf-8')
# Comments, strings and character literals must not be rewritten (in particular
# symbol names passed to dlsym/GetProcAddress must keep their original spelling).
tokens = re.compile(r'/\*[\s\S]*?\*/|//[^\n]*|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|\b[A-Za-z_][A-Za-z_0-9]*\b')
math = {'sin': 'Sin', 'exp': 'Exp', 'log': 'Log', 'pow': 'Pow', 'sqrt': 'Sqrt'}
def replace(match):
    token = match.group()
    if token in math:
        return 'AUDIO_INS(' + math[token] + ')'
    if re.match(r'(?:_?ma_|MA_|MINIAUDIO_|miniaudio_|drwav|drflac|drmp3|DRWAV|DRFLAC|DRMP3)', token):
        return 'cosmeron_audio_vendor_' + token
    return token
out = Path('Codespace/Cosmeron/Modules/Audio/Internal/Miniaudio.inc')
text = tokens.sub(replace, text)
# The upstream external fallback lock must be shared, not duplicated per TU.
# Coalesced definitions preserve that contract in a header-only integration.
lock = 'cosmeron_audio_vendor_ma_atomic_spinlock cosmeron_audio_vendor_ma_atomic_global_lock;'
assert text.count(lock) == 1
text = text.replace(lock, """#if defined(_MSC_VER)
__declspec(selectany) cosmeron_audio_vendor_ma_atomic_spinlock cosmeron_audio_vendor_ma_atomic_global_lock = 0;
#elif defined(__GNUC__) || defined(__clang__)
__attribute__((weak, visibility("hidden"))) cosmeron_audio_vendor_ma_atomic_spinlock cosmeron_audio_vendor_ma_atomic_global_lock = 0;
#else
#error Cosmeron Audio desktop backend requires GCC, Clang, or MSVC.
#endif""")
out.write_text(text)
print(out)

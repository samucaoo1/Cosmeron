#!/usr/bin/env python3
"""Generate FLAC using independent libFLAC through its public encoder API.
Only tests need libFLAC; the delivered decoder uses original C11 code.
"""
import ctypes as c
import ctypes.util
import math
import sys
library = ctypes.util.find_library('FLAC')
if not library:
    sys.exit('libFLAC is required for independent interoperability fixtures')
lib = c.CDLL(library)
lib.FLAC__stream_encoder_new.restype = c.c_void_p
for name in ('channels', 'bits_per_sample', 'sample_rate', 'compression_level'):
    f = getattr(lib, 'FLAC__stream_encoder_set_' + name)
    f.argtypes = [c.c_void_p, c.c_uint]
    f.restype = c.c_int
lib.FLAC__stream_encoder_init_file.argtypes = [c.c_void_p, c.c_char_p, c.c_void_p, c.c_void_p]
lib.FLAC__stream_encoder_init_file.restype = c.c_int
lib.FLAC__stream_encoder_process_interleaved.argtypes = [c.c_void_p, c.POINTER(c.c_int32), c.c_uint]
lib.FLAC__stream_encoder_process_interleaved.restype = c.c_int
lib.FLAC__stream_encoder_finish.argtypes = [c.c_void_p]
lib.FLAC__stream_encoder_finish.restype = c.c_int
lib.FLAC__stream_encoder_delete.argtypes = [c.c_void_p]
path, depth = sys.argv[1], int(sys.argv[2])
assert depth in (16, 24, 32)
encoder = lib.FLAC__stream_encoder_new()
assert encoder
try:
    for name, value in [('channels', 2), ('bits_per_sample', depth), ('sample_rate', 48000), ('compression_level', 8)]:
        assert getattr(lib, 'FLAC__stream_encoder_set_' + name)(encoder, value)
    assert lib.FLAC__stream_encoder_init_file(encoder, path.encode(), None, None) == 0
    frames = 60000
    pcm = (c.c_int32 * (frames * 2))()
    for i in range(frames):
        for ch in range(2):
            t = i / 48000
            value = .25 * math.sin(2 * math.pi * ((173 + ch * 71) * t + 111 * t * t))
            pcm[i * 2 + ch] = int(value * ((1 << (depth - 1)) - 1))
    assert lib.FLAC__stream_encoder_process_interleaved(encoder, pcm, frames)
    assert lib.FLAC__stream_encoder_finish(encoder)
finally:
    lib.FLAC__stream_encoder_delete(encoder)

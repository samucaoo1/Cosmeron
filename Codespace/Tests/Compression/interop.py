"""Independent format interoperability; standard library only."""
import gzip
import os
import random
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
import zipfile
import zlib
exe = str(Path(sys.argv[1]).resolve())
with tempfile.TemporaryDirectory() as tmp:
    root = Path(tmp)
    source, encoded, decoded = [root / n for n in ('source', 'encoded', 'decoded')]
    def run(mode, fmt):
        subprocess.run([exe, mode, str(fmt), str(encoded if mode in ('decode','reject','zipread') else source), str(decoded if mode in ('decode','zipread') else encoded)], check=True)
    for payload in (b'', b'Cosmeron'*15000, os.urandom(70000), bytes(range(256))*32):
        source.write_bytes(payload)
        for fmt, window in ((0,-15),(1,15),(2,31)):
            run('encode',fmt)
            assert zlib.decompress(encoded.read_bytes(), window) == payload
            compressor = zlib.compressobj(9, zlib.DEFLATED, window)
            encoded.write_bytes(compressor.compress(payload) + compressor.flush())
            run('decode',fmt)
            assert decoded.read_bytes() == payload
        # Gzip optional fields and FHCRC, followed by a second member.
        header = b'\x1f\x8b\x08\x1e' + b'\0'*4 + b'\0\xff' + b'\x03\0xyz' + b'file\0comment\0'
        header += struct.pack('<H',zlib.crc32(header)&65535)
        compressor = zlib.compressobj(6,zlib.DEFLATED,-15)
        member = header + compressor.compress(payload) + compressor.flush() + struct.pack('<II',zlib.crc32(payload),len(payload))
        encoded.write_bytes(member + gzip.compress(b'tail',mtime=0))
        run('decode',2); assert decoded.read_bytes() == payload + b'tail'
        for bad in (member[:len(header)-1], member[:-1], member + b'garbage', member[:3]+b'\xff'+member[4:], member[:-8]+b'\0'*8):
            # Empty data has a zero CRC/size; all-zero trailer is valid there.
            if not payload and bad == member[:-8]+b'\0'*8: continue
            encoded.write_bytes(bad); run('reject',2)
        source.write_bytes(payload); run('zipwrite',0)
        with zipfile.ZipFile(encoded) as archive:
            assert archive.read('payload.bin') == payload and archive.read('empty') == b''
        for method in (zipfile.ZIP_STORED,zipfile.ZIP_DEFLATED):
            with zipfile.ZipFile(encoded,'w',compression=method) as archive:
                archive.writestr('directory/',b'')
                archive.writestr('payload.bin',payload)
            run('zipread',0); assert decoded.read_bytes() == payload
    # Deterministic cross-implementation differential corpus with varied entropy.
    rng = random.Random(0xC05E2026)
    for length in (1, 2, 3, 31, 257, 4095, 8193, 65535):
        for sample in (
            bytes(rng.randrange(256) for _ in range(length)),
            bytes(65 + rng.randrange(8) for _ in range(length)),
            (b"Cosmeron-DEFLATE-" * ((length + 16) // 17))[:length]
        ):
            source.write_bytes(sample)
            for fmt, window in ((0, -15), (1, 15), (2, 31)):
                run("encode", fmt)
                assert zlib.decompress(encoded.read_bytes(), window) == sample
                compressor = zlib.compressobj(9, zlib.DEFLATED, window)
                encoded.write_bytes(compressor.compress(sample) + compressor.flush())
                run("decode", fmt)
                assert decoded.read_bytes() == sample

    with zipfile.ZipFile(encoded,'w',compression=zipfile.ZIP_DEFLATED) as archive:
        with archive.open('zip64-small', 'w', force_zip64=True) as entry:
            entry.write(b'ZIP64 header')
    run('zipread',0); assert decoded.read_bytes() == b'ZIP64 header'
    with zipfile.ZipFile(encoded,'w',compression=zipfile.ZIP_BZIP2) as archive:
        archive.writestr('unsupported',b'data')
    run('zipread',0)
    with zipfile.ZipFile(encoded,'w',compression=zipfile.ZIP_STORED) as archive:
        archive.writestr('corrupt',b'CRC target')
    content = bytearray(encoded.read_bytes()); offset = content.index(b'CRC target'); content[offset] ^= 1
    encoded.write_bytes(content); run('zipread','corrupt')
print('DEFLATE/zlib/gzip/ZIP interoperate with Python; optional headers, concatenation and CRC checks pass.')

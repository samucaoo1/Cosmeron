#!/usr/bin/env python3
"""Build original MPEG-2 Layer III intensity-stereo frames for codec parity.
No third-party encoder, MP3 decoder or copyrighted recording is embedded.
The frames carry non-zero left-channel Huffman data and 2-bit right-channel
LSF intensity positions. A separate decoder (miniaudio) supplies reference PCM.
"""
import sys


class Bits:
    def __init__(self):
        self.data = []

    def put(self, value, count):
        assert 0 <= value < (1 << count)
        self.data.extend((value >> i) & 1 for i in range(count - 1, -1, -1))

    def output(self):
        assert len(self.data) % 8 == 0
        return bytes(
            sum(self.data[k + j] << (7 - j) for j in range(8))
            for k in range(0, len(self.data), 8)
        )


def granule(bits, part_length, big_values, gain, compress, table, short=False, mixed=False):
    bits.put(part_length, 12)
    bits.put(big_values, 9)
    bits.put(gain, 8)
    bits.put(compress, 9)
    bits.put(int(short), 1)
    if short:
        bits.put(2, 2)  # pure short blocks
        bits.put(int(mixed), 1)  # mixed_block_flag
        for _ in range(2):
            bits.put(table, 5)
        for _ in range(3):
            bits.put(0, 3)  # subblock_gain
    else:
        for _ in range(3):
            bits.put(table, 5)
        bits.put(15, 4)
        bits.put(0, 3)
    bits.put(0, 1)  # scalefac_scale
    bits.put(0, 1)  # count1table_select


def make_frame(frame_num, ms, intensity_scale, baseline=False, mono=False, short=False, mixed=False, rate=22050, bitrate=64, coded=False):
    mode = 0xC0 if mono else 0x60 if ms and baseline else (
        0x40 if baseline else 0x70 if ms else 0x50)
    version = 0xf3 if rate >= 16000 else 0xe3
    rate_bits = {22050: 0, 24000: 4, 16000: 8, 11025: 0, 12000: 4, 8000: 8}[rate]
    bitrate_bits = {16: 2, 32: 4, 64: 8}[bitrate] << 4
    header = bytes((0xFF, version, bitrate_bits | rate_bits, mode))
    # MPEG-2 22.05kHz, 64kbps, stereo: 208 bytes, 17-byte side info.
    bands = (list(range(0, 72, 6)) if mixed else
             list(range(0, 48, 4)) if short else
             [0, 6, 12, 18, 24, 30, 36])
    code = Bits()
    # The 1-Huffman book: (0,0)="1", (1,0)="01" plus sign bit.
    left_coefficients = set(bands)
    count = 36 if mixed else 24 if short else 22
    for pair in range(count):
        line = pair * 2
        if line in left_coefficients and (frame_num + pair) % 5 != 0:
            code.put(0b01, 2)
            code.put((frame_num // 2 + pair) % 2, 1)
        else:
            code.put(1, 1)
    left_part_length = len(code.data)
    pseed = [0, 1, 2, 3, 2, 1, 0]
    positions = [pseed[(i + frame_num // 3) % 7] for i in range(12 if short else 7)]
    if mixed:
        positions = [pseed[(i + frame_num // 3) % 7] for i in range(6)]
        positions += [(i + frame_num // 3) % 2 for i in range(15)]
    if not baseline:
        for k, pos in enumerate(positions):
            code.put(pos, 1 if mixed and k >= 6 else 2)
    right_part_length = 0 if baseline else (
        27 if mixed else len(positions) * 2)
    if coded and not mono:
        for pair in range(3):
            code.put(0b01, 2)
            code.put((frame_num + pair) % 2, 1)
        right_part_length += 9
    while len(code.data) % 8:
        code.put(0, 1)
    main_bytes = code.output()

    info = Bits()
    info.put(0, 8)  # main_data_begin
    info.put(0, 1 if mono else 2)  # private_bits
    granule(info, left_part_length, count, 188 + (frame_num % 5), 0, 1, short, mixed)
    # scalefac_compress>>1=72 => slen=(2,0,0,0), seven two-bit positions
    if not mono:
        granule(info, right_part_length, 3 if coded else 0, 188,
                0 if baseline else (156 if mixed else 144) + intensity_scale,
                1 if coded else 0, short, mixed)
    info_bytes = info.output()
    assert len(info_bytes) == (9 if mono else 17)
    length = 72000 * bitrate // rate
    assert len(header) + len(info_bytes) + len(main_bytes) <= length
    return header + info_bytes + main_bytes + bytes(
        length - len(header) - len(info_bytes) - len(main_bytes)
    )


def main():
    if len(sys.argv) not in (4, 5, 8):
        sys.exit("usage: MP3LSFIntensityFixture.py OUT.mp3 MS(0|1) INTENSITY_SCALE(0|1) [baseline]")
    path, ms, scale = sys.argv[1], int(sys.argv[2]), int(sys.argv[3])
    if ms not in (0, 1) or scale not in (0, 1):
        sys.exit("MS and intensity_scale must be 0 or 1")
    mode = sys.argv[4] if len(sys.argv) >= 5 else "long"
    if mode not in ("long", "short", "mixed", "baseline", "mono", "baseline-short", "baseline-mixed"):
        sys.exit("unrecognized fixture mode")
    baseline = mode in ("baseline", "mono", "baseline-short", "baseline-mixed")
    mono = mode == "mono"
    short = mode in ("short", "baseline-short", "mixed", "baseline-mixed")
    mixed = mode in ("mixed", "baseline-mixed")
    rate = int(sys.argv[5]) if len(sys.argv) == 8 else 22050
    bitrate = int(sys.argv[6]) if len(sys.argv) == 8 else 64
    coded = bool(int(sys.argv[7])) if len(sys.argv) == 8 else False
    with open(path, "wb") as out:
        for i in range(32):
            out.write(make_frame(i, bool(ms), scale, baseline, mono, short, mixed, rate, bitrate, coded))


if __name__ == "__main__":
    main()

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


def granule(bits, part_length, big_values, gain, compress, table):
    bits.put(part_length, 12)
    bits.put(big_values, 9)
    bits.put(gain, 8)
    bits.put(compress, 9)
    bits.put(0, 1)  # window_switching_flag=0; long blocks
    for _ in range(3):
        bits.put(table, 5)
    bits.put(15, 4)  # region0_count; all spectral pairs use book1
    bits.put(0, 3)
    bits.put(0, 1)  # scalefac_scale
    bits.put(0, 1)  # count1table_select


def make_frame(frame_num, ms, intensity_scale, baseline=False):
    header = bytes((0xFF, 0xF3, 0x80, 0x60 if ms and baseline else
                    0x40 if baseline else 0x70 if ms else 0x50))
    # MPEG-2 22.05kHz, 64kbps, stereo: 208 bytes, 17-byte side info.
    bands = [0, 6, 12, 18, 24, 30, 36]
    code = Bits()
    # The 1-Huffman book: (0,0)="1", (1,0)="01" plus sign bit.
    left_coefficients = set(bands)
    for pair in range(22):
        line = pair * 2
        if line in left_coefficients and (frame_num + pair) % 5 != 0:
            code.put(0b01, 2)
            code.put((frame_num // 2 + pair) % 2, 1)
        else:
            code.put(1, 1)
    left_part_length = len(code.data)
    positions = [0, 1, 2, 3, 2, 1, 0]
    positions = [positions[(i + frame_num // 3) % 7] for i in range(7)]
    if not baseline:
        for pos in positions:
            code.put(pos, 2)
    right_part_length = 0 if baseline else 14
    while len(code.data) % 8:
        code.put(0, 1)
    main_bytes = code.output()

    info = Bits()
    info.put(0, 8)  # main_data_begin
    info.put(0, 2)  # private_bits
    granule(info, left_part_length, 22, 210 + (frame_num % 5), 0, 1)
    # scalefac_compress>>1=72 => slen=(2,0,0,0), seven two-bit positions
    granule(info, right_part_length, 0, 210,\n            0 if baseline else 144 + intensity_scale, 0)
    info_bytes = info.output()
    assert len(info_bytes) == 17
    length = 208
    assert len(header) + len(info_bytes) + len(main_bytes) <= length
    return header + info_bytes + main_bytes + bytes(
        length - len(header) - len(info_bytes) - len(main_bytes)
    )


def main():
    if len(sys.argv) not in (4, 5):
        sys.exit("usage: MP3LSFIntensityFixture.py OUT.mp3 MS(0|1) INTENSITY_SCALE(0|1) [baseline]")
    path, ms, scale = sys.argv[1], int(sys.argv[2]), int(sys.argv[3])
    if ms not in (0, 1) or scale not in (0, 1):
        sys.exit("MS and intensity_scale must be 0 or 1")
    baseline = len(sys.argv) == 5 and sys.argv[4] == "baseline"
    with open(path, "wb") as out:
        for i in range(32):
            out.write(make_frame(i, bool(ms), scale, baseline))


if __name__ == "__main__":
    main()

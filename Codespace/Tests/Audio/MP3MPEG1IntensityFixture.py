#!/usr/bin/env python3
"""Author genuine MPEG-1 Layer III intensity-stereo bitstreams for PCM parity.

This constructs MPEG headers, 32-byte MPEG-1 stereo side information, two
granules/channel, book-1 Huffman symbols and intensity scalefactors directly.
No third-party MP3 encoder implementation or media asset is embedded.
"""
import sys


class Bits:
    def __init__(self):
        self.bits = []

    def put(self, value, width):
        assert 0 <= value < (1 << width)
        self.bits.extend((value >> i) & 1 for i in range(width - 1, -1, -1))

    def append(self, other):
        self.bits.extend(other.bits)

    def bytes(self):
        assert len(self.bits) % 8 == 0
        return bytes(sum(self.bits[i + k] << (7 - k) for k in range(8))
                     for i in range(0, len(self.bits), 8))


def side_granule(side, bits_count, pairs, gain, compress, table, layout):
    side.put(bits_count, 12)
    side.put(pairs, 9)
    side.put(gain, 8)
    side.put(compress, 4)
    side.put(int(layout != "long"), 1)
    if layout != "long":
        side.put(2, 2)
        side.put(int(layout == "mixed"), 1)
        for _ in range(2):
            side.put(table, 5)
        for _ in range(3):
            side.put(0, 3)
    else:
        for _ in range(3):
            side.put(table, 5)
        side.put(15, 4)
        side.put(0, 3)
    side.put(0, 1)  # preflag
    side.put(0, 1)  # scalefac_scale
    side.put(0, 1)  # count1table_select


def payload(frame_no, granule, baseline, mono, layout, coded):
    # 80 pairs of 2 quantized spectral lines, nonzero in different sfb,
    # including regions well above the last right-coded scalefactor.
    marked = {0, 12, 26, 48, 76, 96, 118, 156}
    l = Bits()
    for pair in range(80):
        line = pair * 2
        if line in marked and ((frame_no + pair + granule) % 5 != 0):
            l.put(0b01, 2)  # table 1 (1,0)
            l.put((frame_no // 2 + pair + granule) % 2, 1)  # sign
        else:
            l.put(1, 1)  # table 1 (0,0)
    r = Bits()
    if not baseline and not mono:
        # MPEG-1 scalefac_compress=5 => slen1=slen2=1.
        # All 21 long sfb have one transmitted bit. Upper bands remain
        # position 0 so left-side spectral energy is intensity-coded.
        for band in range(21 if layout == "long" else 36 if layout == "short" else 35):
            r.put((((frame_no // 3) + granule + band) & 1)
                  if band < 11 else 0, 1)
    if coded and not mono:
        for pair in range(3):
            r.put(0b01, 2)
            r.put((frame_no + granule + pair) % 2, 1)
    return l, r


def frame(frame_no, ms, baseline, mono, layout="long", rate=44100, coded=False, bitrate=128):
    mode = 0xC0 if mono else (
        (0x60 if ms else 0x40) if baseline else
        (0x70 if ms else 0x50)
    )
    # MPEG-1 Layer III, 128kbps, 44100Hz, 417-byte frames.
    rate_bits = {44100: 0, 48000: 4, 32000: 8}[rate]
    frame_size = 144000 * bitrate // rate
    bitrate_bits = {64: 5, 128: 9, 320: 14}[bitrate] << 4
    hdr = bytes((0xff, 0xfb, bitrate_bits | rate_bits, mode))
    parts = []
    channel_count = 1 if mono else 2
    for granule in range(2):
        left, right = payload(frame_no, granule, baseline, mono, layout, coded)
        parts.append((left, right))
    side = Bits()
    side.put(0, 9)  # main_data_begin: frame is self-contained
    side.put(0, 5 if mono else 3)
    for _ in range(channel_count * 4):
        side.put(0, 1)  # all scfsi flags disabled
    main = Bits()
    for left, right in parts:
        side_granule(side, len(left.bits), 80, 186 + frame_no % 7, 0, 1, layout)
        main.append(left)
        if not mono:
            side_granule(side, len(right.bits), 3 if coded else 0, 188,
                         0 if baseline else 5, 1 if coded else 0, layout)
            main.append(right)
    assert len(side.bits) == (17 if mono else 32) * 8
    while len(main.bits) % 8:
        main.put(0, 1)
    combined = hdr + side.bytes() + main.bytes()
    assert len(combined) <= frame_size
    return combined + bytes(frame_size - len(combined))


def main():
    if len(sys.argv) not in (5, 7, 9):
        sys.exit("usage: MP3MPEG1IntensityFixture.py OUT.mp3 MS(0|1) baseline|intensity|mono COUNT")
    out, ms, mode, count = sys.argv[1], int(sys.argv[2]), sys.argv[3], int(sys.argv[4])
    if ms not in (0, 1) or mode not in ("baseline", "intensity", "mono") or not 2 <= count <= 4096:
        sys.exit("invalid mode/count")
    layout = sys.argv[5] if len(sys.argv) >= 7 else "long"
    rate = int(sys.argv[6]) if len(sys.argv) >= 7 else 44100
    coded = bool(int(sys.argv[7])) if len(sys.argv) == 9 else False
    bitrate = int(sys.argv[8]) if len(sys.argv) == 9 else 128
    assert layout in ("long", "short", "mixed")
    with open(out, "wb") as file:
        for n in range(count):
            file.write(frame(n, ms, mode != "intensity", mode == "mono", layout, rate, coded, bitrate))


if __name__ == "__main__":
    main()

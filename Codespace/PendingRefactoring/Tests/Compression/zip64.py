"""Exercise ZIP64 end records and per-entry ZIP64 metadata using tiny, standalone fixtures."""
import pathlib
import struct
import subprocess
import sys
import tempfile
import zipfile
import zlib

exe = str(pathlib.Path(sys.argv[1]).resolve())
name = b"payload.bin"
payload = (b"Cosmeron ZIP64 reader interoperability.\n" * 3) + bytes(range(32))
crc = zlib.crc32(payload)
obj = zlib.compressobj(6, zlib.DEFLATED, -15)
packed = obj.compress(payload) + obj.flush()

def fixture():
    localextra = struct.pack("<HHQQ", 1, 16, len(payload), len(packed))
    local = struct.pack("<IHHHHHIIIHH", 0x04034b50, 45, 0, 8, 0, 0,
                        crc, 0xffffffff, 0xffffffff, len(name), len(localextra))
    local += name + localextra + packed
    centralextra = struct.pack("<HHQQQ", 1, 24, len(payload), len(packed), 0)
    central = struct.pack("<IHHHHHHIIIHHHHHII", 0x02014b50, 45, 45, 0, 8, 0, 0,
                          crc, 0xffffffff, 0xffffffff, len(name),
                          len(centralextra), 0, 0, 0, 0, 0xffffffff)
    central += name + centralextra
    end = struct.pack("<IQHHIIQQQQ", 0x06064b50, 44, 45, 45, 0, 0,
                      1, 1, len(central), len(local))
    locator = struct.pack("<IIQI", 0x07064b50, 0, len(local) + len(central), 1)
    eocd = struct.pack("<IHHHHIIH", 0x06054b50, 0, 0, 0xffff, 0xffff,
                       0xffffffff, 0xffffffff, 0)
    return local + central + end + locator + eocd

with tempfile.TemporaryDirectory() as tmp:
    root = pathlib.Path(tmp)
    source = root / "zip64.zip"
    output = root / "out.bin"
    archive = fixture()
    source.write_bytes(archive)
    with zipfile.ZipFile(source) as ref:
        assert ref.read("payload.bin") == payload
    subprocess.run([exe, "zipread", "0", str(source), str(output)], check=True)
    assert output.read_bytes() == payload

    # Metadata with a ZIP64 local offset beyond the archive cannot be accessed.
    bad = bytearray(archive)
    index = bad.index(struct.pack("<HHQQQ", 1, 24, len(payload), len(packed), 0))
    struct.pack_into("<Q", bad, index + 20, 0x100000000)
    source.write_bytes(bad)
    subprocess.run([exe, "rejectzip", "0", str(source), str(output)], check=True)

    # Invalid ZIP64 EOCD locator outside the provided buffer must fail.
    bad = bytearray(archive)
    pos = bad.index(struct.pack("<I", 0x07064b50))
    struct.pack_into("<Q", bad, pos + 8, 0xffffffffffffffff)
    source.write_bytes(bad)
    subprocess.run([exe, "rejectzip", "0", str(source), str(output)], check=True)
    # Writer must cross the classic 65,535-entry boundary without truncation.
    source.write_bytes(b"")
    interop = str(pathlib.Path(exe).with_name("Interop"))
    subprocess.run([interop, "zipwrite64", "0", str(source), str(output)], check=True)
    generated = output.read_bytes()
    assert struct.pack("<I", 0x06064b50) in generated
    assert struct.pack("<I", 0x07064b50) in generated
    with zipfile.ZipFile(output) as ref:
        assert len(ref.namelist()) == 65536
        assert ref.namelist()[0] == "entry-00000"
        assert ref.namelist()[-1] == "entry-65535"

print("ZIP64 central-directory, locator, and per-entry metadata checks passed.")

#include "../../Cosmeron/Modules/Audio/Codec/MP3.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

/* Host-generated MPEG-2/2.5 fixtures: validate actual scalefac_compress,
 * partition bit lengths, channel offsets and byte reservoir continuity.
 * This does not claim that full LSF PCM synthesis is available yet. */
int main(int argc, char **argv) {
  unsigned char fileBytes[65536];
  AUDIO_CODEC_TYPE(TMP3FrameInfo) header;
  AUDIO_CODEC_TYPE(TMP3SideInfo) side;
  AUDIO_CODEC_TYPE(TMP3Reservoir) reservoir = {0};
  FILE *file;
  size_t length, offset = 0, frames = 0, parsed = 0, preroll = 0;
  assert(argc == 3);
  file = fopen(argv[1], "rb");
  assert(file != NULL);
  length = fread(fileBytes, 1, sizeof fileBytes, file);
  assert(fclose(file) == 0 && length > 1000);
  assert(AUDIO_CODEC_FUNC(MP3_FindFrame)(
      fileBytes, length, 0, &offset, &header) == STATUS_CONST(SUCCESS));
  assert(header.mpegVersion == (unsigned int)atoi(argv[2]));
  while (length - offset >= 4 && frames < 50) {
    const unsigned char *data = NULL;
    size_t dataBytes = 0;
    unsigned int ch;
    OPSTATUS status;
    assert(AUDIO_CODEC_FUNC(MP3_ParseHeader)(
        fileBytes + offset, length - offset, &header) ==
        STATUS_CONST(SUCCESS));
    if (header.frameBytes > length-offset) break;
    assert(header.samplesPerFrame == 576);
    status = AUDIO_CODEC_FUNC(MP3_ReservoirFeed)(
        &reservoir, fileBytes + offset, header.frameBytes,
        &data, &dataBytes, &side);
    if (status == STATUS_CONST(INCOMPLETE_SEQUENCE)) ++preroll;
    else {
      assert(status == STATUS_CONST(SUCCESS));
      for (ch = 0; ch < side.channels; ++ch) {
        AUDIO_CODEC_TYPE(TMP3Scalefactors) scale;
        uint8_t intensity = 255;
        status = AUDIO_CODEC_FUNC(MP3_ParseScalefactorsLSF)(
            data,dataBytes,&header,&side,ch,&scale,&intensity);
        assert(status == STATUS_CONST(SUCCESS));
        assert(scale.part2Bits <= side.parts[0][ch].part23Length);
        assert(intensity <= 1);
        ++parsed;
      }
    }
    offset += header.frameBytes;
    ++frames;
  }
  assert(frames >= 4 && parsed >= 4);
  printf("[mp3 LSF] file=%s version=%u frames=%zu scalefactors=%zu "
         "preroll=%zu\n", argv[1], header.mpegVersion,
         frames, parsed, preroll);
  return 0;
}

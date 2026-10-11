#include "../../Cosmeron/Modules/Audio/Offline.h"
OPSTATUS OfflineTU_Beep(AUDIO_PCM_TYPE(TBuffer) *buffer) {
  return AUDIO_BEEP_FUNC(RenderTone)(440, 10, 0.4f, 48000, buffer);
}

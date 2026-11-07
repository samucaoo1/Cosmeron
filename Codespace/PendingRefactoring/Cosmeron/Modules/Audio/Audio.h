#pragma once
/*
 * Compatible umbrella header. AUDIO_NO_DESKTOP retains the original
 * offline-only contract, while standalone CodecOnly.h and Desktop.h allow
 * consumers to select only their needed public API surface.
 */
#include "Offline.h"
#ifndef AUDIO_NO_DESKTOP
#include "Desktop.h"
#endif

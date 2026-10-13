#include "../../Cosmeron/Modules/Audio/Device/Manual.h"
#include <string.h>

/* Deterministic simulated sink: this test actually executes Manual_Pump
 * without relying on a physical PCM device. Partial writes cross frames. */
typedef struct {
  unsigned char bytes[513u * 2u * 2u];
  size_t length, calls, frames, renderCalls;
  bool stall;
} TTestSink;
static void Render(void *argument, float *dst, size_t frames, uint32_t channels) {
  TTestSink *sink = (TTestSink *)argument;
  size_t i;
  sink->frames += frames;
  sink->renderCalls++;
  for (i = 0; i < frames * channels; ++i)
    dst[i] = (i % 2u == 0u) ? -1.0f : 1.0f;
}
static OPSTATUS Write(void *argument, const void *bytes, size_t capacity,
                      size_t *outWritten) {
  TTestSink *sink = (TTestSink *)argument;
  size_t n = capacity > 37u ? 37u : capacity;
  ++sink->calls;
  if (sink->stall) {
    *outWritten = 0;
    return STATUS_CONST(SUCCESS);
  }
  if (sink->length > sizeof sink->bytes - n)
    return STATUS_CONST(OUT_OF_RANGE);
  memcpy(sink->bytes + sink->length, bytes, n);
  sink->length += n;
  *outWritten = n;
  return STATUS_CONST(SUCCESS);
}
int main(void) {
  AUDIO_DEVICE_TYPE(TManual) device = {0};
  TTestSink sink = {0};
  AUDIO_DEVICE_TYPE(TManualConfig) config = {48000, 2, Render, &sink, NULL};
  size_t i;
  if (AUDIO_DEVICE_FUNC(Manual_OpenSink)(&device, &config, Write, &sink) !=
      STATUS_CONST(SUCCESS))
    return 1;
  if (AUDIO_DEVICE_FUNC(Manual_OpenSink)(&device, &config, Write, &sink) !=
      STATUS_CONST(BUSY))
    return 2;
  if (AUDIO_DEVICE_FUNC(Manual_Pump)(&device, 1) !=
      STATUS_CONST(NOT_AVAILABLE))
    return 3;
  if (AUDIO_DEVICE_FUNC(Manual_Start)(&device) != STATUS_CONST(SUCCESS) ||
      !AUDIO_DEVICE_FUNC(Manual_IsStarted)(&device))
    return 4;
  if (AUDIO_DEVICE_FUNC(Manual_Pump)(&device, 513) !=
      STATUS_CONST(SUCCESS))
    return 5;
  if (sink.frames != 513 || sink.renderCalls != 2 || sink.calls < 50 ||
      sink.length != sizeof sink.bytes)
    return 6;
  for (i = 0; i < sizeof sink.bytes; i += 4) {
    if (sink.bytes[i] != 0 || sink.bytes[i+1] != 0x80 ||
        sink.bytes[i+2] != 0xff || sink.bytes[i+3] != 0x7f)
      return 7;
  }
  sink.stall = true;
  if (AUDIO_DEVICE_FUNC(Manual_Pump)(&device, 1) !=
      STATUS_CONST(GENERIC_ERROR))
    return 8;
  if (AUDIO_DEVICE_FUNC(Manual_Stop)(&device) != STATUS_CONST(SUCCESS) ||
      AUDIO_DEVICE_FUNC(Manual_IsStarted)(&device))
    return 9;
  AUDIO_DEVICE_FUNC(Manual_Close)(&device);
  AUDIO_DEVICE_FUNC(Manual_Close)(&device);
  if (device.internal != NULL)
    return 10;
  if (AUDIO_DEVICE_FUNC(Manual_OpenSink)(&device, &config, NULL, &sink) !=
      STATUS_CONST(INVALID_ARGUMENT))
    return 11;
  return 0;
}

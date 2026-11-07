# Audio fixtures

Original synthetic 440 Hz tone, 100 ms, 44100 Hz, stereo. Generated for these
tests, with no external recording or musical work:

```sh
ffmpeg -f lavfi -i 'sine=frequency=440:sample_rate=44100:duration=0.10' -ac 2 -c:a pcm_s16le Tone.wav
ffmpeg -i Tone.wav -c:a libmp3lame -b:a 96k Tone.mp3
ffmpeg -i Tone.wav Tone.flac
```

The fixtures are committed so FFmpeg is not a runtime, build, or CI dependency.
Tests allow MP3 priming/padding frames rather than assuming the WAV frame count.

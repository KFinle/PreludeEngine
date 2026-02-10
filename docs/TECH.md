# Tech Notes

This is a small rhythm gameplay prototype built around a composition-driven pipeline.

## Architecture Snapshot

- `src/Game`
  - UI/HUD + debug tooling (piano roll, transport)
  - Game-specific glue (scenes, music loading)

- `src/Engine`
  - SDL3 runtime (window + event pump + keyboard/gamepad input)
  - Minimal 2D drawing primitives (lines/triangles) and debug text
  - Audio playback via miniaudio (file playback + in-memory PCM loading)

- `src/Rhythm`
  - Music composition + rendering (synth, event sequences, orchestration)
  - DSP helpers + lightweight utilities shared by gameplay

- `src/Gameplay`
  - Gameplay rules + state (scoring, spawning, timing)

- `third_party`
  - Single-file third-party deps

## Audio Pipeline

- The game renders an `EventSequence` to a float buffer.
- The engine loads that PCM into the audio backend and plays it back.
- This removes the previous "render WAV to disk, then re-load" flow.


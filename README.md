# PreludeEngine: Composition API + Rhythm Game Engine

Build:

- Linux: `./build-linux` (or `./build-linux release`)
- macOS: `./build-macos` (or `./build-macos release`)
- Windows: `build-windows.bat` (or `build-windows.bat release`)

More: `docs/BUILDING.md`

## Introduction  
PreludeEngine is one part music composition API, one part rhythm game engine. 
You write songs in code, and then they get rendered out to playable songs.
All sounds are written from scratch using an oscillator and an envelope filter. More details can be found in the Composition Layer header.

This project is built around a single main component: a baked `EventSequence`.
That sequence drives both audio synthesis and gameplay timing, so the rhythm you
hear is always the rhythm you play.

The `src/Rhythm` library composes a song into an `EventSequence`. The `src/Gameplay`
library derives timing targets and spawns notes from that same sequence. The `src/Game`
layer hosts scenes and UI, and the `src/Engine` layer provides the SDL and audio backend.

NOTE: This project was initially prepared for use in the Ubisoft Toronto NEXT competition, but has since been expanded on to remove any dependency to their provided contest API. 
The core composition API was worked on for months in advance of the competition, and I felt it was a compelling enough module to be expanded on for personal projects. 
Here's a link to the original video -> [Prelude Early Demo](https://youtu.be/AW5nzGQ75Y8)

---

## System Overview: One Sequence, Two Outputs

1. Compose a song via the `src/Rhythm` layer to build an `EventSequence`.

2. Render the `EventSequence` into an in-memory PCM buffer on a worker thread.

3. Use the same `EventSequence` to derive gameplay timing targets.

4. Spawn gameplay notes from those targets, then drive HUD, scoring, and input.


The result is tight timing with zero external charting; the music itself is the chart.

---

## Composition Layer


This system is a musical intent compiler. You write what the music should
do and the engine compiles that intent into a fixed `EventSequence`. That
sequence can then be rendered to audio and reused for gameplay logic.

### Musical Context + SequenceBuilder  


`MusicalContext` defines the grid. `SequenceBuilder` collects notes in musical time and builds the final `EventSequence`.

```cpp
// initialize
MusicalContext ctx;
ctx.bpm = 140;
ctx.beats_per_bar = 4;

SequenceBuilder seq(ctx);
SectionBuilder s(seq);
...
...
// build the final sequence
mix.kick = 1.1f;
mix.chord = 0.08f;
EventSequence sequence = seq.Build(mix);
```

### SectionBuilder: The Main Interface  


`SectionBuilder` lets developers think in song sections (Intro, Verse, Chorus, etc) instead
of in raw timestamps. Every layer inside a section is relative to that section
start, and the builder auto-advances the bar cursor when `End()` is called.

```cpp
SectionBuilder s(seq);
s.BeginSection("Chorus", 8)
 .Drums("x---x---x---x---", "----x-------x---", "x-x-x-x-x-x-x-x-")
 .Bass("E1 . B0 . E1 . D#1 -", NoteDuration::Eighth, true)
 .ChordRhythm("X-x-xX-x-x-X-Xx-", NoteDuration::Sixteenth, Chords::Power, Pitch::E, 2, 0.95f)
 .Melody("E4 - B3 E4 G4 ~ B3 -", NoteDuration::Sixteenth, true)
 .End();
```

Key behaviors:

- Sections automatically advance the internal barline cursor.

- Instrument order does not matter; all layers are merged at build time.

- Under-writing patterns is allowed to create intentional silence.


### Pattern Grammar: Fast to Write, Easy to Read  


Patterns are human-readable strings that get compiled into real notes.

#### Drums  


Drum patterns default to a 16th-note grid.

- `x` or `X` = hit (`X` is accented)

- `-` or any other char = rest

Internally, drum patterns use `PatternLayer` with a `NoteSpec` from `DrumKit`,
so kick/snare/hat already know their articulation and voice type.
Drums are percussive: holds are treated like rests, and note
length is clamped to the grid step so grooves stay tight even when patterns
repeat or under-write.

#### Bass and Melody: Monophonic Tokens  


Bass and melody are built by `MelodyBuilder` using a note grid defined by
`NoteDuration`. Tokens are simple:

- `C4`, `F#3`, `As1` `Bb2` = note

- `.` = hold

- `-` = rest

- `~` = slide to the next note

- `|` = visual barline (ignored by the parser)


This is enough to write dense melodies quickly, but the system still enforces
a strict bar-aligned grid for consistent timing.

#### Harmony and ChordRhythm  


Sustained harmony uses a chord function per bar. Rhythmic harmony uses
`ChordRhythm`, which applies a pattern to a chord (chugs, stabs, syncopation).
Chord definitions come from `ChordHelpers` (Major, Minor7, Power, etc).
The key difference from drums: `ChordRhythm` is pitched and sustained, so each
hit fans out into multiple notes, the grid is set by the `NoteDuration` you
pass in, and `gate` trims the note length for tight chugs. Holds (`.`) extend
the chord instead of acting like silence.

### Full Section Example  


```cpp
SectionBuilder s(seq);

s.BeginSection("Example", 4)
 .Drums("x---x---x---x---", "----x-------x---", "x-x-x-x-x-x-x-x-")
 .Bass("E1 . B0 . E1 . D#1 -", NoteDuration::Eighth, true)
 .Harmony({
     {Chords::Minor7,    Pitch::C,  3},
     {Chords::Dominant7, Pitch::Ds, 3},
     {Chords::Major7,    Pitch::F,  3},
     {Chords::Minor7,    Pitch::C,  3},
 })
 .ChordRhythm("X-x-xX-x-x-X-Xx-", NoteDuration::Sixteenth, Chords::Power, Pitch::E, 2, 0.85f)
 .Melody("E4 - B3 E4 G4 ~ B3 -", NoteDuration::Sixteenth, true)
 .End();
```

### Output: EventSequence  

The API converts high-level patterns into `NoteSpec`, then into `NoteEvent`
via `NoteBuilder`. `EventSequence` bakes beats to seconds and sorts by time. This `EventSequence` drives both audio rendering and gameplay timing, so audio and gameplay never drift.

---

## Audio Rendering and Synth Layer  

Once the `EventSequence` is baked, the renderer converts it into a float PCM buffer.
The game then loads that buffer into the runtime audio backend.

High-level flow:

1. `EventSequenceRenderer` walks notes in time order.

2. Each note is dispatched to a voice renderer based on `VoiceType`.

3. The voice output is mixed into a single float buffer.

4. `MusicClipManager` loads the PCM into the engine and controls play/stop/unload.

Optional: `WavWriter` can export buffers as WAV for debugging/export.


Voice coverage:

- **KickSynth**: sine sweep + transient click for punch.

- **NoiseSynth**: white noise with envelopes for snare and hats.

- **SquareSynth**: melodic/chord voices, supports pitch slides.

- **TriangleSynth**: bass voice for weight under the mix.


The system is minimal but expandable. Each voice is a small,
purpose-built synth chain (oscillator + envelope); `MixSettings`
lets each song rebalance the voices without re-authoring the music.

---

## Note Entities: Audio == Gameplay  


There are two kinds of notes in the system, which share the
same timing source.

### Audio Notes  

`NoteSpec` is the authoring unit establishing pitch, octave, duration, articulation, voice,
gate, and velocity. A `NoteEvent` is the baked unit; it has start/duration in both
beats and seconds, plus pitch and voice metadata. These events are what
`EventSequenceRenderer` uses to synthesize audio.

### Gameplay Note Entities  

Gameplay notes live in `GameplayPool::NotePool`. Each note has:

- `beat` (when it should be hit)

- `lane` (Up, Right, Down, Left)

- `voice` (Kick, Snare, Lead, etc.)

- motion data (distance and speed)

- life flags (consumed / miss registered)


Notes are spawned ahead of time, move toward the reticle at a beat-based speed,
and are culled once consumed or too old. This keeps entity counts bounded even
under heavy density.

---

## How We Go From Sound --> Note Rhythms  


This is the pipeline that removes the need for any external timing charts.

1. **Composition**: Patterns become a baked `EventSequence` in musical time.

2. **Audio**: The renderer walks the sequence and synthesizes PCM.

3. **Targets**: `Rhythm::CollectTimingTargets` filters the same sequence by voice

   (Kick, Snare, Melody, etc.) and emits beat targets inside a lookahead window.

4. **Notes**: gameplay spawning turns those beats into note entities and assigns lanes
   via `GetLaneForBeat` hashing.

5. **Sync**: `MusicTransport` applies mild latency compensation so visuals stay
   glued to the audio.

Because the same `EventSequence` drives both outputs, the rhythm you hear is
always the rhythm you play.

---


## Pools

### 1. Note Pool: Gameplay Entities

`GameplayPool::NotePool` is a tight vector-backed pool (`src/Gameplay/NotePool.h`).


- Spawn is beat-based, not frame-based, for concrete timing.


- Beat-bucket de-dupe avoids double-spawns during overlapping scans.


- `CullByBeat` keeps the pool bounded as the song advances.


### 2. Ghost Pool: HUD Visualization

`HUDGhostPool` precomputes upcoming beats from the `EventSequence` and draws
ghost notes outside the lanes so dense streams remain readable.


- Built once per song/mode/lanes mask, then drawn each frame.


- Per-lane arrays keep iteration cheap.


- Hard draw caps prevent runaway visuals in stress tests.


### 3. Particle Pool: Hit Feedback

`HUDParticlePool` spawns short-lived bursts on Good/Perfect hits.


- Uses a max count cap to stay stable.

- Burst parameters (speed, life, radius) are randomized for visual energy.


### Stress Testing

`GameMode::Test` uses `MakeSong_NotePoolTest` and `Rhythm::TimingTargetMode::All` with constant 16th notes spawning at 300bpm. This is to directly test the pooling system for a huge number of entities.

---

## HUDs: Skins, Lanes, and Readability 

The HUD system is built around `IHUDSkin`, which defines a visual style and
exposes layout hooks. The game queries the skin every frame, so the HUD can be swapped without touching game logic.

Key points:


- **Dynamic lanes**: each skin returns an `active_lanes` mask. The game adapts
  spawn + spacing and resets pools automatically when the mask changes.


- **Ghost visualization**: optional ghost paths show upcoming targets outside
  the playable lanes.


- **Multiple styles**: each skin can change geometry, colors, and background
  while reusing the same gameplay state.



This setup allows developers to create any HUD layout they want while maintaining the core gameplay. For this submission, I have provided 2 + a debugging piano roll, but in early testing of the audio system I had several more swappable HUD options as a proof-of-concept.  

---

## General Implementation Notes

- **Scene flow**: `GameplayScene` selects a song, kicks off async rendering, and
  starts playback once the buffer is ready. On exit it stops and unloads audio.


- **Audio pipeline**: `EventSequenceRenderer` synthesizes notes into a float
  buffer (Kick, Noise, Square, Triangle voices). `MusicClipManager` loads PCM into
  the engine. `WavWriter` is available for export/debug.


- **Timing model**: `MusicTransport` provides beat-accurate time with latency
  compensation; timing windows and stability deltas live on `GameState::gameplay.timing_policy`.


- **HUD system**: `IHUDSkin` allows dynamic lane counts, ghost paths, and
  visually distinct HUD styles without rewriting gameplay logic. Any subset of
  lanes can be enabled per skin, and the game adapts spawn + spacing to match.


- **Debugging**: `HUDMode::DebugRoll` renders a piano roll of the sequence to
  verify timing and density at a glance.



---

## Final Thoughts  


The composition layer is the centrepiece of this entire project. It converts human-readable musical intent into a concrete event stream that powers both the audio and the gameplay. Pooling, HUD flexibility, and beat-locked timing do the rest. The result is a self-contained
rhythm game engine that is authored without the need for external audio/MIDI sources, scales to dense note counts, and stays perfectly in sync with its own music.

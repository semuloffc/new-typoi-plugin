# Petrichor - Generative Nature Synthesizer

**Version 0.1.0** | By Reflexed

Petrichor is a VST3 synthesizer that generates natural phenomena: dew drops, canopy ambience, wind, soil rumble, and thunder. It plays autonomously, adapts to your track's key and tempo, and fits into nearly any musical context.

## Features

- **5 Synthesis Engines:**
  - **DEW**: Percussive water drops with modal resonators (glass, wood, metal)
  - **BLOOM**: Lush additive pad with bio-drift and chorus
  - **BREEZE**: Filtered wind with rhythmic gusts
  - **ROOTS**: Deep brown noise with breathing modulation
  - **BURST**: Thunder strikes with automatic and manual triggers

- **Scale Lock**: Quantize all pitches to Root + Scale (Neutral-6, Major-6, Minor/Major Pentatonic, Chromatic)
- **Host Sync**: Automatically syncs to DAW tempo and position (free-runs at 70 BPM without a host)
- **Macros**: GROWTH, WEATHER (Storm/Humid) for expressive sound shaping
- **Multi-Output**: Main stereo + 5 aux stereo buses (one per engine)
- **MIDI Control**: Follow held chords to set key; trigger thunder with C1

## Installation

### Windows
Copy `Petrichor.vst3` to:
```
C:\Program Files\Common Files\VST3\
```

### macOS
Copy `Petrichor.vst3` to:
```
/Library/Audio/Plug-Ins/VST3/
or
~/Library/Audio/Plug-Ins/VST3/
```

### Linux
Copy `Petrichor.vst3` to:
```
~/.vst3/
or
/usr/local/lib/vst3/
```

## Quick Start

1. Load Petrichor in your DAW
2. The default preset "Canopy at Dawn" plays immediately
3. Adjust **GROWTH** (0-100%) to evolve the soundscape:
   - 0-33%: Sparse dew drops
   - 33-66%: Bloom opens, breeze intensifies
   - 66-100%: Full ecosystem with soil and thunder
4. Use **WEATHER** controls:
   - **Storm** (0-100): Increases intensity, splash rate, and thunder probability
   - **Humid** (0-100): More reverb, darker tone, longer decays

## System Requirements

- Windows 10+, macOS 10.13+, or Linux
- VST3-compatible DAW
- CPU: Modern dual-core processor (plugin uses <3% of one core at 48 kHz)
- RAM: 50 MB

## Building from Source

Requires CMake 3.22+ and a C++17 compiler.

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

The VST3 will be in `build/Petrichor_artefacts/Release/VST3/`.

## License

© 2024 Reflexed. All rights reserved.

## Support

For issues and feedback: https://github.com/reflexed/petrichor/issues

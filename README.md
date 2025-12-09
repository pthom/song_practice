# SongPractice

A cross-platform desktop application for singers to practice with audio tracks.

## Features

- Load audio files (MP3, WAV, FLAC, etc.)
- Visual waveform display with interactive cursor
- Loop points for focused practice
- Speed adjustment without pitch change
- Practice session persistence

## Building

### Prerequisites

- CMake 3.15+
- C++17 compatible compiler
- Dear ImGui Bundle (handled automatically by CMake)

### Build Instructions

```bash
# Clone the repository
git clone <repository-url>
cd SongPractice

# Create build directory
mkdir build
cd build

# Configure and build
cmake ..
cmake --build .
```

## Usage

1. Launch the application
2. Use File -> Open Audio File to load a track
3. Use playback controls to practice with your audio
4. Set loop points by Shift+clicking on the waveform
5. Adjust tempo as needed for comfortable practice

## Development Status

This project is currently in early development. See [ROADMAP.md](ROADMAP.md) for development progress and planned features.

## License

TBD
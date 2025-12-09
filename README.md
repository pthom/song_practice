# SongPractice

A cross-platform desktop application designed specifically for singers to practice with audio tracks. Built with Dear ImGui Bundle for a responsive, professional interface.

## 🎵 Features

### Audio Playback
- **Multi-format support**: Load MP3 and WAV audio files
- **Interactive waveform display**: Visual representation with clickable timeline navigation
- **High-quality playback**: RtAudio-powered cross-platform audio engine
- **Precise seeking**: Click anywhere on the waveform to jump to that position

### Practice Tools
- **Markers system**: Add, edit, and navigate between practice markers
- **Smart marker navigation**: Intelligent previous/next marker behavior
- **Tempo adjustment**: Real-time speed control (25%-200%) without pitch change using SoundTouch
- **Transport controls**: Professional play/pause/stop/seek buttons with tooltips

### Session Management
- **Settings persistence**: Automatically saves and restores your practice sessions
- **Per-track settings**: Save/load custom marker and tempo configurations for individual songs
- **Global settings**: Remembers last opened file, playback position, and preferences
- **JSON-based storage**: Human-readable settings files with `.songpractice.json` extension

### User Experience
- **Keyboard shortcuts**: 
  - `Space` - Play/pause toggle
  - `Left/Right arrows` - Navigate between markers
- **Responsive UI**: Organized layout with audio info, controls, tempo, and marker sections
- **Visual feedback**: Color-coded time display, button states, and marker highlighting
- **Cross-platform**: Runs on Windows, macOS, and Linux

## 🚀 Getting Started

### Prerequisites

- **CMake 3.15+**
- **C++17 compatible compiler** (GCC 8+, Clang 7+, MSVC 2019+)
- **Git** (for dependency management)

All other dependencies (Dear ImGui Bundle, RtAudio, SoundTouch, nlohmann/json, dr_libs) are automatically fetched by CMake.

### Building from Source

```bash
# Clone the repository
git clone <repository-url>
cd SongPractice

# Create and enter build directory
mkdir cmake-build-release
cd cmake-build-release

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the application
cmake --build . --config Release

# Run the application
./SongPractice  # Linux/macOS
# or SongPractice.exe on Windows
```

### Quick Build (Debug)

```bash
mkdir cmake-build-debug && cd cmake-build-debug
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .
```

## 📖 How to Use

### Basic Workflow

1. **Load Audio**: `File → Open Audio File...` or `Ctrl+O`
2. **Navigate**: Click on the waveform or use transport buttons to move around
3. **Add Markers**: Click the `+` button to mark important sections at the current playback position
4. **Practice**: Use `Previous/Next Marker` buttons or arrow keys to jump between sections
5. **Adjust Tempo**: Slide the tempo control to slow down difficult passages
6. **Save Session**: `File → Save Track Settings...` to preserve your markers and tempo settings

### Keyboard Shortcuts

| Key | Action |
|-----|--------|
| `Space` | Play/Pause toggle |
| `←` | Previous marker |
| `→` | Next marker |

### Marker Management

- **Add markers** at the current playback position using the `+` button
- **Navigate markers** with dedicated buttons or arrow keys  
- **Edit markers** by modifying the name in the text field
- **Delete markers** using the delete button next to each marker
- **Smart navigation**: If you're close to a marker (within 1 second), "Previous" goes to the actual previous marker; if you're far, it goes to the current marker first

### Settings Files

- **Global settings**: `songpractice-settings.json` (auto-saved in executable directory)
- **Track settings**: Custom `.songpractice.json` files you can save/load manually
- **What's saved**: Markers, tempo settings, playback position, and associated audio file path

## 🛠️ Technical Details

### Architecture
- **Audio Engine**: RtAudio + dr_libs for cross-platform audio I/O and decoding
- **Tempo Processing**: SoundTouch library for high-quality time-stretching
- **UI Framework**: Dear ImGui Bundle with ImPlot for waveform visualization
- **Settings**: nlohmann/json for human-readable configuration files

### Supported Formats
- **Audio**: MP3, WAV (more formats planned)
- **Sample Rates**: Any rate supported by RtAudio (typically 44.1kHz, 48kHz, 96kHz, etc.)
- **Channels**: Mono and stereo

### System Requirements
- **OS**: Windows 10+, macOS 10.14+, Linux (Ubuntu 18.04+ or equivalent)
- **RAM**: 256MB minimum (depends on audio file size)
- **Audio**: Any RtAudio-compatible audio interface

## 🗺️ Development Status

SongPractice is feature-complete for basic practice sessions. See [ROADMAP.md](ROADMAP.md) for detailed development progress and future enhancements.

**Completed Features:**
- ✅ Audio file loading and playback
- ✅ Interactive waveform display  
- ✅ Transport controls with keyboard shortcuts
- ✅ Marker system with smart navigation
- ✅ Real-time tempo adjustment
- ✅ Settings persistence (global and per-track)

**Planned Enhancements:**
- 🔄 Additional audio format support (FLAC, OGG, etc.)
- 🔄 Loop regions for repeated section practice
- 🔄 Pitch adjustment independent of tempo
- 🔄 Metronome integration
- 🔄 Audio effects and EQ

## 🤝 Contributing

Contributions are welcome! Please feel free to submit issues, feature requests, or pull requests.

## 📄 License

TBD
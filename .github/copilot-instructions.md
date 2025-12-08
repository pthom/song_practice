# Copilot Instructions for SongPractice

## Project Overview
SongPractice is a cross-platform desktop application for singers to practice with audio tracks. Built using Dear ImGui Bundle for UI, targeting Windows/macOS/Linux with audio processing capabilities. Speed/pitch adjustment can be implemented as offline processing operations.

## Architecture Patterns

### Core Components
- **AudioEngine**: Handle playback, speed/pitch adjustment, format support (mp3, wav)
- **WaveformRenderer**: Visual representation with draggable cursor, ImGui-based drawing
- **LoopManager**: Set/manage practice loop points with precise timing
- **SettingsManager**: Persist user preferences (loop points, speed, pitch) between sessions
- **UI Layer**: ImGui immediate mode interface with custom audio-specific widgets

### Key Technical Requirements
- Cross-platform audio backend using **RtAudio** (simplest multiplatform option)
- Efficient waveform visualization for large audio files using ImGui custom drawing
- Precise timing for loop points during playback
- Non-blocking UI during tempo/pitch processing operations (show progress dialog)
- Audio format support via **libsndfile** or **dr_libs** (header-only)

## Development Conventions

### File Organization
```
src/
├── audio/          # AudioEngine, format handlers, DSP
├── ui/             # ImGui windows, custom widgets
├── core/           # LoopManager, SettingsManager, utilities  
├── platform/       # OS-specific implementations
└── external/       # Third-party libraries (ImGui, audio libs)
```

### Dear ImGui Bundle Patterns
- Use ImGui Bundle's integrated backends (SDL2/OpenGL3 recommended)
- Leverage ImPlot for waveform visualization with custom data callbacks
- Use ImGui::DragFloat for tempo adjustment, ImGui::SliderFloat for seeking
- Implement file dialog using ImGui Bundle's portable-file-dialogs integration
- Custom drawing with ImDrawList for waveform cursor and loop markers

### Audio Processing Guidelines
- **Playback**: Use RtAudio callback for simple playback (separate thread)
- **Speed/Pitch**: Process offline with progress dialog - use SoundTouch or similar
- **Loop Points**: Store as sample positions, convert to time for UI display
- **Threading**: Only playback needs real-time thread, tempo processing can block UI with progress
- Handle different sample rates by resampling during load (libsamplerate)

### Settings Persistence
- JSON-based settings files in user config directory
- Per-track settings: loop points, speed, pitch adjustments
- Global settings: UI layout, default parameters, audio device selection
- Auto-save settings during playback to prevent data loss

## Critical Dependencies
- **UI Framework**: Dear ImGui Bundle (includes SDL2, OpenGL3, ImPlot, portable-file-dialogs)
- **Audio I/O**: RtAudio (header-only, excellent cross-platform support)
- **Audio Decoding**: libsndfile (robust) or dr_wav/dr_mp3 (header-only, simpler)
- **Speed/Pitch Processing**: SoundTouch library (proven, cross-platform)
- **Resampling**: libsamplerate (if needed for format compatibility)

## Build & Development
- **Build System**: CMake with Dear ImGui Bundle's find_package integration
- **Dependencies**: Use vcpkg for libsndfile/SoundTouch, RtAudio as git submodule
- **Development**: Debug builds work fine - no real-time constraints for tempo processing
- **Platform Testing**: RtAudio handles driver differences, but test file formats early
- **Example CMake**: `find_package(imgui-bundle REQUIRED)` + `target_link_libraries(... imgui-bundle::imgui-bundle)`

## Key User Workflows
1. **Load Track**: Drag-drop or file browser → decode → generate waveform visualization
2. **Set Loop**: Click/drag on waveform to set start/end points → visual feedback
3. **Practice Session**: Adjust speed/pitch → loop playback → save settings automatically
4. **Resume Practice**: Load previous session settings → continue where left off

## Testing Considerations  
- Test with various audio formats and sample rates
- Verify loop points are sample-accurate across different playback speeds
- Test UI responsiveness during heavy audio processing
- Cross-platform audio device compatibility testing essential
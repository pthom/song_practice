# Copilot Instructions for SongPractice

## Project Overview
SongPractice is a desktop tool for singers to practice over pre-recorded audio tracks (mp3, wav). The application provides:

**Core Features:**
- Visual waveform display with draggable playback cursor for precise navigation
- Basic playback controls (play, pause, stop, seek, advance by 1/5/10 seconds)
- Loop points for focused practice on difficult sections
- Playback speed adjustment without pitch change for manageable tempo practice
- Save/load practice sessions (loop points, speed settings)
- Optional: Pitch adjustment and metronome features

**Technical Stack:** Dear ImGui Bundle + RtAudio for cross-platform desktop deployment (Windows/macOS/Linux).

## Architecture Patterns

### Core Components
- **AudioEngine**: Handle playback, speed/pitch adjustment, format support (mp3, wav)
- **WaveformRenderer**: Visual representation with draggable cursor, ImGui-based drawing
- **LoopManager**: Set/manage practice loop points with precise timing
- **SettingsManager**: Persist user preferences (loop points, speed, pitch) between sessions
- **UI Layer**: ImGui immediate mode interface with custom audio-specific widgets

### Key Technical Requirements
- **Waveform Visualization**: Render large audio files efficiently using ImGui/ImPlot with zoom/scroll
- **Precise Playback Control**: Sample-accurate seeking and loop points for practice sessions
- **Offline Processing**: Speed/pitch adjustment as background operations with progress indication
- **Session Persistence**: Auto-save practice settings per track to prevent data loss
- **Cross-platform Audio**: RtAudio handles platform differences (ASIO/DirectSound/CoreAudio/ALSA)

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

## Essential User Workflows
1. **Track Loading**: File dialog or drag-drop → decode audio → generate waveform display with time markers
2. **Navigation**: Click waveform to jump to position, drag cursor for scrubbing, use +/-1/5/10s buttons
3. **Loop Setup**: Shift+click to set start point, Shift+click again for end → visual loop region highlight
4. **Speed Practice**: Adjust tempo slider (50%-150%) → audio processes in background → loop playback at new speed
5. **Session Management**: Settings auto-save on changes, restore previous loop/speed when reopening same track

## Testing Considerations  
- Test with various audio formats and sample rates
- Verify loop points are sample-accurate across different playback speeds
- Test UI responsiveness during heavy audio processing
- Cross-platform audio device compatibility testing essential
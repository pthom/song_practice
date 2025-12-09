# SongPractice Development Strategy

## Project Roadmap

This document outlines the development strategy for SongPractice, broken down into manageable tasks that follow a logical progression from basic functionality to advanced features.

## Development Phases

### Phase 1: Foundation (Tasks 1-3)
**Goal**: Establish core infrastructure and basic audio playback

1. **Project Setup & Basic Structure**
   - Initialize CMake project with Dear ImGui Bundle
   - Set up basic application window and main loop
   - Include RtAudio as dependency and create basic folder structure (src/audio, src/ui, src/core)

2. **Audio File Loading & Decoding**
   - Implement file dialog for audio selection and basic audio decoding using libsndfile or dr_libs
   - Load audio data into memory buffer, support mp3/wav formats

3. **Basic Audio Playback Engine**
   - Create AudioEngine class with RtAudio callback for simple playback
   - Implement play/pause/stop functionality with thread-safe state management

### Phase 2: User Interface (Tasks 4-6)
**Goal**: Create interactive waveform display and basic controls

4. **Waveform Visualization**
   - Use ImPlot to render audio waveform with time axis
   - Downsample large audio files for efficient display

5. **Playback Position Indicator**
   - Add draggable cursor on waveform showing current playback position
   - Implement cursor dragging for scrubbing through audio

6. **Basic Playback Controls UI**
   - Create ImGui controls for play/pause buttons
   - Add seek buttons (using ImGui::PushItemFlag(ImGuiItemFlags_ButtonRepeat, true) so that buttons can be held down)
   - Present controls as a single horizontal row with generous spacing so singers can hit buttons quickly, even during practice sessions
   - Use icon-enabled buttons (Font Awesome via Hello ImGui) with hovered tooltips and disabled states that reflect the AudioEngine (e.g., greyed-out Pause when nothing is playing)
   - The play button should toggle to a pause icon when playing, and vice versa
   - Add a button to stop playback and reset position to start
  - Add current time display with total duration
  - Show transport status inline (e.g., "2:13 / 5:05") and color it differently while recording/playing in later phases

### Phase 3: Practice Features (Tasks 7-8)
**Goal**: Implement core practice functionality

- Pressing the space bar should toggle play/pause functionality

7. **Markers**
   - Provide an "Add Marker" button that drops a marker at the current playback position; markers are stored per track once SettingsManager is available
   - Render markers directly on the waveform using ImPlot legends; the marker immediately preceding the playhead becomes the "current" marker and is highlighted with a distinct color in the plot
   - Place the marker legend inside the plot area (ImPlot legend widgets) so singers can see labels without shifting their focus away from the waveform
   - Expose inline edit controls (no popup dialogs) for the current marker so users can rename, delete, or nudge its timestamp without leaving the main UI
   - Add previous/next marker buttons that always seek to adjacent markers; if no markers exist, these buttons remain disabled until separate time-based seek controls are introduced
   - Keep the stop button in place for now; keyboard shortcuts (arrows for marker navigation) will be tackled in a later iteration once the marker workflow is stable

8. **Settings Persistence**
   - Implement persistence for ApplicationState, with JSON serialization using nlohmann/json
   - Save and load global settings (last opened file, markers, play position) on application start/exit
   - Add per-track settings functionality with manual load/save via menu items:
     - "Load Track Settings..." menu item that opens a file dialog to select a .songpractice.json file
     - "Save Track Settings..." menu item that opens a file dialog to save current markers and settings
     - Per-track settings include: markers, play position, tempo adjustment, and associated audio file path
     - Use portable-file-dialogs (pfd) for cross-platform file selection
     - Settings files use .songpractice.json extension for easy identification

### Phase 4: Advanced Features (Tasks 9-10)
**Goal**: Add tempo control and polish the application

9. **Speed Adjustment (Real-time)**
   - Integrate SoundTouch library for high-quality tempo adjustment without pitch change
   - Implement real-time processing with sliding window approach for immediate feedback
   - Create separate audio processing thread to maintain UI responsiveness and prevent audio dropouts
   - Add tempo slider UI control with range 50%-150% (0.5x to 1.5x speed)
   - Integrate tempo setting into ApplicationState for persistence across sessions
   - Handle tempo changes during playback with smooth transitions
   - Optimize buffering strategy to balance latency and audio quality
   - Add visual tempo indicator in the transport controls area

10. **Polish & Testing**
    - Add error handling for unsupported formats
    - Test cross-platform compatibility
    - Implement keyboard shortcuts:
      - Space bar for play/pause toggle
      - Left/Right arrow keys for marker navigation (previous/next marker)
    - Improve UI responsiveness and user experience

## Task Dependencies

```
Task 1 (Setup) → Task 2 (Loading) → Task 3 (Playback)
                                          ↓
Task 4 (Waveform) ← ← ← ← ← ← ← ← ← ← ← ← ← 
    ↓
Task 5 (Cursor) → Task 6 (Controls)
    ↓
Task 7 (Loops) → Task 8 (Settings)

Task 9 (Speed) can start after Task 3
Task 10 (Polish) spans all tasks
```

## Development Notes

- **Parallel Development**: Tasks 4-6 can be developed in parallel after Task 3
- **Independent Features**: Task 9 (Speed Adjustment) can be developed independently after Task 3
- **Iterative Testing**: Each task delivers a working feature that can be tested independently
- **MVP Milestone**: After Task 6, you have a functional audio player
- **Practice Tool Milestone**: After Task 8, you have a complete practice tool

## Success Criteria

- **Phase 1**: Can load and play audio files with basic controls
- **Phase 2**: Interactive waveform with seeking and visual feedback
- **Phase 3**: Full practice session workflow (loops + persistence)
- **Phase 4**: Professional-quality tool ready for distribution

Each phase builds upon the previous one while delivering tangible user value.
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
   - Add zoom and scroll capabilities for navigation

5. **Playback Position Indicator**
   - Add draggable cursor on waveform showing current playback position
   - Implement click-to-seek and cursor dragging for scrubbing through audio

6. **Basic Playback Controls UI**
   - Create ImGui controls for play/pause/stop buttons, seek buttons (+/-1/5/10 seconds)
   - Add current time display with total duration

### Phase 3: Practice Features (Tasks 7-8)
**Goal**: Implement core practice functionality

7. **Loop Points System**
   - Implement LoopManager class to set start/end loop points via Shift+click on waveform
   - Add visual loop region highlighting and loop playback logic

8. **Settings Persistence**
   - Create SettingsManager with JSON serialization for per-track settings (loop points, speed)
   - Auto-save settings on changes, restore on track reload

### Phase 4: Advanced Features (Tasks 9-10)
**Goal**: Add tempo control and polish the application

9. **Speed Adjustment (Offline)**
   - Integrate SoundTouch library for tempo adjustment without pitch change
   - Implement background processing with progress dialog for 50%-150% speed range

10. **Polish & Testing**
    - Add error handling for unsupported formats
    - Test cross-platform compatibility
    - Improve UI responsiveness, add keyboard shortcuts for common actions

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
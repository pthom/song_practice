cmake_minimum_required(VERSION 3.15)
project(SongPractice)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Find Dear ImGui Bundle
FetchContent_Declare(
    imgui_bundle 
    GIT_REPOSITORY https://github.com/pthom/imgui_bundle.git 
    GIT_TAG main)
FetchContent_MakeAvailable(imgui_bundle)

# Add RtAudio as external dependency
FetchContent_Declare(
    rtaudio
    GIT_REPOSITORY https://github.com/thestk/rtaudio.git
    GIT_TAG 6.0.1
)
FetchContent_MakeAvailable(rtaudio)

# Header-only audio decoding utilities (dr_wav, dr_mp3)
FetchContent_Declare(
    dr_libs
    GIT_REPOSITORY https://github.com/mackron/dr_libs.git
    GIT_TAG master
)
FetchContent_MakeAvailable(dr_libs)

# Source files
set(SOURCES
    src/main.cpp
    src/ui/MainWindow.cpp
    src/ui/MainWindow.h
    src/audio/AudioEngine.cpp
    src/audio/AudioEngine.h
    src/core/Utils.cpp
    src/core/Utils.h
)

# Create executable
imgui_bundle_add_app(${PROJECT_NAME} ${SOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
    rtaudio
)

# Include directories
target_include_directories(${PROJECT_NAME} 
    PRIVATE 
    src
    external
    ${dr_libs_SOURCE_DIR}
    ${rtaudio_SOURCE_DIR}
)

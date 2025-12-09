#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "imgui.h"

class WaveformRenderer
{
public:
    WaveformRenderer() = default;
    void clear();
    void setWaveform(const std::vector<float>& interleavedSamples,
                     uint32_t channelCount,
                     uint32_t sampleRate);
    bool hasWaveform() const;
    void draw(const char* plotId,
              const ImVec2& size,
              float currentTimeSeconds) const;

private:
    struct ChannelEnvelope
    {
        std::vector<float> minValues;
        std::vector<float> maxValues;
    };

    struct WaveformLevel
    {
        uint32_t samplesPerBucket = 0;
        std::vector<float> times;
        std::vector<ChannelEnvelope> channels;
    };

    WaveformLevel buildLevel(const std::vector<float>& interleavedSamples,
                             uint32_t channelCount,
                             uint32_t sampleRate,
                             uint32_t samplesPerBucket) const;
    const WaveformLevel* pickLevel(float samplesPerPixel) const;

    uint32_t m_channelCount = 0;
    uint32_t m_sampleRate = 0;
    uint64_t m_frameCount = 0;
    float m_durationSeconds = 0.0f;
    std::vector<WaveformLevel> m_levels;
    std::vector<uint32_t> m_bucketTargets = {64, 256, 1024, 4096, 16384};
};


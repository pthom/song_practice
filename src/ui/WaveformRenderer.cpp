#include "WaveformRenderer.h"

#include <algorithm>
#include <cmath>
#include "implot/implot.h"


void WaveformRenderer::clear()
{
    m_channelCount = 0;
    m_sampleRate = 0;
    m_frameCount = 0;
    m_durationSeconds = 0.0f;
    m_levels.clear();
}

void WaveformRenderer::setWaveform(const std::vector<float>& interleavedSamples,
                                   uint32_t channelCount,
                                   uint32_t sampleRate)
{
    clear();

    if (interleavedSamples.empty() || channelCount == 0 || sampleRate == 0)
        return;

    m_channelCount = channelCount;
    m_sampleRate = sampleRate;
    m_frameCount = static_cast<uint64_t>(interleavedSamples.size()) / channelCount;
    m_durationSeconds = static_cast<float>(m_frameCount) / static_cast<float>(sampleRate);

    for (uint32_t samplesPerBucket : m_bucketTargets)
    {
        if (samplesPerBucket > m_frameCount)
            break;
        m_levels.push_back(buildLevel(interleavedSamples, channelCount, sampleRate, samplesPerBucket));
    }

    if (m_levels.empty())
    {
        const uint32_t bucketSize = static_cast<uint32_t>(std::max<uint64_t>(1, m_frameCount / 512));
        m_levels.push_back(buildLevel(interleavedSamples, channelCount, sampleRate, bucketSize));
    }
}

bool WaveformRenderer::hasWaveform() const
{
    return !m_levels.empty();
}

bool WaveformRenderer::draw(const char* plotId,
                            const ImVec2& size,
                            float currentTimeSeconds,
                            float& outSeekTimeSeconds,
                            const std::vector<MarkerView>& markers) const
{
    outSeekTimeSeconds = currentTimeSeconds;

    if (!hasWaveform())
        return false;

    bool dragged = false;

    ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, ImVec2(10.0f, 6.0f));
    const ImPlotFlags plotFlags = ImPlotFlags_CanvasOnly | ImPlotFlags_NoMenus;
    if (ImPlot::BeginPlot(plotId, size, plotFlags))
    {
        ImPlotAxisFlags axisFlags = ImPlotAxisFlags_NoHighlight | ImPlotAxisFlags_Lock;
        ImPlot::SetupAxisLimits(ImAxis_X1, 0.0, m_durationSeconds, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, -1.0, 1.0, ImGuiCond_Always);
        ImPlot::SetupAxes(nullptr, nullptr, axisFlags, axisFlags);

        const ImPlotRect limits = ImPlot::GetPlotLimits();
        const float visibleDuration = static_cast<float>(limits.X.Max - limits.X.Min);
        const float plotPixelWidth = ImGui::GetWindowDrawList()->GetClipRectMax().x - ImGui::GetWindowDrawList()->GetClipRectMin().x;
        const float samplesPerPixel = (visibleDuration <= 0.0f || plotPixelWidth <= 0.0f)
                                          ? static_cast<float>(m_frameCount)
                                          : (visibleDuration * m_sampleRate) / plotPixelWidth;
        const WaveformLevel* level = pickLevel(samplesPerPixel);
        if (level != nullptr)
        {
            for (uint32_t channel = 0; channel < level->channels.size(); ++channel)
            {
                const ChannelEnvelope& envelope = level->channels[channel];
                const std::string label = (m_channelCount > 1)
                                              ? "Channel " + std::to_string(channel + 1)
                                              : "Waveform";
                ImPlot::PushStyleColor(ImPlotCol_Line, IM_COL32(100, 180, 255, 255));
                ImPlot::PlotShaded(label.c_str(),
                                   level->times.data(),
                                   envelope.minValues.data(),
                                   envelope.maxValues.data(),
                                   static_cast<int>(level->times.size()));
                ImPlot::PopStyleColor();
            }
        }

        for (const auto& marker : markers)
        {
            double markerX = marker.timeSeconds;
            const ImVec4 color = ImVec4(0.6f, 0.8f, 1.0f, 1.0f);
            const std::string id = "Marker##" + marker.label;
            ImPlot::TagX(markerX, color, "%s", marker.label.c_str());
            ImPlot::DragLineX(ImGui::GetID(id.c_str()),
                                  &markerX,
                                  color,
                                  1.0f,
                                  ImPlotDragToolFlags_NoInputs);
        }

        double cursorValue = static_cast<double>(currentTimeSeconds);
        const ImPlotDragToolFlags cursorFlags = ImPlotDragToolFlags_NoFit;
        bool clicked = false;
        bool hovered = false;
        bool held = false;
        if (ImPlot::DragLineX(ImGui::GetID("PlaybackCursor"),
                              &cursorValue,
                              ImVec4(1.0f, 0.35f, 0.2f, 1.0f),
                              2.0f,
                              cursorFlags,
                              &clicked,
                              &hovered,
                              &held))
        {
            dragged = true;
            outSeekTimeSeconds = static_cast<float>(cursorValue);
        }

        ImPlot::EndPlot();
    }
    ImPlot::PopStyleVar();

    return dragged;
}

WaveformRenderer::WaveformLevel WaveformRenderer::buildLevel(const std::vector<float>& interleavedSamples,
                                                             uint32_t channelCount,
                                                             uint32_t sampleRate,
                                                             uint32_t samplesPerBucket) const
{
    WaveformLevel level;
    level.samplesPerBucket = samplesPerBucket;
    const uint64_t bucketCount = (m_frameCount + samplesPerBucket - 1) / samplesPerBucket;
    level.times.resize(bucketCount);
    level.channels.resize(channelCount);

    for (uint32_t channel = 0; channel < channelCount; ++channel)
    {
        ChannelEnvelope& envelope = level.channels[channel];
        envelope.minValues.resize(bucketCount);
        envelope.maxValues.resize(bucketCount);
    }

    for (uint64_t bucketIndex = 0; bucketIndex < bucketCount; ++bucketIndex)
    {
        const uint64_t frameStart = bucketIndex * samplesPerBucket;
        const uint64_t frameEnd = std::min<uint64_t>(frameStart + samplesPerBucket, m_frameCount);
        const uint64_t framesInBucket = frameEnd - frameStart;
        if (framesInBucket == 0)
            continue;

        level.times[bucketIndex] = static_cast<float>(frameStart) / static_cast<float>(sampleRate);

        for (uint32_t channel = 0; channel < channelCount; ++channel)
        {
            float minSample = 1.0f;
            float maxSample = -1.0f;
            const uint64_t channelOffset = channel;
            const uint64_t startIndex = (frameStart * channelCount) + channelOffset;
            for (uint64_t frame = 0; frame < framesInBucket; ++frame)
            {
                const float sample = interleavedSamples[startIndex + frame * channelCount];
                minSample = std::min(minSample, sample);
                maxSample = std::max(maxSample, sample);
            }
            level.channels[channel].minValues[bucketIndex] = minSample;
            level.channels[channel].maxValues[bucketIndex] = maxSample;
        }
    }

    return level;
}

const WaveformRenderer::WaveformLevel* WaveformRenderer::pickLevel(float samplesPerPixel) const
{
    if (m_levels.empty())
        return nullptr;

    const WaveformLevel* bestLevel = &m_levels.front();
    float bestDifference = std::abs(static_cast<float>(bestLevel->samplesPerBucket) - samplesPerPixel);

    for (const WaveformLevel& level : m_levels)
    {
        const float difference = std::abs(static_cast<float>(level.samplesPerBucket) - samplesPerPixel);
        if (difference < bestDifference)
        {
            bestDifference = difference;
            bestLevel = &level;
        }
    }

    return bestLevel;
}

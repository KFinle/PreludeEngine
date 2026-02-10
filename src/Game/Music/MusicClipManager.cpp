#include "MusicClipManager.h"

#include <cstdint>

#include "Engine/Engine.h"
#include "Audio/Music/Render/EventSequenceRenderer.h"
#include "Audio/Music/Render/RenderSettings.h"
#include "Debug/DebugLogger.h"

RenderedSequence MusicClipManager::RenderSequence(const std::string& id, const EventSequence& seq)
{
    RenderSettings settings;
    settings.sample_rate = 48000;
    settings.tail_seconds = 0.25f;

    auto buffer = EventSequenceRenderer::RenderToBuffer(seq, settings);
    Logger::PrintLog(Logger::MUSIC, "BPM: " + std::to_string(seq.bpm));

    const uint32_t sample_rate = static_cast<uint32_t>(settings.sample_rate);
    const uint32_t channels = 1;
    const uint64_t frames = static_cast<uint64_t>(buffer.size());

    const std::string sound_id = id;
    Engine::LoadAudioPCM(sound_id.c_str(), buffer.data(), frames, channels, sample_rate);

    RenderedSequence clip;
    clip.id = id;
    clip.sound_id = sound_id;
    clip.filepath.clear();
    clip.length_sec = (settings.sample_rate > 0) ? (static_cast<float>(buffer.size()) / static_cast<float>(settings.sample_rate)) : 0.0f;

    m_clips[id] = clip;
    return clip;
}

void MusicClipManager::Play(const std::string& id, const bool loop)
{
    auto iterator = m_clips.find(id);
    if (iterator == m_clips.end())
    {
        Logger::PrintLog(Logger::MUSIC, "Play failed, not found: " + id);
        return;
    }
    Engine::PlayAudio(iterator->second.sound_id.c_str(), loop);
}

void MusicClipManager::Stop(const std::string& id)
{
    auto iterator = m_clips.find(id);
    if (iterator == m_clips.end())
    {
        Logger::PrintLog(Logger::MUSIC, "Stop failed: " + id);
        return;
    }
    Engine::StopAudio(iterator->second.sound_id.c_str());
}


void MusicClipManager::CleanupGeneratedFiles()
{
    for (auto& pair : m_clips)
    {
        const auto& clip = pair.second;
        if (clip.sound_id.empty()) continue;
        Engine::StopAudio(clip.sound_id.c_str());
        Engine::UnloadAudio(clip.sound_id.c_str());
    }
    m_clips.clear();
}

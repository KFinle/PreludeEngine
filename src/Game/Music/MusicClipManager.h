#pragma once
#include <unordered_map>
#include <string>

#include "Audio/Music/Events/EventSequence.h"
#include "RenderedSequence.h"

//////////////////////////////////////////////////////////////////////////
// Responsible for generating, playing, stopping, and cleaning up songs //
//////////////////////////////////////////////////////////////////////////
class MusicClipManager
{
public:
    MusicClipManager() = default;

    // renders and caches: id -> filepath
    RenderedSequence RenderSequence(const std::string& id, const EventSequence& seq);

    // plays a cached id
    void Play(const std::string& id, bool loop = false);

    // deletes all files generated through this manager
    void CleanupGeneratedFiles();

    void Stop(const std::string& id);

private:
    std::unordered_map<std::string, RenderedSequence> m_clips;
};

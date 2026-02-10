#pragma once
#include <atomic>
#include <string>
#include <thread>
#include <mutex>
#include "Audio/Music/Events/EventSequence.h"


////////////////////////////////////////////////////
// AsyncSongRender                                //
// - rendering happens off the main thread        //
// - playback and cleanup stay on the main thread //
////////////////////////////////////////////////////
class MusicClipManager;
class AsyncSongRender
{
public:
    AsyncSongRender() = default;
    ~AsyncSongRender();

    void Start(MusicClipManager* music, std::string song_id, EventSequence seq);
    bool IsDone() const { return m_done.load(); }
    bool Succeeded() const { return m_ok.load(); }
    std::string GetError() const;
    void Join();

private:
    void ThreadMain();

private:
    MusicClipManager* m_music = nullptr;
    std::string m_song_id;
    EventSequence m_seq;
    std::thread m_thread;
    std::atomic<bool> m_done{false};
    std::atomic<bool> m_ok{false};
    mutable std::mutex m_err_mutex;
    std::string m_error;
};

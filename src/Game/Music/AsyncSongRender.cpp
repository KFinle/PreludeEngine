#include "AsyncSongRender.h"
#include "Music/MusicClipManager.h"

AsyncSongRender::~AsyncSongRender()
{
    Join();
}

void AsyncSongRender::Start(MusicClipManager* music, std::string song_id, EventSequence seq)
{
    // don't allow double start
    if (m_thread.joinable()) return;

    m_music = music;
    m_song_id = std::move(song_id);
    m_seq = std::move(seq);
    m_done.store(false);
    m_ok.store(false);
    m_thread = std::thread(&AsyncSongRender::ThreadMain, this);
}

void AsyncSongRender::Join()
{
    if (m_thread.joinable()) m_thread.join();
}

std::string AsyncSongRender::GetError() const
{
    std::lock_guard lock(m_err_mutex);
    return m_error;
}

void AsyncSongRender::ThreadMain()
{
    bool ok = true;

    try
    {
        m_music->RenderSequence(m_song_id, m_seq);
    }
    catch (const std::exception& e)
    {
        ok = false;
        std::lock_guard lock(m_err_mutex);
        m_error = e.what();
    }
    catch (...)
    {
        ok = false;
        std::lock_guard lock(m_err_mutex);
        m_error = "Unknown error while rendering song";
    }

    m_ok.store(ok);
    m_done.store(true);
}

#include "AudioPlayer.h"

#include <cassert>

#include "miniaudio/miniaudio.h"

namespace Engine
{
    static bool HasFlag(const SoundFlags flags, const SoundFlags test)
    {
        return (static_cast<unsigned>(flags) & static_cast<unsigned>(test)) != 0;
    }

    AudioPlayer& AudioPlayer::Get()
    {
        static AudioPlayer instance;
        return instance;
    }

    AudioPlayer::AudioPlayer() = default;

    AudioPlayer::~AudioPlayer()
    {
        if (m_initialized)
        {
            Shutdown();
        }
    }

    bool AudioPlayer::Initialize()
    {
        if (m_initialized) return true;

        m_engine = new ma_engine();
        const ma_result result = ma_engine_init(nullptr, m_engine);
        assert(result == MA_SUCCESS);

        m_initialized = (result == MA_SUCCESS);
        return m_initialized;
    }

    void AudioPlayer::ClearSounds()
    {
        for (auto& pair : m_sounds)
        {
            ma_sound_uninit(pair.second.sound);
            delete pair.second.sound;

            if (pair.second.buffer)
            {
                ma_audio_buffer_uninit_and_free(reinterpret_cast<ma_audio_buffer*>(pair.second.buffer));
            }
        }
        m_sounds.clear();
    }

    void AudioPlayer::Shutdown()
    {
        if (!m_initialized) return;

        ClearSounds();

        ma_engine_uninit(m_engine);
        delete m_engine;
        m_engine = nullptr;

        m_initialized = false;
    }

    bool AudioPlayer::LoadFile(const char* filename)
    {
        if (!m_initialized) return false;

        if (m_sounds.find(filename) != m_sounds.end()) return true;

        ma_sound* sound = new ma_sound();
        const ma_result result = ma_sound_init_from_file(m_engine, filename, 0, nullptr, nullptr, sound);
        assert(result == MA_SUCCESS);
        if (result != MA_SUCCESS)
        {
            delete sound;
            return false;
        }

        SoundEntry entry;
        entry.sound = sound;
        entry.buffer = nullptr;

        m_sounds[filename] = entry;
        return true;
    }

    bool AudioPlayer::Play(const char* filename, const SoundFlags flags)
    {
        if (!m_initialized) return false;

        auto it = m_sounds.find(filename);
        if (it == m_sounds.end())
        {
            if (!LoadFile(filename)) return false;
            it = m_sounds.find(filename);
            if (it == m_sounds.end()) return false;
        }

        ma_sound* sound = it->second.sound;

        (void)Stop(filename);
        (void)ma_sound_seek_to_pcm_frame(sound, 0);
        ma_sound_set_looping(sound, HasFlag(flags, SoundFlags::Looping));

        return ma_sound_start(sound) == MA_SUCCESS;
    }

    bool AudioPlayer::Stop(const char* filename)
    {
        if (!m_initialized) return false;

        auto it = m_sounds.find(filename);
        if (it == m_sounds.end()) return false;

        if (ma_sound_is_playing(it->second.sound))
        {
            return ma_sound_stop(it->second.sound) == MA_SUCCESS;
        }

        return false;
    }

    bool AudioPlayer::IsPlaying(const char* filename) const
    {
        if (!m_initialized) return false;

        auto it = m_sounds.find(filename);
        if (it == m_sounds.end()) return false;

        return ma_sound_is_playing(it->second.sound);
    }

    bool AudioPlayer::Unload(const char* id)
    {
        if (!m_initialized) return false;
        if (!id) return false;

        auto it = m_sounds.find(id);
        if (it == m_sounds.end()) return false;

        (void)ma_sound_stop(it->second.sound);
        ma_sound_uninit(it->second.sound);
        delete it->second.sound;

        if (it->second.buffer)
        {
            ma_audio_buffer_uninit_and_free(reinterpret_cast<ma_audio_buffer*>(it->second.buffer));
        }

        m_sounds.erase(it);
        return true;
    }

    bool AudioPlayer::PlayPcmF32(const char* id,
                                const float* interleaved_samples,
                                const uint64_t frame_count,
                                const uint32_t channels,
                                const uint32_t sample_rate,
                                const SoundFlags flags)
    {
        if (!m_initialized) return false;
        if (!id) return false;
        if (!interleaved_samples) return false;
        if (frame_count == 0) return false;
        if (channels == 0) return false;
        if (sample_rate == 0) return false;

        (void)flags;
        (void)Unload(id);

        ma_audio_buffer_config buffer_config = ma_audio_buffer_config_init(
            ma_format_f32,
            static_cast<ma_uint32>(channels),
            static_cast<ma_uint64>(frame_count),
            interleaved_samples,
            nullptr
        );
        buffer_config.sampleRate = static_cast<ma_uint32>(sample_rate);

        ma_audio_buffer* buffer = nullptr;
        const ma_result buffer_result = ma_audio_buffer_alloc_and_init(&buffer_config, &buffer);
        assert(buffer_result == MA_SUCCESS);
        if (buffer_result != MA_SUCCESS)
        {
            return false;
        }

        ma_sound* sound = new ma_sound();
        const ma_result sound_result = ma_sound_init_from_data_source(
            m_engine,
            reinterpret_cast<ma_data_source*>(&buffer->ref),
            0,
            nullptr,
            sound
        );
        assert(sound_result == MA_SUCCESS);
        if (sound_result != MA_SUCCESS)
        {
            ma_audio_buffer_uninit_and_free(buffer);
            delete sound;
            return false;
        }

        SoundEntry entry;
        entry.sound = sound;
        entry.buffer = buffer;
        m_sounds[id] = entry;
        return true;
    }
}

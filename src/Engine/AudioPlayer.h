#pragma once

#include <cstdint>
#include <map>
#include <string>

struct ma_engine;
struct ma_sound;

namespace Engine
{
    enum class SoundFlags : unsigned
    {
        None = 0,
        Looping = 1u << 1u,
    };

    class AudioPlayer
    {
    public:
        static AudioPlayer& Get();

        bool Initialize();
        void Shutdown();

        bool Play(const char* filename, SoundFlags flags);
        bool Stop(const char* filename);
        bool IsPlaying(const char* filename) const;

        bool PlayPcmF32(const char* id,
                        const float* interleaved_samples,
                        uint64_t frame_count,
                        uint32_t channels,
                        uint32_t sample_rate,
                        SoundFlags flags);

        bool Unload(const char* id);

    private:
        AudioPlayer();
        ~AudioPlayer();

        bool LoadFile(const char* filename);
        void ClearSounds();

        struct SoundEntry
        {
            ma_sound* sound = nullptr;
            void* buffer = nullptr;
        };

        ma_engine* m_engine = nullptr;
        std::map<std::string, SoundEntry> m_sounds;
        bool m_initialized = false;
    };
}

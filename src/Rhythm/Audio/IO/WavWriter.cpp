#include "WavWriter.h"
#include <cstdio>
#include <algorithm>
#include <cstdint>

namespace AudioIO
{
//https://www.mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html
////////////////////////////////////////////////////////////////////////////////    
// WAV File Layout:                                                           //
// 1. RIFF Header (12 bytes):                                                 //
//   - "RIFF" (4 bytes)                                                       //
//   - File size - 8 (4 bytes)                                                //
//   - "WAVE" (4 bytes)                                                       //
// 2. Format Chunk (24 bytes):                                                //
//   - "fmt " (4 bytes)                                                       //
//   - Chunk size (4 bytes, 16 for PCM)                                       //
//   - Audio format (2 bytes, 1 = PCM)                                        //
//   - Num channels (2 bytes)                                                 //
//   - Sample rate (4 bytes)                                                  //
//   - Byte rate (4 bytes, sample_rate * num_channels * bits_per_sample/8)    //
//   - Block align (2 bytes, num_channels * bits_per_sample/8)                //
//   - Bits per sample (2 bytes)                                              //
// 3. Data Chunk (8 + data bytes):                                            //
//   - "data" (4 bytes)                                                       //
//   - Data size (4 bytes, num_samples * num_channels * bits_per_sample/8)    //
//   - Audio samples (data_size bytes)                                        //
////////////////////////////////////////////////////////////////////////////////    
    bool WriteWav(const std::string& path, const std::vector<float>& samples, const WavConfig& cfg)
    {
        if (cfg.num_channels <= 0)      return false;
        if (cfg.bits_per_sample != 16)  return false;

        FILE* file = std::fopen(path.c_str(), "wb");
        if (!file) return false;

        constexpr uint16_t audio_format = 1;
        const uint16_t num_channels = static_cast<uint16_t>(cfg.num_channels);
        const uint32_t sample_rate = static_cast<uint32_t>(cfg.sample_rate);
        const uint16_t bits_per_sample = static_cast<uint16_t>(cfg.bits_per_sample);
        const uint16_t block_align = num_channels * bits_per_sample / 8;
        const uint32_t byte_rate = sample_rate * block_align;

        const uint32_t num_frames = static_cast<uint32_t>(samples.size() / num_channels);
        const uint32_t data_size = num_frames * block_align;
        constexpr uint32_t fmt_chunk_size = 16;
        const uint32_t riff_chunk_size = 4 + (8 + fmt_chunk_size) + (8 + data_size);

        // RIFF header
        (void)std::fwrite("RIFF", 1, 4, file);
        (void)std::fwrite(&riff_chunk_size, 4, 1, file);
        (void)std::fwrite("WAVE", 1, 4, file);

        // fmt chunk
        (void)std::fwrite("fmt ", 1, 4, file);
        (void)std::fwrite(&fmt_chunk_size, 4, 1, file);
        (void)std::fwrite(&audio_format, 2, 1, file);
        (void)std::fwrite(&num_channels, 2, 1, file);
        (void)std::fwrite(&sample_rate, 4, 1, file);
        (void)std::fwrite(&byte_rate, 4, 1, file);
        (void)std::fwrite(&block_align, 2, 1, file);
        (void)std::fwrite(&bits_per_sample, 2, 1, file);

        // data chunk
        (void)std::fwrite("data", 1, 4, file);
        (void)std::fwrite(&data_size, 4, 1, file);

        // convert float [-1,1] to signed 16-bit
        for (float sample_value : samples)
        {
            float clamped = std::clamp(sample_value, -1.0f, 1.0f);
            int16_t sample_i16 = static_cast<int16_t>(clamped * cfg.convert_to_16_bit);
            (void)std::fwrite(&sample_i16, sizeof(int16_t), 1, file);
        }

        (void)std::fclose(file);
        return true;
    }
}

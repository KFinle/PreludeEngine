#pragma once
#include <string>
#include <vector>

////////////////////////////
// Check .cpp for details //
////////////////////////////
namespace AudioIO
{
    struct WavConfig
    {
        int sample_rate = 48000;
        int num_channels = 1;
        int bits_per_sample = 16;

        // max value for 16-bit number
        float convert_to_16_bit = 32767.0f;
    };

    bool WriteWav(const std::string& path, const std::vector<float>& samples, const WavConfig& cfg);
}

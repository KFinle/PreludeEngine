#pragma once
#include <string>

struct RenderedSequence
{
    std::string id;
    std::string sound_id;
    std::string filepath;
    float length_sec = 0.0f;
};

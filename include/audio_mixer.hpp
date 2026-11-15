#pragma once
#include <algorithm>

namespace lapCore::AudioMixer
{
    inline float sfx = 1.0f;
    inline float music = 1.0f;

    inline float Sfx(float base = 1.0f)
    { return std::clamp(base * sfx, 0.0f, 1.0f); }
    inline float Music(float base = 1.0f)
    { return std::clamp(base * music, 0.0f, 1.0f); }
}

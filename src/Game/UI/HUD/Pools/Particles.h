#pragma once
#include <vector>
#include "UI/Core/GameUI.h"

#include "RhythmSettings.h"

///////////////////
// Particle Pool //
///////////////////////////////////////////////////////////
// Particles active when player successfully hits a note //
///////////////////////////////////////////////////////////
class HUDParticlePool
{
public:
    struct Particle
    {
        float position_x = 0.0f;
        float position_y = 0.0f;
        float velocity_x = 0.0f;
        float velocity_y = 0.0f;
        float age = 0.0f;
        float life = 0.0f;
        float radius = 1.0f;
        GameUI::Colour colour{};
    };

    void Clear();
    void SetMaxParticles(size_t max_particles);
    void SpawnBurst(float center_x, float center_y, const GameUI::Colour& colour, int particle_count,
                    float speed_min, float speed_max,
                    float life_min, float life_max,
                    float radius_min, float radius_max);
    void Update(float dt_sec);
    void Draw() const;
    size_t Count() const { return m_particles.size(); }

private:
    std::vector<Particle> m_particles;
    size_t m_max_particles = 20;
};

namespace HUDParticlePoolInternal
{
    inline float RandomRange(const float min_v, const float max_v)
    {
        return min_v + (max_v - min_v) * Rhythm::RandFloat01();
    }
}

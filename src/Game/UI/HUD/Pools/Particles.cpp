
#include "Particles.h"

#include "Math/MathUtils.h"

void HUDParticlePool::Clear()
{
    m_particles.clear();
}

void HUDParticlePool::SetMaxParticles(const size_t max_particles)
{
    m_max_particles = max_particles;
    if (m_particles.size() > m_max_particles) m_particles.resize(m_max_particles);
}

void HUDParticlePool::SpawnBurst(const float center_x, const float center_y, const GameUI::Colour& colour, const int particle_count,
                                        const float speed_min, const float speed_max,
                                        const float life_min, const float life_max,
                                        const float radius_min, const float radius_max)
{
    using namespace HUDParticlePoolInternal;

    if (particle_count <= 0 || m_max_particles == 0) return;

    const size_t available = m_particles.size() < m_max_particles ? m_max_particles - m_particles.size() : 0;

    const int spawn_count = static_cast<int>(MinFloat(static_cast<float>(particle_count), static_cast<float>(available)));
    if (spawn_count <= 0) return;

    for (int spawn_index = 0; spawn_index < spawn_count; ++spawn_index)
    {
        const float angle = RandomRange(0.0f, Rhythm::kTwoPi);
        const float speed = RandomRange(speed_min, speed_max);

        Particle particle{};
        particle.position_x = center_x;
        particle.position_y = center_y;
        particle.velocity_x = cosf(angle) * speed;
        particle.velocity_y = sinf(angle) * speed;
        particle.age = 0.0f;
        particle.life = RandomRange(life_min, life_max);
        particle.radius = RandomRange(radius_min, radius_max);
        particle.colour = colour;

        m_particles.push_back(particle);
    }
}

void HUDParticlePool::Update(float dt_sec)
{
    dt_sec = ClampFloat(dt_sec, 0.0f, 0.1f);

    for (size_t particle_index = 0; particle_index < m_particles.size();)
    {
        Particle& particle = m_particles[particle_index];
        particle.age += dt_sec;

        if (particle.age >= particle.life)
        {
            m_particles[particle_index] = m_particles.back();
            m_particles.pop_back();
            continue;
        }

        particle.position_x += particle.velocity_x * dt_sec;
        particle.position_y += particle.velocity_y * dt_sec;
        ++particle_index;
    }
}

void HUDParticlePool::Draw() const
{
    for (const Particle& particle : m_particles)
    {
        const float life_t = 1.0f - (particle.age / particle.life);
        const float fade = ClampFloat(life_t, 0.0f, 1.0f);
        GameUI::Colour colour =
        {
            particle.colour.red * fade,
            particle.colour.green * fade,
            particle.colour.blue * fade
        };
        const float radius = particle.radius * (0.6f + 0.4f * fade);
        GameUI::DrawFilledCircle(particle.position_x, particle.position_y, radius, colour, 16);
    }
}

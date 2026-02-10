#pragma once
#include <cmath>

#include "RhythmSettings.h"

enum WaveType
{
    Sine,
    Saw,
    Square,
    Triangle
};

////////////////
// Oscillator //
/////////////////////////////////////////////////////
// this is where we build the actual synth sounds! //
/////////////////////////////////////////////////////
// TODO:
// - add more wave type (especially a true pulse tone    //
// - build an interface to allow for oscillator stacking //
///////////////////////////////////////////////////////////
class Oscillator
{
public:
    void ResetPhase()
    {
        m_phase = 0.0;
    }

    Oscillator(const double sample_rate) { m_sample_rate = sample_rate; }
    void SetWaveType(const WaveType type) { m_type = type; }
    void SetSampleRate(const double sample_rate) { m_sample_rate = sample_rate; }
    void SetFrequency(const double hz) { m_frequency = hz; }
    void SetPhase(const double phase = 0.0) { m_phase = phase; }

    float GetNextSample()
    {
        const double phase_increment = m_frequency / m_sample_rate;
        m_phase += phase_increment;
        if (m_phase >= 1.0) m_phase -= 1.0;

        switch (m_type)
        {
            case Sine:
                return std::sinf(Rhythm::kTwoPi * static_cast<float>(m_phase));

            case Saw:
                return 2.0f * static_cast<float>(m_phase) - 1.0f;

            case Square:
                return (m_phase < 0.5) ? 1.0f : -1.0f;

            case Triangle:
                return 1.0f - 4.0f * std::fabs(static_cast<float>(m_phase - 0.5));
        }
        return 0.0f;
    }

private:
    double m_sample_rate = 48000.0;
    double m_frequency = 440.0;
    double m_phase = 0.0;
    WaveType m_type = Sine;
};

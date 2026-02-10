#include "EventSequenceRenderer.h"
#include "Audio/Music/Events/Midi.h"
#include "Audio/Music/Events/NoteEvent.h"
#include "Math/MathUtils.h"
#include "Audio/Synth/Voices/KickSynth.h"
#include "Audio/Synth/Voices/NoiseSynth.h"
#include "Audio/Synth/Voices/SquareSynth.h"
#include "Audio/Synth/Voices/TriangleSynth.h"
#include "Audio/Music/DSP/BufferUtils.h"
#include "Audio/Music/DSP/TransientUtils.h"
#include "Audio/Synth/Primitives/Oscillator.h"
#include "Audio/Synth/Primitives/EnvelopeFilter.h"
#include <vector>
#include <cmath>

namespace
{
    //////////////////////////
    // Voice Render Context //
    //////////////////////////
    struct VoiceRenderContext
    {
        const NoteEvent& event;  
        const RenderSettings& settings;
        float base_frequency;
        int musical_samples;
    };

    // converts tail time in seconds to samples
    int CalculateTailSamples(const float tail_seconds, const int sample_rate)
    {
        return static_cast<int>(std::round(tail_seconds * static_cast<float>(sample_rate)));
    }

    //////////
    // Kick //
    //////////
    // rendered as a pitched sine sweep with a transient click
    void RenderKick(std::vector<float>& voice, const VoiceRenderContext& context)
    {
        const int tail_samples = CalculateTailSamples(0.06f, context.settings.sample_rate);
        const int total_samples = context.musical_samples + tail_samples;
        
        voice.reserve(total_samples);
        
        Oscillator oscillator(context.settings.sample_rate);
        oscillator.SetWaveType(Sine);
        oscillator.ResetPhase();
        
        KickSynth::Render(
            voice, oscillator, total_samples,
            static_cast<float>(context.settings.sample_rate),
            context.base_frequency, context.base_frequency * 3.0f,
            0.04f, context.settings.percussion_decay_seconds
        );
        
        // add a short click to simulate compressor
        TransientUtils::AddClick(voice, 0.2f, 48);

        // fade out tail to prevent clicks on overlapping notes 
        BufferUtils::ApplyFadeOut(voice, tail_samples);
    }

    /////////////////
    // Snare / Hat //
    /////////////////
    // noise-based percussive voices
    void RenderNoise(std::vector<float>& voice, const VoiceRenderContext& context)
    {
        voice.reserve(context.musical_samples);
        
        NoiseSynth::NoiseType noise_type = (context.event.voice == VoiceType::Hat)
            ? NoiseSynth::NoiseType::Hat
            : NoiseSynth::NoiseType::Snare;
        
        NoiseSynth::Render(
            voice, context.musical_samples,
            context.settings.percussion_attack_seconds, context.settings.percussion_decay_seconds,
            static_cast<float>(context.settings.sample_rate), noise_type
        );
    }

    ////////////////////////
    // Square-Based Synth //
    ////////////////////////
    // shared renderer for lead and chord voices
    void RenderSquareBased(std::vector<float>& voice, const VoiceRenderContext& context, const WaveType wave_type)
    {
        const int tail_samples = CalculateTailSamples(0.02f, context.settings.sample_rate);
        const int total_samples = context.musical_samples + tail_samples;
        
        voice.reserve(total_samples);
        
        Oscillator oscillator(context.settings.sample_rate);
        EnvelopeFilter envelope_filter;
        
        // reset phase when not sliding to avoid weirdness
        if (context.event.slide_to_hz <= 0.0f)
        {
            oscillator.ResetPhase();
        }
        
        // choose between sliding and static pitch rendering
        if (context.event.slide_to_hz > 0.0f)
        {
            SquareSynth::RenderWithSlide(
                voice, oscillator, envelope_filter, total_samples,
                context.base_frequency, context.event.slide_to_hz,
                static_cast<float>(context.settings.sample_rate),
                context.settings.slide_time_seconds
            );
        }
        else
        {
            SquareSynth::Render(
                voice, oscillator, wave_type, envelope_filter, total_samples,
                context.base_frequency, static_cast<float>(context.settings.sample_rate)
            );
        }
        BufferUtils::ApplyFadeOut(voice, tail_samples);
    }

    ///////////////////
    // Triangle Bass //
    ///////////////////
    void RenderTriangle(std::vector<float>& voice, const VoiceRenderContext& context)
    {
        voice.reserve(context.musical_samples);
        Oscillator oscillator(context.settings.sample_rate);
        TriangleSynth::Render(voice, oscillator, context.musical_samples, context.base_frequency);
    }

    //////////////////////
    // Voice Dispatcher //
    //////////////////////
    // routes a note event to the correct renderer
    // returns false if the voice type is unsupported
    bool RenderVoice(std::vector<float>& voice, const VoiceRenderContext& context)
    {
        switch (context.event.voice)
        {
            case VoiceType::Kick:
                RenderKick(voice, context);
                return true;
            
            case VoiceType::Snare:
            case VoiceType::Hat:
                RenderNoise(voice, context);
                return true;
            
            case VoiceType::Lead:
                RenderSquareBased(voice, context, Square);
                return true;
            
            case VoiceType::Chord:
                RenderSquareBased(voice, context, Saw);
                return true;
            
            case VoiceType::Triangle:
                RenderTriangle(voice, context);
                return true;
            
            default: return false;
        }
    }

    ///////////////////
    // Mixing Helper //
    ///////////////////
    void MixVoiceIntoBuffer(std::vector<float>& mix, const std::vector<float>& voice, const int start_sample, const float gain)
    {
        for (int sample_index = 0; sample_index < static_cast<int>(voice.size()); ++sample_index)
        {
            const int mix_index = start_sample + sample_index;
            if (mix_index < 0 || mix_index >= static_cast<int>(mix.size())) break;
            
            mix[mix_index] += voice[sample_index] * gain;
        }
    }
}

/////////////////////////////
// Main Render Entry Point //
/////////////////////////////
std::vector<float>
EventSequenceRenderer::RenderToBuffer(const EventSequence& sequence, const RenderSettings& settings)
{
    // compute total render duration
    const float total_len_sec = sequence.GetLengthSec() + settings.tail_seconds;
    const int total_samples = static_cast<int>(std::ceil(total_len_sec * static_cast<float>(settings.sample_rate)));

    // beats-to-seconds conversion
    const float seconds_per_beat = 60.0f / sequence.bpm;
    
    // initialize output buffer
    std::vector mix(total_samples, 0.0f);

    /////////////////////////////
    // Per-Note Rendering Loop //
    /////////////////////////////
    for (const NoteEvent& note_event : sequence.notes)
    {
        // convert musical time to seconds
        const float start_second = note_event.start_beat * seconds_per_beat;
        const float duration_seconds = note_event.duration_beat * seconds_per_beat;

        // convert seconds to samples
        const int start_sample = static_cast<int>(std::round(start_second * static_cast<float>(settings.sample_rate)));
        const int musical_samples = static_cast<int>(std::round(duration_seconds * static_cast<float>(settings.sample_rate)));
        
        // skip invalid notes
        if (musical_samples <= 0) continue;

        // construct render context for this note
        VoiceRenderContext context =
        {
            note_event,
            settings,
            static_cast<float>(MidiToFrequency(note_event.midi_note)),
            musical_samples
        };

        // render the voice into a temporary buffer
        std::vector<float> voice;
        if (!RenderVoice(voice, context)) continue;

        // compute final gain
        const float total_gain = note_event.velocity *  sequence.mix.GetVoiceGain(note_event.voice) * sequence.mix.master_gain;
        
        // mix rendered voice into the output buffer
        MixVoiceIntoBuffer(mix, voice, start_sample, total_gain);
    }

    // final output clamp
    for (float& sample_value : mix)
    {
        sample_value = ClampFloat(sample_value, -1.0f, 1.0f);
    }
    return mix;
}

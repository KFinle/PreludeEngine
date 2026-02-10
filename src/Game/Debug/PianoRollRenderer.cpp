#include "PianoRollRenderer.h"
#include "Engine/Engine.h"
#include "UI/Core/GameUI.h"
#include "Math/MathUtils.h"

namespace
{
    constexpr float drum_visual_note_duration = 0.25f;
    constexpr float velocity_brightness_min = 0.6f;
    constexpr float velocity_brightness_range = 0.4f;
    constexpr float min_note_width = 1.0f;
    constexpr float min_note_height = 2.0f;
    constexpr float drum_note_height_scale = 0.75f;
    constexpr float debug_panel_text_y_offset = 6.0f;
    constexpr float debug_panel_text_x_offset = 8.0f;
    constexpr float debug_line_height = 14.0f;
    constexpr float debug_line_spacing = 2.0f;
    
    float CalculateDrumVisualDuration(const int beats_per_bar)
    {
        return (static_cast<float>(beats_per_bar) / 4.0f) * drum_visual_note_duration;
    }
}

PianoRollRenderer::ThemeColours::RGB 
PianoRollRenderer::ThemeColours::GetVoiceColour(const VoiceType voice)
{
    switch (voice) {
        case VoiceType::Kick:     return { 0.90f, 0.20f, 0.20f };
        case VoiceType::Snare:    return { 0.95f, 0.55f, 0.20f };
        case VoiceType::Hat:      return { 0.80f, 0.80f, 0.80f };
        case VoiceType::Triangle: return { 0.25f, 0.45f, 0.85f };
        case VoiceType::Lead:     return { 0.30f, 0.85f, 0.45f };
        case VoiceType::Chord:    return { 0.65f, 0.35f, 0.85f };
        default:                  return { 0.70f, 0.70f, 0.70f };
    }
}

PianoRollRenderer::RenderLayout PianoRollRenderer::CalculateLayout(const Config& config) const
{
    RenderLayout layout;
    
    layout.drum_height = config.drum_track_height;
    layout.debug_height = config.debug_text_height;
    layout.pitch_height = MaxFloat(0.0f, config.height - layout.drum_height - layout.debug_height);
    layout.total_height = layout.drum_height + layout.pitch_height + layout.debug_height;
    
    // center content vertically
    const float container_center_y = config.origin_y - config.height * 0.5f;
    const float content_bottom_y = container_center_y + layout.total_height * 0.5f;
    
    // stack 
    layout.debug_top_y = content_bottom_y;
    layout.pitch_top_y = layout.debug_top_y + layout.debug_height;
    layout.drum_top_y = layout.pitch_top_y + layout.pitch_height;
    
    // lane configuration
    layout.pitch_lane_count = (config.midi_max - config.midi_min) + 1;
    layout.drum_lane_height = layout.drum_height / 3.0f;
    
    // time conversion
    layout.pixels_per_beat = config.width / MaxFloat(1.0f, config.beats_per_screen);
    layout.view_start_beat = config.view_start_beat;
    layout.view_end_beat = config.view_start_beat + config.beats_per_screen;
    
    return layout;
}

void PianoRollRenderer::DrawVerticalGridLines(
    const float region_left_x, const float region_top_y, const float region_height,
    const float view_start_beat, const float view_end_beat, const float pixels_per_beat, const int beats_per_bar,
    const ThemeColours::RGB& beat_colour, const ThemeColours::RGB& bar_colour)
{
    const int start_beat_index = static_cast<int>(std::floor(view_start_beat));
    const int end_beat_index = static_cast<int>(std::ceil(view_end_beat));
    
    for (int beat_index = start_beat_index; beat_index <= end_beat_index; ++beat_index)
    {
        const float beat_x = region_left_x + (static_cast<float>(beat_index) - view_start_beat) * pixels_per_beat;
        const bool is_bar_line = (beats_per_bar > 0) && (beat_index % beats_per_bar == 0);
        const auto& colour = is_bar_line ? bar_colour : beat_colour;
        
        Engine::DrawLine(beat_x, region_top_y, beat_x, region_top_y + region_height, 
                     colour.red, colour.green, colour.blue);
    }
}

void PianoRollRenderer::DrawHorizontalGridLines(
    const float region_left_x, const float region_top_y, const float region_width, const float region_height,
    const int num_divisions, const ThemeColours::RGB& colour)
{
    if (num_divisions <= 0) return;
    
    const float lane_height = region_height / static_cast<float>(num_divisions);
    
    for (int lane_index = 0; lane_index <= num_divisions; ++lane_index)
    {
        const float line_y = region_top_y + static_cast<float>(lane_index) * lane_height;
        Engine::DrawLine(region_left_x, line_y, region_left_x + region_width, line_y, 
                     colour.red, colour.green, colour.blue);
    }
}

PianoRollRenderer::NoteRenderInfo::Position
PianoRollRenderer::CalculateDrumNotePosition(const VoiceType voice, const float start_beat, 
                                              const RenderLayout& layout, const Config& config)
{
    NoteRenderInfo::Position pos;
    
    const int drum_lane = GetDrumLaneIndex(voice);
    const float visual_duration = CalculateDrumVisualDuration(config.beats_per_bar);
    
    pos.position_x = config.origin_x + (start_beat - layout.view_start_beat) * layout.pixels_per_beat;
    pos.width = MaxFloat(min_note_width, visual_duration * layout.pixels_per_beat);
    pos.height = layout.drum_lane_height * drum_note_height_scale;
    pos.position_y = layout.drum_top_y + static_cast<float>(drum_lane) * layout.drum_lane_height 
          + (layout.drum_lane_height - pos.height) * 0.5f;
    
    return pos;
}

PianoRollRenderer::NoteRenderInfo::Position
PianoRollRenderer::CalculatePitchNotePosition(const NoteEvent& note, const float start_beat,
                                               const float end_beat, const RenderLayout& layout, 
                                               const Config& config)
{
    NoteRenderInfo::Position pos;
    
    if (layout.pitch_height <= 0.0f) return pos;
    
    pos.position_x = config.origin_x + (start_beat - layout.view_start_beat) * layout.pixels_per_beat;
    pos.width = MaxFloat(min_note_width, (end_beat - start_beat) * layout.pixels_per_beat);
    pos.height = MaxFloat(min_note_height, MinFloat(config.lane_height, layout.pitch_height * 0.02f));
    
    const float clamped_midi = ClampFloat(static_cast<float>(note.midi_note), 
                                     static_cast<float>(config.midi_min), 
                                     static_cast<float>(config.midi_max));
    
    const int lane_index = static_cast<int>(clamped_midi) - config.midi_min;
    
    const float lane_height = layout.pitch_height / static_cast<float>(layout.pitch_lane_count);
    
    const float note_center_y = layout.pitch_top_y + (static_cast<float>(lane_index) + 0.5f) * lane_height;
    
    pos.position_y = ClampFloat(note_center_y - pos.height * 0.5f,
                  layout.pitch_top_y,
                  layout.pitch_top_y + layout.pitch_height);
    
    return pos;
}

PianoRollRenderer::ThemeColours::RGB
PianoRollRenderer::CalculateNoteColour(const VoiceType voice, const float velocity)
{
    auto colour = ThemeColours::GetVoiceColour(voice);
    const float clamped_velocity = ClampFloat(velocity, 0.0f, 1.0f);
    const float brightness = velocity_brightness_min + velocity_brightness_range * clamped_velocity;
    
    colour.red = ClampFloat(colour.red * brightness, 0.0f, 1.0f);
    colour.green = ClampFloat(colour.green * brightness, 0.0f, 1.0f);
    colour.blue = ClampFloat(colour.blue * brightness, 0.0f, 1.0f);
    
    return colour;
}

PianoRollRenderer::NoteRenderInfo 
PianoRollRenderer::CalculateNoteRenderInfo(const NoteEvent& note, const RenderLayout& layout, 
                                           const Config& config) const
{
    NoteRenderInfo info;
    
    // calculate visible beat range
    float start_beat = note.start_beat;
    float end_beat = IsDrumVoice(note.voice) 
        ? start_beat + CalculateDrumVisualDuration(config.beats_per_bar)
        : start_beat + MaxFloat(0.001f, note.duration_beat);
    
    if (config.clamp_to_view)
    {
        if (end_beat < layout.view_start_beat || start_beat > layout.view_end_beat)
        {
            return info;
        }
        start_beat = MaxFloat(start_beat, layout.view_start_beat);
        end_beat = MinFloat(end_beat, layout.view_end_beat);
    }
    
    if (end_beat <= start_beat) return info;
    
    // calculate position based on voice type
    if (IsDrumVoice(note.voice))
    {
        info.position = CalculateDrumNotePosition(note.voice, start_beat, layout, config);
    }
    else
    {
        info.position = CalculatePitchNotePosition(note, start_beat, end_beat, layout, config);
    }
    
    info.note_colour = CalculateNoteColour(note.voice, note.velocity);
    info.is_valid = true;
    
    return info;
}

void PianoRollRenderer::DrawNote(const NoteRenderInfo& info)
{
    if (!info.is_valid) return;
    
    const GameUI::Colour colour = { info.note_colour.red, info.note_colour.green, info.note_colour.blue };

    GameUI::DrawRectangle(info.position.position_x, info.position.position_y, 
                          info.position.width, info.position.height, colour, true);
}

void PianoRollRenderer::DrawDebugPanel(const float panel_top_y, const float panel_height,
                                       const Config& config)
{
    const GameUI::Colour bg_colour =
    { 
        ThemeColours::DEBUG_PANEL_BG.red, 
        ThemeColours::DEBUG_PANEL_BG.green,
        ThemeColours::DEBUG_PANEL_BG.blue 
    };
    
    GameUI::DrawRectangle(config.origin_x, panel_top_y, config.width, panel_height, bg_colour);
    
    char buffer[64];
    const float playhead_beat = SecondsToBeats(config.playhead_sec, config.bpm);
    
    float text_y = panel_top_y + debug_panel_text_y_offset;
    const float text_x = config.origin_x + debug_panel_text_x_offset;
    
    (void)std::snprintf(buffer, sizeof(buffer), "BPM %.2f | Beats/Bar %d", 
                 config.bpm, config.beats_per_bar);
    
    Engine::Print(text_x, text_y, buffer);
    text_y += debug_line_height * debug_line_spacing;
    
    (void)std::snprintf(buffer, sizeof(buffer), "Playhead %.2f beats (%.3f sec)", 
                 playhead_beat, config.playhead_sec);
    
    Engine::Print(text_x, text_y, buffer);
}

void PianoRollRenderer::DrawPlayhead(const RenderLayout& layout, const Config& config)
{
    const float playhead_beat = SecondsToBeats(config.playhead_sec, config.bpm);
    const float playhead_x = config.origin_x + (playhead_beat - layout.view_start_beat) * layout.pixels_per_beat;
    
    const float content_top_y = layout.drum_top_y + layout.drum_height;
    const float content_bottom_y = layout.debug_top_y;
    
    Engine::DrawLine(playhead_x, content_bottom_y, playhead_x, content_top_y, 
                 ThemeColours::PLAYHEAD.red, ThemeColours::PLAYHEAD.green, ThemeColours::PLAYHEAD.blue);
}

void PianoRollRenderer::DrawGrids(const RenderLayout& layout, const Config& config)
{
    // drum track grid
    DrawHorizontalGridLines(config.origin_x, layout.drum_top_y, config.width, layout.drum_height, 
                           3, ThemeColours::GRID_DRUM_LANE);
    
    DrawVerticalGridLines(config.origin_x, layout.drum_top_y, layout.drum_height, layout.view_start_beat,
                          layout.view_end_beat, layout.pixels_per_beat, config.beats_per_bar,
                          ThemeColours::GRID_BEAT, ThemeColours::GRID_DRUM_BAR);
    
    // pitch lanes grid
    if (layout.pitch_height > 0.0f)
    {
        DrawHorizontalGridLines(config.origin_x, layout.pitch_top_y, config.width, layout.pitch_height,
                               layout.pitch_lane_count, ThemeColours::GRID_PITCH_LANE);
        DrawVerticalGridLines(config.origin_x, layout.pitch_top_y, layout.pitch_height, layout.view_start_beat,
                              layout.view_end_beat, layout.pixels_per_beat, config.beats_per_bar,
                              ThemeColours::GRID_BEAT, ThemeColours::GRID_BAR);
    }
}

void PianoRollRenderer::Draw(const EventSequence& sequence, const Config& config) const
{
    const RenderLayout layout = CalculateLayout(config);
    
    if (config.draw_grid)
    {
        DrawGrids(layout, config);
    }
    
    for (const NoteEvent& note : sequence.notes)
    {
        const NoteRenderInfo info = CalculateNoteRenderInfo(note, layout, config);
        DrawNote(info);
    }
    
    DrawDebugPanel(layout.debug_top_y, layout.debug_height, config);
    DrawPlayhead(layout, config);
}

#pragma once
#include "Audio/Music/Events/EventSequence.h"

////////////////////////////////////////////////////////////////////
// Pseudo Piano Roll layout used for debugging the CompositionAPI //
////////////////////////////////////////////////////////////////////
// NOTE: the .cpp is pretty sloppy code. I originally intended to //
//       remove it, but I'm keeping it in                         //
//       just because the end result is really cool to look at.   //
//       This is a visualization tool that was incredibly helpful //
//       when debugging the composition tools in this project,    //
//       so removing it from the final build would be sad.        //
////////////////////////////////////////////////////////////////////

class PianoRollRenderer
{
public:
    struct Config
    {
        float origin_x, origin_y, width, height;

        // pitch
        int   midi_min;
        int   midi_max;
        float lane_height;

        // musical time
        float bpm;
        int   beats_per_bar;

        // view
        float view_start_beat;
        float beats_per_screen;

        // playhead
        float playhead_sec;

        // layout
        float drum_track_height = 60.0f;
        float debug_text_height = 72.0f;

        // options
        bool draw_grid    = true;
        bool clamp_to_view = true;
    };

    void Draw(const EventSequence& sequence, const Config& config) const;

private:
    // theme
    struct ThemeColours
    {
        struct RGB { float red, green, blue; };
        
        // grid colors
        static constexpr RGB GRID_BEAT = { 0.18f, 0.18f, 0.18f };
        static constexpr RGB GRID_BAR = { 0.35f, 0.35f, 0.35f };
        static constexpr RGB GRID_PITCH_LANE = { 0.14f, 0.14f, 0.14f };
        static constexpr RGB GRID_DRUM_LANE = { 0.16f, 0.16f, 0.16f };
        static constexpr RGB GRID_DRUM_BAR = { 0.30f, 0.30f, 0.30f };
        
        // note colors by voice type
        static RGB GetVoiceColour(VoiceType voice);
        
        // UI elements
        static constexpr RGB PLAYHEAD = { 1.0f, 0.0f, 0.0f };
        static constexpr RGB DEBUG_PANEL_BG = { 0.08f, 0.08f, 0.10f };
    };

    // layout calculation
    struct RenderLayout
    {
        // drum track
        float drum_top_y;
        float drum_height;
        float drum_lane_height;
        
        // pitch lanes
        float pitch_top_y;
        float pitch_height;
        int pitch_lane_count;
        
        // debug panel
        float debug_top_y;
        float debug_height;
        
        // time conversion
        float pixels_per_beat;
        float view_start_beat;
        float view_end_beat;
        
        // total content height
        float total_height;
    };

    // note rendering
    struct NoteRenderInfo
    {
        struct Position
        {
            float position_x = 0.0f;
            float position_y = 0.0f;
            float width = 0.0f;
            float height = 0.0f;
        };
        
        Position position;
        ThemeColours::RGB note_colour = {0.0f, 0.0f, 0.0f};
        bool is_valid = false;
    };

    // layout
    RenderLayout CalculateLayout(const Config& config) const;

    // grid rendering
    static void DrawGrids(const RenderLayout& layout, const Config& config);

    static void DrawVerticalGridLines(float region_left_x, float region_top_y,
                                      float region_height, float view_start_beat, float view_end_beat,
                                      float pixels_per_beat, int beats_per_bar,
                                      const ThemeColours::RGB& beat_colour, const ThemeColours::RGB& bar_colour);

    static void DrawHorizontalGridLines(float region_left_x, float region_top_y, float region_width,
                                        float region_height, int num_divisions, const ThemeColours::RGB& colour);

    // note rendering
    NoteRenderInfo CalculateNoteRenderInfo(const NoteEvent& note, const RenderLayout& layout, 
                                           const Config& config) const;

    static NoteRenderInfo::Position CalculateDrumNotePosition(VoiceType voice, float start_beat,
                                                              const RenderLayout& layout, const Config& config);

    static NoteRenderInfo::Position CalculatePitchNotePosition(const NoteEvent& note, float start_beat, float end_beat,
                                                               const RenderLayout& layout, const Config& config);

    static ThemeColours::RGB CalculateNoteColour(VoiceType voice, float velocity);

    static void DrawNote(const NoteRenderInfo& info);

    // UI overlays
    static void DrawDebugPanel(float panel_top_y, float panel_height,
                               const Config& config);

    static void DrawPlayhead(const RenderLayout& layout, const Config& config);
};

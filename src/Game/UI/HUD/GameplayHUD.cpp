#include "UI/HUD/GameplayHUD.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <vector>
#include "Engine/Engine.h"
#include "UI/Core/GameUI.h"
#include "Math/MathUtils.h"
#include "UI/HUD/HUDSkin.h"
#include "Gameplay/HUDMode.h"
#include "UI/HUD/Skins/HUDSkinSinglePlayer.h"
#include "UI/HUD/Skins/HUDSkinTwoPlayer.h"
#include "UI/HUD/Pools/Particles.h"
#include "UI/HUD/Pools/Ghosts.h"

//////////////////////
// HUD Skin Manager //
//////////////////////
namespace
{
	IHUDSkin* hud_skin = nullptr;
	HUDMode current_mode;
	HUDParticlePool hit_particles;
	HUDGhostPool ghost_pool;
	float last_hit_beat = -1.0f;

	// const char* HudModeName(HUDMode mode)
	// {
	// 	switch (mode)
	// 	{
	// 		case HUDMode::SinglePlayer: return "SinglePlayer";
	// 		case HUDMode::TwoPlayer: return "TwoPlayer";
	// 		case HUDMode::DebugRoll: return "DebugRoll";
	// 		default: return "Unknown";
	// 	}
	// }

	IHUDSkin* GetSkinForMode(const HUDMode mode)
	{
		switch (mode)
		{
		case HUDMode::SinglePlayer:
			return new SinglePlayerSkin();
		case HUDMode::TwoPlayer:
			return new TwoPlayerSkin();
		default:
			return new SinglePlayerSkin();
		}
	}

	void InitializeSkin(const HUDMode mode)
	{
		// clean up old skin
		if (hud_skin)
		{
			delete hud_skin;
			hud_skin = nullptr;
		}

		current_mode = mode;

		// initialize for all cockpit modes
		if (mode != HUDMode::DebugRoll)
		{
			hud_skin = GetSkinForMode(mode);
		}
	}

	/////////////////
	// HUD overlay //
	/////////////////
	static void DrawHudOverlay(const EventSequence& sequence, const GameState& game)
	{
		// display debug information at top left
		float text_x = 10.0f;
		float cursor_y = APP_VIRTUAL_HEIGHT - 30;

		char text_buffer[128];
		(void)snprintf(text_buffer, sizeof(text_buffer), "DIFFICULTY: %s", game.gameplay.difficulty.c_str());
		Engine::Print(text_x, cursor_y, text_buffer);
		cursor_y -= 25;

		// current entity count
		// snprintf(text_buffer, sizeof(text_buffer), "NOTES ON LANES: %u",
		// 	static_cast<unsigned>(game.gameplay.note_pool.Count()));
		// Engine::Print(text_x, cursor_y, text_buffer);
		// cursor_y -= 25;

		const size_t remaining = ghost_pool.Remaining(sequence, game.gameplay.timing_mode, game.hud.active_lanes_mask, game.gameplay.current_beat);
		
		(void)snprintf(text_buffer, sizeof(text_buffer), "REMAINING HITS: %u", static_cast<unsigned>(remaining));
		Engine::Print(text_x, cursor_y, text_buffer);
		cursor_y -= 25;

		// hit accuracy stats
		(void)snprintf(text_buffer, sizeof(text_buffer),
			"PERFECT: %d | GOOD: %d | LATE: %d | MISSED: %d",
			game.gameplay.score.totals.perfect,
			game.gameplay.score.totals.good,
			game.gameplay.score.totals.late,
			game.gameplay.score.totals.missed);
		Engine::Print(text_x, cursor_y, text_buffer);
		cursor_y -= 25;

		// current beat
		(void)snprintf(text_buffer, sizeof(text_buffer),
			"BEAT: %.2f",
			game.gameplay.current_beat);
		Engine::Print(text_x, cursor_y, text_buffer);
		cursor_y -= 25;

		// punishment mode status
		(void)snprintf(text_buffer, sizeof(text_buffer),
			"PUNISH: %s",
			game.gameplay.punish_enabled ? "ON" : "OFF");
		Engine::Print(text_x, cursor_y, text_buffer);
	}

	///////////////////
	// Stability bar //
	///////////////////
	void DrawStabilityBar(const GameState& game)
	{
		// stability bar at bottom right of screen
		constexpr float bar_width = 200.0f;
		constexpr float bar_height = 20.0f;
		constexpr float padding = 20.0f;
		constexpr float bar_x = static_cast<float>(APP_VIRTUAL_WIDTH) - bar_width - padding;
		constexpr float bar_y = padding;

		// clamp stability to 0-1 range
		const float clamped_stability = ClampFloat(game.gameplay.stability, 0.0f, 1.0f);

		// colour shifts from green to red
		float bar_red;
		float bar_green;
		float bar_blue;
		if (clamped_stability < 0.3f)
		{
			bar_red = 1.0f; bar_green = 0.0f; bar_blue = 0.0f;
		}
		else if (clamped_stability < 0.6f)
		{
			bar_red = 1.0f; bar_green = 0.8f; bar_blue = 0.0f;
		}
		else
		{
			bar_red = 0.0f; bar_green = 1.0f; bar_blue = 0.0f;
		}

		// draw the filled bar with colour gradient

		if (game.gameplay.punish_enabled)
		{
			GameUI::DrawHealthBar(bar_x, bar_y, bar_width, bar_height,
				clamped_stability, 1.0f,
				bar_red, bar_green, bar_blue,
				1.0f, 1.0f, 1.0f);

			// draw text label abvove bar
			char stability_buffer[16];
			(void)snprintf(stability_buffer, sizeof(stability_buffer), "STABILITY: %.2f", game.gameplay.stability);
			GameUI::PrintTextWithOutline(bar_x, bar_y + bar_height + 5, stability_buffer, GameUI::COLOUR_WHITE);
		}
	}

	float NoteDepthScalePx()
	{
		constexpr float vmin_px = static_cast<float>(APP_VIRTUAL_HEIGHT);
		return vmin_px * 0.62f;
	}

	void UpdateHitParticles(const float dt_sec, const IHUDSkin& skin, GameState& game)
	{
		hit_particles.Update(dt_sec);

		if (game.hud.last_consumed_target_beat < last_hit_beat - 0.001f) last_hit_beat = -1.0f;

		const float beat = game.hud.last_consumed_target_beat;
		if (beat <= last_hit_beat + 0.0001f) return;

		last_hit_beat = beat;

		if (game.hud.last_timing != TimingResult::Perfect && game.hud.last_timing != TimingResult::Good) return;

		int particle_count = (game.hud.last_timing == TimingResult::Perfect) ? 6 : 3;

		GameUI::Colour lane_colour = skin.GetLaneColour(game.hud.last_hit_lane);
		GameUI::Colour colour =
		{
			lane_colour.red * 0.6f,
			lane_colour.green * 0.6f,
			lane_colour.blue * 0.6f
		};

		float target_x = 0.0f;
		float target_y = 0.0f;
		skin.SetLaneTarget(game.hud.last_hit_lane, target_x, target_y);

		const float particle_x = target_x;
		const float particle_y = target_y;

		hit_particles.SpawnBurst(particle_x, particle_y, colour, particle_count,
			20.0f, 60.0f,
			0.20f, 0.9f,
			1.5f, 6.0f);
	}
}

namespace GameplayHUD
{
	// basic hud renderer 
	void DrawGameplayHUD(const MusicTransport& music, const EventSequence& sequence, GameState& game, const HUDMode mode)
	{
		// reinitialize skin if mode changed
		if (current_mode != mode) InitializeSkin(mode);

		// initialize skin on first call
		if (!hud_skin) InitializeSkin(mode);

		// begin frame for the current skin
		hud_skin->BeginFrame(APP_VIRTUAL_WIDTH, APP_VIRTUAL_HEIGHT, game.gameplay.current_beat, music.dt_seconds);
		game.hud.active_lanes_mask = hud_skin->GetActiveLanesMask();

		UpdateHitParticles(music.dt_seconds, *hud_skin, game);

		// draw background elements
		hud_skin->DrawBackground();

		// draw ghost notes outside the lanes
		if (hud_skin->GhostsEnabled())
		{
			ghost_pool.Draw(
				sequence,
				game.gameplay.timing_mode,
				game.gameplay.current_beat,
				game.hud.ghost_approach_window_beats,
				*hud_skin,
				game.hud.active_lanes_mask);
		}

		// draw particles
		hit_particles.Draw();

		// draw lane rails and centre lines
		hud_skin->DrawLanes();

		// draw all entities approaching along their lanes (far to near)
		const float depth_scale_px = NoteDepthScalePx();
		std::vector<size_t> draw_order;
		draw_order.reserve(game.gameplay.note_pool.Count());

		for (size_t note_index = 0; note_index < game.gameplay.note_pool.Count(); ++note_index)
		{
			draw_order.push_back(note_index);
		}

		std::sort(draw_order.begin(), draw_order.end(),
			[&](const int left_index, const int right_index)
			{
				const auto& left_note = game.gameplay.note_pool.GetNote(left_index);
				const auto& right_note = game.gameplay.note_pool.GetNote(right_index);
				if (fabsf(left_note.distance_px - right_note.distance_px) > 0.01f) return left_note.distance_px > right_note.distance_px;
				return left_note.beat < right_note.beat;
			});

		for (auto note_index : draw_order)
		{
			const auto& note = game.gameplay.note_pool.GetNote(note_index);
			const float depth_normalized = ClampFloat(note.distance_px / depth_scale_px, 0.0f, 1.0f);

			HUDSkinEntity entity{};
			entity.id = note.id;
			entity.lane = note.lane;
			entity.depth_normalized = depth_normalized;
			entity.size = note.size;
			entity.consumed = note.consumed;

			hud_skin->DrawEntity(entity);
		}

		// draw centre reticle
		hud_skin->DrawReticle();

		// draw stability bar
		DrawStabilityBar(game);

		// draw debug overlay 
		DrawHudOverlay(sequence, game);
	}

	// ============================================================
	void DrawDebugRoll(
		const MusicTransport& music,
		const EventSequence& sequence,
		PianoRollRenderer& roll,
		GameState& game)
	{
		// set mode to debug piano roll
		game.hud.hud_mode = HUDMode::DebugRoll;

		// configure piano roll renderer
		PianoRollRenderer::Config config{};
		config.origin_x = APP_VIRTUAL_WIDTH * 0.04f;
		config.origin_y = APP_VIRTUAL_HEIGHT * 0.08f;
		config.width = APP_VIRTUAL_WIDTH * 0.90f;
		config.height = APP_VIRTUAL_HEIGHT * 0.55f;

		config.bpm = sequence.bpm;
		config.beats_per_bar = sequence.beats_per_bar;

		config.midi_min = 24;
		config.midi_max = 84;
		config.lane_height = 8.0f;

		config.playhead_sec = music.visual_seconds;
		config.beats_per_screen = 16.0f;
		config.view_start_beat = ClampFloat(music.GetBeat() - 4.0f, 0.0f, INFINITY);

		config.draw_grid = true;
		config.clamp_to_view = true;

		// draw debug mode label and piano roll
		Engine::Print(40, APP_VIRTUAL_HEIGHT - 40, "MODE: DEBUG (PIANO ROLL)");
		roll.Draw(sequence, config);

	}

    /////////////////
	// Entry Point //
    /////////////////
	void Draw(const MusicTransport& music, GameState& game, const HUDMode mode, const EventSequence& sequence, PianoRollRenderer& roll)
	{
		game.hud.hud_mode = mode;

		// handle cockpit modes with skins
        if (mode != HUDMode::DebugRoll) DrawGameplayHUD(music, sequence, game, mode);
		
        // handle debug roll mode
		else if (mode == HUDMode::DebugRoll)
		{
			DrawDebugRoll(music, sequence, roll, game);
		}
	}
}

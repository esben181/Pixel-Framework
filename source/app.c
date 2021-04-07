#include "debug.h"
#include "memory_arena.h"
#include "state.h"
#include "app.h"

#include "state_game.c"
#include "state.c"


internal void
AppUpdate(Platform *p)
{
	platform = p;
	Assert(sizeof(App) <= platform->permanent_storage_size);
	app = platform->permanent_storage;

	if (!platform->initialized)	
	{
		platform->initialized = 1;
		platform->target_frames_per_second = 75.0f;

		// Allocate arenas
		{ 
			app->permanent_arena = MemoryArenaInit(platform->permanent_storage,
							       platform->permanent_storage_size);
			MemoryArenaAllocate(&app->permanent_arena, sizeof(App));

			app->frame_arena = MemoryArenaInit(platform->scratch_storage,
							   platform->scratch_storage_size);

			u64 state_arena_size = app->permanent_arena.memory_left;
			app->state_arena = MemoryArenaInit(MemoryArenaAllocate(&app->permanent_arena, state_arena_size),
							   state_arena_size);
		}


		// Set up state arena and initial app state
		{
			app->current_state = STATE_game;
			app->next_state = STATE_INVALID;

			StateInit(app->current_state, &app->state_arena);
		}

	}

	StateUpdate(app->current_state, &app->state_arena);

	MemoryArenaClear(&app->frame_arena);


	// Calculate next app state
	{
		if (app->next_state != STATE_INVALID)
		{
			StateCleanUp(app->current_state, &app->state_arena);
			app->current_state = app->next_state;
			StateInit(app->current_state, &app->state_arena);
		}
	}


	
	// Update platform data
	{
		platform->last_mouse_x = platform->mouse_x;
		platform->last_mouse_y = platform->mouse_y;
		// Update key data for next frame
		{
			platform->last_key = 0;
			for (u32 i = 0; i < KEY_MAX; ++i)
			{
				platform->key_pressed[i] = 0;
			}
			platform->left_mouse_pressed = 0;
			platform->right_mouse_pressed = 0;
		}
	}
}



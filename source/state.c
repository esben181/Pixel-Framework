internal void
StateInit(StateType type, MemoryArena *state_arena)
{
	switch (type)
	{
#define State(name, lower_name) case STATE_##lower_name:\
		{\
			name##StateInit(MemoryArenaAllocate(state_arena, sizeof(name##State)));\
		} break;
#include "state_list.h"
#undef State
		default:
			platform->OutputError("Invalid State", "Cannot initialize state type #%d.", type);
	}
}

internal StateType
StateUpdate(StateType type, MemoryArena *state_arena)
{
	StateType next_state = STATE_INVALID;
	switch (type)
	{
#define State(name, lower_name) case STATE_##lower_name:\
		{\
			next_state = name##StateUpdate(state_arena->memory);\
		} break;
#include "state_list.h"
#undef State
		default:
			platform->OutputError("Invalid State", "Cannot update state type #%d.", type);
	}

	return next_state;
}

internal StateType
StateCleanUp(StateType type, MemoryArena *state_arena)
{
	switch (type)
	{
#define State(name, lower_name) case STATE_##lower_name:\
		{\
			name##StateCleanUp(state_arena->memory);\
		} break;
#include "state_list.h"
#undef State
		default:
			platform->OutputError("Invalid State", "Cannot clean up state type #%d.", type);
	}
	MemoryArenaClear(state_arena);
}

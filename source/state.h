typedef enum StateType StateType;
enum StateType
{
	STATE_INVALID,
#define State(name, lower_name) STATE_##lower_name,
#include "state_list.h"
#undef State
};

internal void
StateInit(StateType type, MemoryArena *state_arena);
internal StateType
StateUpdate(StateType type, MemoryArena *state_arena);
internal StateType
StateCleanUp(StateType type, MemoryArena *state_arena);


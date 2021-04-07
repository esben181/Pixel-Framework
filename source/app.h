typedef struct App App;
struct App
{
	MemoryArena permanent_arena;
	MemoryArena state_arena;
	MemoryArena frame_arena;

	u32 render_w;
	u32 render_h;

	StateType current_state;
	StateType next_state;
};

global App *app;

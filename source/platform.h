enum 
{
#define Key(code_name, str_name) KEY_##code_name,
#include "key_list.h"
	KEY_MAX	
#undef Key

};

char *key_name[KEY_MAX] = {
#define Key(code_name, str_name) str_name,
#include "key_list.h"
#undef Key
};

typedef struct Platform Platform;
struct Platform
{
	b32 initialized;
	b32 running;
	f32 target_frames_per_second;
	f32 current_time;

	f32 mouse_x;
	f32 mouse_y;
	f32 last_mouse_x;
	f32 last_mouse_y;
	b32 left_mouse_down;
	b32 right_mouse_down;
	b32 left_mouse_pressed;
	b32 right_mouse_pressed;
	b8  key_down[KEY_MAX];
	b8  key_pressed[KEY_MAX];
	i32 last_key;

	u32 window_width;
	u32 window_height;
	b32 window_resized;

	u64 permanent_storage_size;
	void *permanent_storage; // MUST be cleared to zero by platform
	u64 scratch_storage_size;
	void *scratch_storage;



	void (*OutputError)(char *, char *, ...);
	void (*SwapBuffers)(void);
	void (*SetPixel)(u32, u32, color_t);
};

global Platform *platform;

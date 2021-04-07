#include <windows.h>
#include <stdarg.h>
#include <stdio.h>

#include "language.h"
#include "constants.h"
#include "platform.h"

#include "app.c"

global Platform global_platform;

#define BYTES_PER_PIXEL 4
typedef struct PixelBuffer 
{
    BITMAPINFO info;
    void *pixels;
    u32 width;
    u32 height;
    u32 pitch;		
} PixelBuffer;

global PixelBuffer global_pixel_buffer;

void Win32SetPixel(u32 x, u32 y, color_t color)
{
	// out of bounds
	if (x < 0 || x > global_pixel_buffer.width || y < 0 || y > global_pixel_buffer.height)
	{
		return;
	}
	color_t *buffer = (color_t*)global_pixel_buffer.pixels;
	buffer[global_pixel_buffer.width*y + x] = color;
}

internal void
Win32DisplayBuffer(HDC device_context,
		    PixelBuffer *buffer)
{
    StretchDIBits(device_context,
                  // destination
                  0, 0,
		  global_platform.window_width, global_platform.window_height,
                  // source
                  0, 0,
                  buffer->width, buffer->height,
                  buffer->pixels,
                  &buffer->info,
                  DIB_RGB_COLORS,
                  SRCCOPY);
}

internal void
Win32ResizeBuffer(PixelBuffer *buffer, u32 width, u32 height)
{
	if (buffer->pixels)
	{
		ZeroMemory(buffer->pixels, buffer->width * buffer->height * BYTES_PER_PIXEL);
		VirtualFree(buffer->pixels, 0, MEM_RELEASE);
	}
	buffer->width = width;
	buffer->height = height;
	buffer->pitch = width*BYTES_PER_PIXEL;

	buffer->info.bmiHeader.biSize = sizeof(buffer->info.bmiHeader);
	buffer->info.bmiHeader.biWidth = buffer->width;
	buffer->info.bmiHeader.biHeight = -buffer->height;
	buffer->info.bmiHeader.biPlanes = 1;
	buffer->info.bmiHeader.biBitCount = 32;
	buffer->info.bmiHeader.biCompression = BI_RGB;
	buffer->info.bmiHeader.biSizeImage = 0;
	buffer->info.bmiHeader.biClrUsed = 0;


	size_t buffer_data_size = buffer->width*buffer->height * BYTES_PER_PIXEL;
	buffer->pixels = VirtualAlloc(NULL, buffer_data_size, MEM_COMMIT, PAGE_READWRITE);

	global_platform.window_width = width;
	global_platform.window_height = height;
}

void
Win32OutputError(char *title, char *format, ...)
{
	va_list args;
	va_start(args, format);
	u32 character_count = vsnprintf(0, 0, format, args)+1;
	va_end(args);

	local_persist char text[4096];

	character_count = MinU32(4096, character_count);

	va_start(args, format);
	vsnprintf(text, character_count, format, args);
	va_end(args);

	text[character_count - 1] = '\0';

	MessageBoxA(0, text, title, MB_OK);

}

u64
Win32KeyInput(u64 virtual_keycode)
{
	u64 key_input = 0;
	if ((virtual_keycode >= 'A' && virtual_keycode <= 'Z') ||
	    (virtual_keycode >= '0' && virtual_keycode <= '9'))
	{
		key_input = (virtual_keycode >= 'A' && virtual_keycode <= 'Z') ? KEY_a + (virtual_keycode-'A') : KEY_0 + (virtual_keycode-'0');
	}
	else
	{
		if (virtual_keycode == VK_ESCAPE)
		{
			key_input = KEY_esc;
		}
		else if (virtual_keycode >= VK_F1 && virtual_keycode <= VK_F12)
		{
			key_input = KEY_f1 + virtual_keycode - VK_F1;
		}
		else if (virtual_keycode == VK_BACK)
		{
			key_input = KEY_backspace;
		}
		else if (virtual_keycode == VK_TAB)
		{
			key_input = KEY_tab;
		}
		else if (virtual_keycode == VK_SPACE)
		{
			key_input = KEY_space;
		}
		else if (virtual_keycode == VK_RETURN)
		{
			key_input = KEY_enter;
		}
		else if (virtual_keycode == VK_LEFT)
		{
			key_input = KEY_left;
		}
		else if (virtual_keycode == VK_UP)
		{
			key_input = KEY_up;
		}
		else if (virtual_keycode == VK_RIGHT)
		{
			key_input = KEY_right;
		}
		else if (virtual_keycode == VK_DOWN)
		{
			key_input = KEY_down;
		}
		else if (virtual_keycode == VK_OEM_MINUS)
		{
			key_input = KEY_minus;
		}
		else if (virtual_keycode == VK_OEM_COMMA)
		{
			key_input = KEY_comma;
		}
		else if (virtual_keycode == VK_OEM_PERIOD)
		{
			key_input = KEY_period;
		}
		else if (virtual_keycode == VK_OEM_PLUS)
		{
			key_input = KEY_equal;
		}
		else if (virtual_keycode == VK_OEM_2)
		{
			key_input = KEY_slash;
		}
		else if (virtual_keycode == VK_OEM_7)
		{
			key_input = KEY_quote;
		}
		else if (virtual_keycode == VK_SHIFT)
		{
			key_input = KEY_shift;
		}
		else if (virtual_keycode == VK_CONTROL)
		{
			key_input = KEY_ctrl;
		}
		else if (virtual_keycode == VK_MENU)
		{
			key_input = KEY_alt;
		}
	}

	return key_input;
}

internal LRESULT WINAPI
Win32ProcessMessages(HWND window, UINT msg, WPARAM wparam, LPARAM lparam)
{
	LRESULT result = 0;
	if (msg == WM_CLOSE)
	{
		global_platform.running = 0;
	} 
	else if (msg == WM_SIZE)
	{
		RECT client_rect;
		GetClientRect(window, &client_rect);
		Win32ResizeBuffer(&global_pixel_buffer, client_rect.right - client_rect.left, client_rect.bottom - client_rect.top);
	}
	else if (msg == WM_LBUTTONDOWN)
	{
		if (!global_platform.left_mouse_down)
		{
			global_platform.left_mouse_pressed = 1;
		}
		global_platform.left_mouse_down = 1;
	} 
	else if (msg == WM_LBUTTONUP)
	{
		global_platform.left_mouse_down = 0;
		global_platform.left_mouse_pressed = 0;
	} 
	else if (msg == WM_RBUTTONDOWN)
	{
		if (!global_platform.right_mouse_down)
		{
			global_platform.right_mouse_pressed = 1;
		}
		global_platform.right_mouse_down = 1;
	} 
	else if (msg == WM_RBUTTONUP)
	{
		global_platform.right_mouse_down = 0;
		global_platform.right_mouse_pressed = 0;
	} 

	else if (msg == WM_KEYUP || msg == WM_SYSKEYUP)
	{
		u64 key_input = Win32KeyInput(wparam);
		global_platform.key_down[key_input] = 0;
		global_platform.key_pressed[key_input] = 0;
	} 
	else if (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN)
	{
		u64 key_input = Win32KeyInput(wparam);
		if (!global_platform.key_down[key_input])
		{
			global_platform.key_pressed[key_input] = 1;
		}
		global_platform.key_down[key_input] = 1;
		global_platform.last_key = (i32)key_input;

		if (key_input == KEY_f4 && global_platform.key_down[KEY_alt])
		{
			global_platform.running = 0;
		}
	} 
	else
	{
		result = DefWindowProc(window, msg, wparam, lparam);
	}

	return result;
}

// Entry point
int WINAPI
WinMain(HINSTANCE instance,
        HINSTANCE prev_instance,
        LPSTR cmd_line,
        int show_cmd)
{
	WNDCLASSEX window_class = {0};	
	{
		window_class.cbSize = sizeof(WNDCLASSEX); 
		window_class.style = CS_HREDRAW | CS_VREDRAW;
		window_class.lpfnWndProc = Win32ProcessMessages;
		window_class.hInstance = instance;
		window_class.lpszClassName = WINDOW_TITLE "WindowClass";
		RegisterClassEx(&window_class);
	}

	HWND window;
	{
		DWORD window_style = WS_SIZEBOX | WS_CAPTION | WS_BORDER | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU;
		RECT window_rect = {0};
		window_rect.right = DEFAULT_WINDOW_WIDTH;
		window_rect.bottom = DEFAULT_WINDOW_HEIGHT;

		AdjustWindowRectEx(&window_rect, window_style, 0, 0);

		window = CreateWindowEx(0,
					window_class.lpszClassName,
					WINDOW_TITLE,
					window_style,
					CW_USEDEFAULT, CW_USEDEFAULT,
					window_rect.right - window_rect.left,
					window_rect.bottom - window_rect.top,
					0, 0, 
					instance,
					0);
		Win32ResizeBuffer(&global_pixel_buffer, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
	}

	if (!window)
	{
		Win32OutputError("Fatal Error", "Window creation failed.");
		return 1;
	}


	// Set platform initial values
	{
		global_platform.initialized = 0;
		global_platform.running = 1;
		global_platform.window_width = DEFAULT_WINDOW_WIDTH;
		global_platform.window_height = DEFAULT_WINDOW_HEIGHT;
		global_platform.window_resized = 0;

		// Allocate game memory
		{
			global_platform.permanent_storage_size = PERMANENT_STORAGE_SIZE;
			global_platform.scratch_storage_size = SCRATCH_STORAGE_SIZE;
			u64 total_size = global_platform.permanent_storage_size + global_platform.scratch_storage_size;
			global_platform.permanent_storage = VirtualAlloc(0, total_size, 
									 MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

			if (!global_platform.permanent_storage)
			{
				Win32OutputError("Fatal Error", "Game memory allocation failed.");
				return 1;
			}

			global_platform.scratch_storage = (u8 *)global_platform.permanent_storage +
				global_platform.permanent_storage_size;

		}

		global_platform.OutputError = Win32OutputError;
		global_platform.SetPixel = Win32SetPixel;
	}
	
	LARGE_INTEGER performance_counter_frequency;
	LARGE_INTEGER begin_frame_time;
	LARGE_INTEGER end_frame_time;

	UINT minimum_sleep_ms = 1;
	b32 sleep_is_granular = timeBeginPeriod(minimum_sleep_ms) == TIMERR_NOERROR;
	QueryPerformanceFrequency(&performance_counter_frequency);

	ShowWindow(window, show_cmd);

	while (global_platform.running)
	{
		QueryPerformanceCounter(&begin_frame_time);

		MSG msg = {0};
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				global_platform.running = 0;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}



		// Update mouse coordinates
		{
			POINT pt;
			GetCursorPos(&pt);		
			ScreenToClient(window, &pt);
			global_platform.mouse_x = pt.x;
			global_platform.mouse_y = pt.y;
		}


		AppUpdate(&global_platform);

		HDC device_context = GetDC(window);
		Win32DisplayBuffer(device_context, &global_pixel_buffer);
		ReleaseDC(window, device_context);

		QueryPerformanceCounter(&end_frame_time);
		// Hit target frame rate
		{
			i64 desired_tick_count = (i64)performance_counter_frequency.QuadPart / global_platform.target_frames_per_second;
			i64 tick_count = end_frame_time.QuadPart - begin_frame_time.QuadPart;
			i64 wait_time = desired_tick_count - tick_count;

			LARGE_INTEGER begin_wait_time;
			LARGE_INTEGER end_wait_time;

			QueryPerformanceCounter(&begin_wait_time);
			while (wait_time > 0)
			{

				if (sleep_is_granular)
				{
					DWORD sleep_time_ms = (DWORD)(1000.0 * ((f64)(wait_time) / performance_counter_frequency.QuadPart));
					if (sleep_time_ms > minimum_sleep_ms)
					{
						sleep_time_ms -= minimum_sleep_ms;
						Sleep(sleep_time_ms);
					}
				}
				QueryPerformanceCounter(&end_wait_time);
				wait_time -= end_wait_time.QuadPart - begin_wait_time.QuadPart;
				begin_wait_time = end_wait_time;
			}

		}
		
		global_platform.current_time += 1.0f / global_platform.target_frames_per_second;

	}
	timeEndPeriod(minimum_sleep_ms);
	return 0;
}


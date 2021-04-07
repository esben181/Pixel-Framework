typedef struct GameState GameState;
struct GameState
{
	v2 player_pos;
};

internal void
GameStateInit(GameState *game)
{
	game->player_pos.x = 0;
	game->player_pos.y = 0;
}

internal StateType
GameStateUpdate(GameState *game)
{
	StateType next_state = 0;
	// draw/clear background color
	for (u32 j = 0; j < platform->window_height; ++j)
		for (u32 i = 0; i < platform->window_width; ++i)
		{
			platform->SetPixel(i, j, 0xff00ffff);
		}

	
	if (platform->key_down[KEY_left])
	{
		game->player_pos.x -= 1;
	}
	if (platform->key_down[KEY_right])
	{
		game->player_pos.x += 1;
	}
	if (platform->key_down[KEY_up])
	{
		game->player_pos.y -= 1;
	}
	if (platform->key_down[KEY_down])
	{
		game->player_pos.y += 1;
	}
	// draw player
	u32 width = 50, height = 50;
	for (u32 j = 0; j < width; ++j)
		for (u32 i = 0; i < height; ++i)
		{
			platform->SetPixel(i + game->player_pos.x, j+game->player_pos.y, 0x00ff00ff);
		}


	return next_state;
}
internal void
GameStateCleanUp(GameState *game)
{
}

typedef struct MemoryArena MemoryArena;
struct MemoryArena
{
	void *memory;
	u64 memory_size;
	u64 memory_allocate_offset;
	u64 memory_left;
};

internal MemoryArena
MemoryArenaInit(void *memory, u64 memory_size)
{
	MemoryArena m = {0};
	m.memory = memory;
	m.memory_size = memory_size;
	m.memory_left = memory_size;
	return m;
}

internal void *
MemoryArenaAllocate(MemoryArena *arena, u64 size)
{
	void *memory = 0;

	Assert(arena->memory_allocate_offset + size <= arena->memory_size);

	memory = (void *)((u8 *)arena->memory + arena->memory_allocate_offset);
	arena->memory_allocate_offset += size;
	arena->memory_left -= size;

	return memory;
}

internal void
MemoryArenaClear(MemoryArena *arena)
{
	arena->memory_allocate_offset = 0;
	arena->memory_left = arena->memory_size;
}

internal void
MemoryArenaZero(MemoryArena *arena)
{
	MemorySet(arena->memory, 0, arena->memory_size);
}

internal char *
MemoryArenaCreateString(MemoryArena *arena, char *format, ...)
{
	va_list args;
	va_start(args, format);
	u32 character_count = vsnprintf(0, 0, format, args) + 1;
	va_end(args);

	char *string = MemoryArenaAllocate(arena, character_count);
	SoftAssert(string);
	if (string)
	{
		va_start(args, format);
		vsnprintf(string, character_count, format, args);
		va_end(args);
		string[character_count - 1] = '\0';
	}

	return string;

}

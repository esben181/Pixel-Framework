#define Assert FatalAssert
#define FatalAssert(b) if (!(b)) { _Assert(#b, __FILE__, __LINE__, 1); }
#define SoftAssert(b)  if (!(b)) { _Assert(#b, __FILE__, __LINE__, 0); }

internal void
_Assert(char *expression, char *filename, int line, b32 fatal)
{
	if (fatal)
	{
		platform->OutputError("Fatal Assertion", "Assertion of %s at %s:%d failed. Crashing program...",
				      expression, filename, line);
		*(int *)0 = 0;
	}
	else
	{
		platform->OutputError("Assertion Error", "Soft assertion of %s at %s:%d failed.",
				      expression, filename, line);
	}
}

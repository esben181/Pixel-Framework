#include <stdint.h>
#include <string.h>
#include <math.h>

#define global static
#define internal static
#define local_persist static


typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef i8 b8;
typedef i16 b16;
typedef i32 b32;
typedef i64 b64;
typedef float f32;
typedef double f64;

typedef u32 color_t;


#define Bytes(n)     ((u64)n)
#define Kilobytes(n) (Bytes((u64)n)*1024u)
#define Megabytes(n) (Kilobytes((u64)n)*1024u)
#define Gigabytes(n) (Megabytes((u64)n)*1024u)
#define PIf 3.14159265359f

#define MemorySet memset
#define SinF32    sinf
#define SinF64    sin
#define ArcsinF32  asinf
#define ArcsinF64  asin
#define CStringLength strlen // TODO(esben): change name
#define CStringConcatenate strcat // TODO(esben): change name
#define Abs abs
#define MaxNumber(a, b) (a > b ? a : b)

// DJB2 hash algorithm
internal u32
HashCString(char *str)
{
	u32 hash = 5381;
	int c;
	while (c = *str++)
	{
		hash = ((hash << 5) + hash )+ c; 
	}
	return hash;
}

internal f32
MaxF32(f32 a, f32 b)
{
	return a > b ? a : b;
}

internal f32
MinF32(f32 a, f32 b)
{
	return a > b ? b : a;
}

internal f32
ClampF32(f32 val, f32 min, f32 max)
{
	return MaxF32(min, MinF32(val, max));
}

internal u32
MinU32(u32 a, u32 b)
{
	return a > b ? b : a;
}

typedef union v2
{
	struct
	{
		f32 x, y;
	};
	struct
	{
		f32 width, height;
	};

} v2;

typedef union v3
{
	struct 
	{
		f32 x, y, z;
	};
	struct
	{
		f32 r, g, b;
	};
	f32 element[3];
} v3;

typedef union v4
{
	struct
	{
		f32 x, y;
		union
		{
			struct 
			{
				f32 width, height;
			};
			struct
			{
				f32 z;
				union
				{
					f32 w;
					f32 radius;
				};
			};
		};
	};
	struct
	{
		f32 r, g, b, a;
	};
	f32 element[4];
} v4;
#define color4 v4

typedef struct m4
{
	f32 elements[4][4]; // (i, j): i = row; j = column
} m4;


internal v2
V2Init(f32 x, f32 y)
{
	v2 v = { x, y };
	return v;
};
#define v2(x, y) V2Init(x, y)

internal v2
V2AddV2(v2 a, v2 b)
{
	a.x += b.x;
	a.y += b.y;

	return a;
}

internal v2
V2SubV2(v2 a, v2 b)
{
	a.x -= b.x;
	a.y -= b.y;

	return a;
}

internal v3
V3Init(f32 x, f32 y, f32 z)
{
	v3 v = { x, y, z };
	return v;
}
#define v3(x, y, z) V3Init(x, y, z)

internal v4
V4Init(f32 x, f32 y, f32 z, f32 w)
{
	v4 v = { x, y, z, w };
	return v;
}
#define v4(x, y, z, w) V4Init(x, y, z, w)

internal v4
V4AddF32(v4 a, f32 b)
{
	a.x += b;
	a.y += b;
	a.z += b;
	a.w += b;

	return a;
}
internal v3
V3AddV3(v3 a, v3 b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;

	return a;
}

internal b32
V2InRect(v2 pt, v4 rect)
{
	return ((pt.x >= rect.x && pt.x <= rect.x + rect.width) &&
	    (pt.y >= rect.y && pt.y <= rect.y + rect.height));
}

internal f32
Deg2Rad(f32 degrees)
{
	return degrees * (PIf / 180.f);
}

internal f32
Rad2Deg(f32 radians)
{
	return radians * (180 / PIf);
}
internal m4
M4Init(f32 diagonal)
{
	m4 m = {
		{
			{ diagonal, 0, 0, 0 },
			{ 0, diagonal, 0, 0 },
			{ 0, 0, diagonal, 0 },
			{ 0, 0, 0, diagonal },
		}
	};
	return m;
}

internal m4
M4MultiplyF32(m4 matrix, f32 scalar)
{
	for (u32 j = 0; j < 4; ++j)
	{
		for (u32 i = 0; i < 4; ++i)
		{
			matrix.elements[i][j] *= scalar;
		}
	}
	return matrix;
}

internal m4
M4MultiplyM4(m4 a, m4 b)
{
	m4 result = {0};
	for (u32 j = 0; j < 4; ++j)
	{
		for (u32 i = 0; i < 4; ++i)
		{
			result.elements[i][j] = (a.elements[0][j]*b.elements[i][0] +
					    a.elements[1][j]*b.elements[i][1] +
					    a.elements[2][j]*b.elements[i][2] +
					    a.elements[3][j]*b.elements[i][3]);
		}
	}
	return result;
}

internal m4
M4TranslateV3(v3 translation)
{
	m4 m = M4Init(1.f);
	m.elements[3][0] = translation.x;
	m.elements[3][1] = translation.y;
	m.elements[3][2] = translation.z;

	return m;
}
internal m4
M4ScaleV3(v3 scale)
{
	m4 m = M4Init(1.f);
	m.elements[0][0] = scale.x;
	m.elements[1][1] = scale.y;
	m.elements[2][2] = scale.z;

	return m;
}

internal m4
M4Ortho(i32 left, i32 right, i32 bottom, i32 top, f32 near_depth, f32 far_depth)
{
	m4 m = M4ScaleV3(v3(2.f / (right - left), 2.f / (top - bottom), -2.f / (far_depth - near_depth)));
	m.elements[3][0] = (left + right) / (left - right);
	m.elements[3][1] = (bottom + top) / (bottom - top);
	m.elements[3][2] = (far_depth + near_depth) / (near_depth - far_depth);
	return m;

}


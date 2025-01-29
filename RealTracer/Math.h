#pragma once
#include "precomp.h"

#include "Vec3.h"
#include "Color.h"
#include "Random.h"

#include "Interval.h"

using uint = unsigned int;

constexpr float PI = 3.1415926535897932385f;

inline float DegToRad(float degrees)
{
	return degrees * PI / 180.f;
}
inline float RadToDeg(float radians)
{
	return radians / PI / 180.f;
}



inline float Lerp(float start, float end, float a)
{
	return(1 - a) * start + a * end;
}
inline Vec3 Lerp(Vec3 start, Vec3 end, float a)
{
	float x = Lerp(start.x(), end.x(), a);
	float y = Lerp(start.y(), end.y(), a);
	float z = Lerp(start.z(), end.z(), a);
	return Vec3(x, y, z);
}

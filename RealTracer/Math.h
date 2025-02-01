#pragma once
#include "precomp.h"

#include "Vec3.h"
#include "Vec3Single.h"
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



inline xs::batch<float> Lerp(xs::batch<float> start, xs::batch<float> end, xs::batch<float> a)
{
	return(1 - a) * start + a * end;
}
inline Vec3Group Lerp(Vec3Group start, Vec3Group end, xs::batch<float> a)
{
	xs::batch<float> x = Lerp(start.x, end.x, a);
	xs::batch<float> y = Lerp(start.y, end.y, a);
	xs::batch<float> z = Lerp(start.z, end.z, a);
	return Vec3Group(x, y, z);
}

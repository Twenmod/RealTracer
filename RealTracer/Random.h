#pragma once
#include "precomp.h"
using uint = unsigned int;


static uint seed = 0x12345678;

inline uint WangHash(uint s)
{
	s = (s ^ 61) ^ (s >> 16);
	s *= 9, s = s ^ (s >> 4);
	s *= 0x27d4eb2d;
	s = s ^ (s >> 15);
	return s;
}
inline uint InitSeed(uint seedBase)
{
	return WangHash((seedBase + 1) * 17);
}

inline uint RandomUInt()
{
	seed ^= seed << 13;
	seed ^= seed >> 17;
	seed ^= seed << 5;
	return seed;
}

// RandomFloat()
// Calculate a random unsigned int and cast it to a float in the range
// [0..1)
inline float RandomFloat() { return RandomUInt() * 2.3283064365387e-10f; }
inline float Rand(float range) { return RandomFloat() * range; }
inline uint RandInt(uint range) { return RandomUInt() % range; }

//Range is [min, max)
inline float Rand(float min, float max)
{
	return min + (max - min) * RandomFloat();
}
#pragma once
#include "Interval.h"
#include "Vec3.h"

using Color = Vec3;

inline float LinearToGammaSpace(float _linear)
{
	if (_linear > 0)
	{
		return std::sqrt(_linear);
	}
	return 0;
}

inline void WriteColor(std::ostream& out, float _r, float _g, float _b)
{
	auto r = LinearToGammaSpace(_r);
	auto g = LinearToGammaSpace(_g);
	auto b = LinearToGammaSpace(_b);

	// Translate the [0,1] component values to the byte range [0,255].
	static const Interval intensity(0.f, 0.999f);
	int rbyte = int(256 * intensity.Clamp(r));
	int gbyte = int(256 * intensity.Clamp(g));
	int bbyte = int(256 * intensity.Clamp(b));

	// Write out the pixel color components.
	out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}

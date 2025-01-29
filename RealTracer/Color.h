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

inline void WriteColor(std::ostream& out, const Color& pixel_color)
{
	auto r = LinearToGammaSpace(pixel_color.x());
	auto g = LinearToGammaSpace(pixel_color.y());
	auto b = LinearToGammaSpace(pixel_color.z());

	// Translate the [0,1] component values to the byte range [0,255].
	static const Interval intensity(0.f, 0.999f);
	int rbyte = int(256 * intensity.Clamp(r));
	int gbyte = int(256 * intensity.Clamp(g));
	int bbyte = int(256 * intensity.Clamp(b));

	// Write out the pixel color components.
	out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}

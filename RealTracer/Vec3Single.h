#pragma once

#include <cmath>
#include <iostream>
#include "Random.h"

class Vec3Single
{
public:
	float values[3];

	Vec3Single() : values{ 0,0,0 } {}
	Vec3Single(float x, float y, float z) : values{ x, y, z } {}
	Vec3Single(float scalar) : values{ scalar, scalar, scalar } {}

	float x() const { return values[0]; }
	float y() const { return values[1]; }
	float z() const { return values[2]; }

	Vec3Single operator-() const { return Vec3Single(-values[0], -values[1], -values[2]); }
	float operator[](int i) const { return values[i]; }
	float& operator[](int i) { return values[i]; }

	Vec3Single& operator+=(const Vec3Single& v)
	{
		values[0] += v.values[0];
		values[1] += v.values[1];
		values[2] += v.values[2];
		return *this;
	}

	Vec3Single& operator*=(float t)
	{
		values[0] *= t;
		values[1] *= t;
		values[2] *= t;
		return *this;
	}

	Vec3Single& operator/=(float t)
	{
		return *this *= 1 / t;
	}

	float Length() const
	{
		return std::sqrt(Length2());
	}

	float Length2() const
	{
		return values[0] * values[0] + values[1] * values[1] + values[2] * values[2];
	}

	bool NearZero() const
	{
		float treshold = 1e-8f;
		return (abs(values[0]) < treshold) && (abs(values[1]) < treshold) && (abs(values[2]) < treshold);
	}


	static Vec3Single Random()
	{
		return Vec3Single((RandomFloat() - 0.5f) * 2, (RandomFloat() - 0.5f) * 2, (RandomFloat() - 0.5f) * 2);
	}

	static Vec3Single Random(float min, float max)
	{
		return Vec3Single(Rand(min, max), Rand(min, max), Rand(min, max));
	}



};

// Vector Utility Functions

inline std::ostream& operator<<(std::ostream& out, const Vec3Single& v)
{
	return out << v.values[0] << ' ' << v.values[1] << ' ' << v.values[2];
}

inline Vec3Single operator+(const Vec3Single& u, const Vec3Single& v)
{
	return Vec3Single(u.values[0] + v.values[0], u.values[1] + v.values[1], u.values[2] + v.values[2]);
}

inline Vec3Single operator-(const Vec3Single& u, const Vec3Single& v)
{
	return Vec3Single(u.values[0] - v.values[0], u.values[1] - v.values[1], u.values[2] - v.values[2]);
}

inline Vec3Single operator*(const Vec3Single& u, const Vec3Single& v)
{
	return Vec3Single(u.values[0] * v.values[0], u.values[1] * v.values[1], u.values[2] * v.values[2]);
}

inline Vec3Single operator*(float t, const Vec3Single& v)
{
	return Vec3Single(t * v.values[0], t * v.values[1], t * v.values[2]);
}

inline Vec3Single operator*(const Vec3Single& v, float t)
{
	return t * v;
}

inline Vec3Single operator/(const Vec3Single& v, float t)
{
	return (1 / t) * v;
}

inline float dot(const Vec3Single& u, const Vec3Single& v)
{
	return u.values[0] * v.values[0]
		+ u.values[1] * v.values[1]
		+ u.values[2] * v.values[2];
}

inline Vec3Single cross(const Vec3Single& u, const Vec3Single& v)
{
	return Vec3Single(u.values[1] * v.values[2] - u.values[2] * v.values[1],
		u.values[2] * v.values[0] - u.values[0] * v.values[2],
		u.values[0] * v.values[1] - u.values[1] * v.values[0]);
}

inline Vec3Single Normalize(const Vec3Single& v)
{
	return v / v.Length();
}
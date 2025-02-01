#pragma once

#include <cmath>
#include <iostream>
#include "Random.h"

#undef min
#undef max

class Vec3Group
{
public:
	xs::batch<float> x;
	xs::batch<float> y;
	xs::batch<float> z;

	Vec3Group() {}
	Vec3Group(xs::batch<float> _x, xs::batch<float> _y, xs::batch<float> _z) : x(_x), y(_y), z(_z) {}
	Vec3Group(xs::batch<float> scalar) : x(scalar), y(scalar), z(scalar) {}

	Vec3Group operator-() const { return Vec3Group(-x, -y, -z); }

	Vec3Group& operator+=(const Vec3Group& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	Vec3Group& operator*=(float t)
	{
		x *= t;
		y *= t;
		z *= t;
		return *this;
	}

	Vec3Group& operator/=(float t)
	{
		return *this *= 1 / t;
	}

	xs::batch<float> Length() const
	{
		return sqrt(Length2());
	}

	xs::batch<float> Length2() const
	{
		return x * x + y * y + z * z;
	}

	xs::batch_bool<float> NearZero() const
	{
		float treshold = 1e-8f;
		return (abs(x) < treshold) && (abs(y) < treshold) && (abs(z) < treshold);
	}


	static Vec3Group Random()
	{
		return Vec3Group(RandomBatch() - 0.5f * 2.f, RandomBatch() - 0.5f * 2.f, RandomBatch() - 0.5f * 2.f);
	}

	static Vec3Group Random(xs::batch<float> min, xs::batch<float> max)
	{
		return Vec3Group(RandomBatch(min, max), RandomBatch(min, max), RandomBatch(min, max));
	}



};

// point3 is just an alias for vec3, but useful for geometric clarity in the code.
using Point3Group = Vec3Group;


// Vector Utility Functions

inline std::ostream& operator<<(std::ostream& out, const Vec3Group& v)
{
	return out << v.x << ' ' << v.y << ' ' << v.z;
}

inline Vec3Group operator+(const Vec3Group& u, const Vec3Group& v)
{
	return Vec3Group(u.x + v.x, u.y + v.y, u.z + v.z);
}

inline Vec3Group operator-(const Vec3Group& u, const Vec3Group& v)
{
	return Vec3Group(u.x - v.x, u.y - v.y, u.z - v.z);
}

inline Vec3Group operator*(const Vec3Group& u, const Vec3Group& v)
{
	return Vec3Group(u.x * v.x, u.y * v.y, u.z * v.z);
}

inline Vec3Group operator*(xs::batch<float> t, const Vec3Group& v)
{
	return Vec3Group(t * v.x, t * v.y, t * v.z);
}

inline Vec3Group operator*(const Vec3Group& v, xs::batch<float> t)
{
	return t * v;
}

inline Vec3Group operator/(const Vec3Group& v, xs::batch<float> t)
{
	return (1 / t) * v;
}

inline xs::batch<float> Dot(const Vec3Group& u, const Vec3Group& v)
{
	return u.x * v.x
		+ u.y * v.y
		+ u.z * v.z;
}

inline Vec3Group Cross(const Vec3Group& u, const Vec3Group& v)
{
	return Vec3Group(u.y * v.z - u.z * v.y,
		u.z * v.x - u.x * v.z,
		u.x * v.y - u.y * v.x);
}

inline Vec3Group Normalize(const Vec3Group& v)
{
	xs::batch<float> length = v.Length();
	xs::batch_bool<float> isZero = length <= 0.001f;
	Vec3Group normalized;
	normalized.x = xs::select(isZero, xs::batch<float>(0.f), v.x / length);
	normalized.y = xs::select(isZero, xs::batch<float>(0.f), v.y / length);
	normalized.z = xs::select(isZero, xs::batch<float>(0.f), v.z / length);

	return normalized;
}

inline Vec3Group RandomUnitVector()
{
	while (true)
	{
		Vec3Group vec = Vec3Group::Random(xs::batch<float>(-1), xs::batch<float>(1));

		return Normalize(vec);
	}
}

inline Vec3Group RandomCircleUnitVector()
{
	Vec3Group point = Vec3Group(RandomBatch(xs::batch<float> (- 1), xs::batch<float> (1)), RandomBatch(xs::batch<float>(-1), xs::batch<float>(1)), xs::batch<float>(0));
	return point / point.Length();
}

inline Vec3Group Reflect(const Vec3Group& vector, const Vec3Group& normal)
{
	return vector - 2.f * Dot(vector, normal) * normal;
}

inline Vec3Group Refract(const Vec3Group& vector, const Vec3Group& normal, xs::batch<float> relativeIOR)
{
	xs::batch<float> cosTheta = xs::min(Dot(-vector, normal), xs::batch<float> (1.0f));
	Vec3Group perpandicular = relativeIOR * (vector + cosTheta * normal);
	Vec3Group parallel = -sqrt(abs(1.f - perpandicular.Length2())) * normal;
	return perpandicular + parallel;
}
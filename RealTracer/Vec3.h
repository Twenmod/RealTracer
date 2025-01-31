#pragma once

#include <cmath>
#include <iostream>
#include "Random.h"

#undef min
#undef max

class Vec3
{
public:
	xs::batch<float> x;
	xs::batch<float> y;
	xs::batch<float> z;

	Vec3() {}
	Vec3(xs::batch<float> _x, xs::batch<float> _y, xs::batch<float> _z) : x(_x), y(_y), z(_z) {}
	Vec3(xs::batch<float> scalar) : x(scalar), y(scalar), z(scalar) {}

	Vec3 operator-() const { return Vec3(-x, -y, -z); }

	Vec3& operator+=(const Vec3& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	Vec3& operator*=(float t)
	{
		x *= t;
		y *= t;
		z *= t;
		return *this;
	}

	Vec3& operator/=(float t)
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


	static Vec3 Random()
	{
		return Vec3(RandomBatch() - 0.5f * 2.f, RandomBatch() - 0.5f * 2.f, RandomBatch() - 0.5f * 2.f);
	}

	static Vec3 Random(xs::batch<float> min, xs::batch<float> max)
	{
		return Vec3(RandomBatch(min, max), RandomBatch(min, max), RandomBatch(min, max));
	}



};

// point3 is just an alias for vec3, but useful for geometric clarity in the code.
using Point3 = Vec3;


// Vector Utility Functions

inline std::ostream& operator<<(std::ostream& out, const Vec3& v)
{
	return out << v.x << ' ' << v.y << ' ' << v.z;
}

inline Vec3 operator+(const Vec3& u, const Vec3& v)
{
	return Vec3(u.x + v.x, u.y + v.y, u.z + v.z);
}

inline Vec3 operator-(const Vec3& u, const Vec3& v)
{
	return Vec3(u.x - v.x, u.y - v.y, u.z - v.z);
}

inline Vec3 operator*(const Vec3& u, const Vec3& v)
{
	return Vec3(u.x * v.x, u.y * v.y, u.z * v.z);
}

inline Vec3 operator*(xs::batch<float> t, const Vec3& v)
{
	return Vec3(t * v.x, t * v.y, t * v.z);
}

inline Vec3 operator*(const Vec3& v, xs::batch<float> t)
{
	return t * v;
}

inline Vec3 operator/(const Vec3& v, xs::batch<float> t)
{
	return (1 / t) * v;
}

inline xs::batch<float> Dot(const Vec3& u, const Vec3& v)
{
	return u.x * v.x
		+ u.y * v.y
		+ u.z * v.z;
}

inline Vec3 Cross(const Vec3& u, const Vec3& v)
{
	return Vec3(u.y * v.z - u.z * v.y,
		u.z * v.x - u.x * v.z,
		u.x * v.y - u.y * v.x);
}

inline Vec3 Normalize(const Vec3& v)
{
	xs::batch<float> length = v.Length();
	xs::batch_bool<float> isZero = length <= 0.001f;
	Vec3 normalized;
	normalized.x = xs::select(isZero, xs::batch<float>(0.f), v.x / length);
	normalized.y = xs::select(isZero, xs::batch<float>(0.f), v.y / length);
	normalized.z = xs::select(isZero, xs::batch<float>(0.f), v.z / length);

	return normalized;
}

inline Vec3 RandomUnitVector()
{
	while (true)
	{
		Vec3 vec = Vec3::Random(xs::batch<float>(-1), xs::batch<float>(1));

		return Normalize(vec);
	}
}

inline Vec3 RandomCircleUnitVector()
{
	Vec3 point = Vec3(RandomBatch(xs::batch<float> (- 1), xs::batch<float> (1)), RandomBatch(xs::batch<float>(-1), xs::batch<float>(1)), xs::batch<float>(0));
	return point / point.Length();
}

inline Vec3 Reflect(const Vec3& vector, const Vec3& normal)
{
	return vector - 2.f * Dot(vector, normal) * normal;
}

inline Vec3 Refract(const Vec3& vector, const Vec3& normal, xs::batch<float> relativeIOR)
{
	xs::batch<float> cosTheta = xs::min(Dot(-vector, normal), xs::batch<float> (1.0f));
	Vec3 perpandicular = relativeIOR * (vector + cosTheta * normal);
	Vec3 parallel = -sqrt(abs(1.f - perpandicular.Length2())) * normal;
	return perpandicular + parallel;
}
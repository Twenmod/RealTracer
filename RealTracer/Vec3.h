#pragma once

#include <cmath>
#include <iostream>
#include "Random.h"

class Vec3
{
public:
	float values[3];

	Vec3() : values{ 0,0,0 } {}
	Vec3(float x, float y, float z) : values{ x, y, z } {}
	Vec3(float scalar) : values{ scalar, scalar, scalar } {}

	float x() const { return values[0]; }
	float y() const { return values[1]; }
	float z() const { return values[2]; }

	Vec3 operator-() const { return Vec3(-values[0], -values[1], -values[2]); }
	float operator[](int i) const { return values[i]; }
	float& operator[](int i) { return values[i]; }

	Vec3& operator+=(const Vec3& v)
	{
		values[0] += v.values[0];
		values[1] += v.values[1];
		values[2] += v.values[2];
		return *this;
	}

	Vec3& operator*=(float t)
	{
		values[0] *= t;
		values[1] *= t;
		values[2] *= t;
		return *this;
	}

	Vec3& operator/=(float t)
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

	bool NearZero() const{
		float treshold = 1e-8f;
		return (abs(values[0]) < treshold) && (abs(values[1]) < treshold) && (abs(values[2]) < treshold);
	}


	static Vec3 Random()
	{
		return Vec3((RandomFloat()-0.5f)*2, (RandomFloat() - 0.5f) * 2, (RandomFloat() - 0.5f) * 2);
	}

	static Vec3 Random(float min, float max)
	{
		return Vec3(Rand(min, max), Rand(min, max), Rand(min, max));
	}



};

// point3 is just an alias for vec3, but useful for geometric clarity in the code.
using Point3 = Vec3;


// Vector Utility Functions

inline std::ostream& operator<<(std::ostream& out, const Vec3& v)
{
	return out << v.values[0] << ' ' << v.values[1] << ' ' << v.values[2];
}

inline Vec3 operator+(const Vec3& u, const Vec3& v)
{
	return Vec3(u.values[0] + v.values[0], u.values[1] + v.values[1], u.values[2] + v.values[2]);
}

inline Vec3 operator-(const Vec3& u, const Vec3& v)
{
	return Vec3(u.values[0] - v.values[0], u.values[1] - v.values[1], u.values[2] - v.values[2]);
}

inline Vec3 operator*(const Vec3& u, const Vec3& v)
{
	return Vec3(u.values[0] * v.values[0], u.values[1] * v.values[1], u.values[2] * v.values[2]);
}

inline Vec3 operator*(float t, const Vec3& v)
{
	return Vec3(t * v.values[0], t * v.values[1], t * v.values[2]);
}

inline Vec3 operator*(const Vec3& v, float t)
{
	return t * v;
}

inline Vec3 operator/(const Vec3& v, float t)
{
	return (1 / t) * v;
}

inline float dot(const Vec3& u, const Vec3& v)
{
	return u.values[0] * v.values[0]
		+ u.values[1] * v.values[1]
		+ u.values[2] * v.values[2];
}

inline Vec3 cross(const Vec3& u, const Vec3& v)
{
	return Vec3(u.values[1] * v.values[2] - u.values[2] * v.values[1],
		u.values[2] * v.values[0] - u.values[0] * v.values[2],
		u.values[0] * v.values[1] - u.values[1] * v.values[0]);
}

inline Vec3 Normalize(const Vec3& v)
{
	return v / v.Length();
}

inline Vec3 RandomUnitVector()
{
	while (true)
	{
		Vec3 vec = Vec3::Random(-1,1);
		float length2 = vec.Length2();
		if (1e-32 < length2 && length2 <= 1)
		{
			return vec / sqrt(length2);
		}
	}
}
inline Vec3 RandomCircleUnitVector()
{
	while (true)
	{
		Vec3 point = Vec3(Rand(-1, 1), Rand(-1, 1), 0);
		if (point.Length2() < 1)
			return point;
	}
}


inline Vec3 RandomOnHemiSphere(const Vec3& _normal)
{
	Vec3 vec = RandomUnitVector();
	if (dot(vec, _normal) > 0.f)
	{
		return vec;
	}
	else
	{
		return -vec;
	}
}

inline Vec3 Reflect(const Vec3& vector, const Vec3& normal)
{
	return vector - 2.f * dot(vector, normal) * normal;
}

inline Vec3 Refract(const Vec3& vector, const Vec3& normal, float relativeIOR)
{
	float cosTheta = std::min(dot(-vector, normal), 1.f);
	Vec3 perpandicular = relativeIOR * (vector + cosTheta * normal);
	Vec3 parallel = -sqrt(abs(1.f - perpandicular.Length2())) * normal;
	return perpandicular + parallel;
}
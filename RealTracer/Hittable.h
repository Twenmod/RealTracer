#pragma once
#include "Ray.h"

class Material;

class HitInfo
{
public:
	Point3 m_point;
	Vec3 m_normal;
	Material* m_material;
	float m_t;
	bool m_frontFace;
	void SetNormal(const Ray& ray, const Vec3& normal);
};


class Hittable
{
public:
	virtual ~Hittable() = default;

	virtual bool Intersect(const Ray& ray, Interval rayT, HitInfo& outHit) const = 0;
};


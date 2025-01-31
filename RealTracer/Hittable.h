#pragma once
#include "Ray.h"

class Material;

class HitInfoGroup
{
public:
	HitInfoGroup() {};

	Vec3 point;
	Vec3 normal;
	xs::batch<float> t;
	xs::batch_bool<float> frontFace;
	xs::batch<int> material;

	void SetNormal(const RayGroup& _ray, Vec3 _normal)
	{
		frontFace = Dot(_ray.direction, _normal) < 0.f;
		normal.x = xs::select(frontFace, _normal.x, -_normal.x);
		normal.y = xs::select(frontFace, _normal.y, -_normal.y);
		normal.z = xs::select(frontFace, _normal.z, -_normal.z);
	}

};


class Hittable
{
public:
	virtual ~Hittable() = default;

	virtual xs::batch_bool<float> Intersect(const RayGroup& ray, IntervalGroup rayT, HitInfoGroup& outHit) const = 0;
};


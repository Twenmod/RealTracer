#pragma once
#include "Ray.h"
#include "AABB.h"

class Material;

class HitInfoGroup
{
public:
	HitInfoGroup() {};

	Vec3Group point;
	Vec3Group normal;
	xs::batch<float> t;
	xs::batch_bool<float> frontFace;
	xs::batch<float> u;
	xs::batch<float> v;
	xs::batch<int> material;

	void SetNormal(const RayGroup& _ray, Vec3Group _normal, xs::batch_bool<float> mask)
	{
		Vec3Group rayDir = Normalize(_ray.direction);
		xs::batch_bool<float> newFrontFace = Dot(rayDir, _normal) < 0.f;
		frontFace = (newFrontFace & mask) | (frontFace & ~mask);// basically select

		normal.x = xs::select(mask, xs::select(frontFace, _normal.x, -_normal.x), normal.x);
		normal.y = xs::select(mask, xs::select(frontFace, _normal.y, -_normal.y), normal.y);
		normal.z = xs::select(mask, xs::select(frontFace, _normal.z, -_normal.z), normal.z);
	}


};


class Hittable
{
public:
	virtual ~Hittable() = default;
	virtual xs::batch_bool<float> Intersect(const RayGroup& ray, IntervalGroup rayT, HitInfoGroup& outHit) const = 0;
	virtual const AABB& GetBoundingBox() const = 0;

};


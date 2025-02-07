#pragma once
#include "Hittable.h"

enum E_MATERIALS;

class Sphere :
	public Hittable
{
public:
	Sphere(E_MATERIALS material, Vec3 position, float radius);

	xs::batch_bool <float> Intersect(const RayGroup& ray, IntervalGroup rayT, HitInfoGroup& outHit) const override;
	const AABB& GetBoundingBox() const override
	{
		return m_boundingBox;
	};
	Vec3 position;
	float radius;
private:
	void CalcUV(const Vec3Group& direction, xs::batch<float>& outU, xs::batch<float>& outV) const;
	E_MATERIALS material;
	AABB m_boundingBox;
};


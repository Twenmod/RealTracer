#pragma once
#include "Hittable.h"
class Quad :
    public Hittable
{
public:
	Quad(E_MATERIALS material, const Point3& cornerPos, const Vec3& u = Vec3(0,0,1), const Vec3& v = Vec3(0, 1, 0));

	xs::batch_bool <float> Intersect(const RayGroup& ray, IntervalGroup rayT, HitInfoGroup& outHit) const override;

private:
	E_MATERIALS material;
	Point3 corner;
	Vec3 u, v;
	Vec3 w;
	Vec3 normal;
	float d;
};


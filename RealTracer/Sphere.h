#pragma once
#include "Hittable.h"

enum E_MATERIALS;

class Sphere :
	public Hittable
{
public:
	Sphere(E_MATERIALS material, float posX, float posY,float posZ, float radius);

	xs::batch_bool <float> Intersect(const RayGroup& ray, IntervalGroup rayT, HitInfoGroup& outHit) const override;
	float posX;
	float posY;
	float posZ;
	float radius;
private:
	E_MATERIALS material;
};


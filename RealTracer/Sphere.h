#pragma once
#include "Hittable.h"
class Sphere :
	public Hittable
{
public:
	Sphere(Material& material, const Point3 position, float radius);

	bool Intersect(const Ray& ray, Interval rayT, HitInfo& outHit) const override;
private:
	Point3 m_position;
	float m_radius;
	Material& m_material;
};


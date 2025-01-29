#include "precomp.h"
#include "Common.h"
#include "Sphere.h"

Sphere::Sphere(Material& _material, const Point3 _position, float _radius) :
	m_material(_material)
{
	m_position = _position;
	m_radius = abs(_radius);
}

bool Sphere::Intersect(const Ray& _ray, Interval _rayT, HitInfo& _outHit) const
{
	Vec3 oc = m_position - _ray.GetOrigin();
	float a = _ray.GetDirection().Length2();
	float h = dot(_ray.GetDirection(), oc);
	float c = oc.Length2() - m_radius * m_radius;
	float discriminant = h * h - a * c;

	if (discriminant < 0)
	{
		return false;
	}

	float sqrtd = std::sqrt(discriminant);

	//Find nearest root
	float root = (h - sqrtd) / a;
	if (!_rayT.Surrounds(root))
	{
		root = (h + sqrtd) / a;
		if (!_rayT.Surrounds(root))
			return false;
	}

	_outHit.m_t = root;
	Vec3 point = _ray.At(root);
	_outHit.m_point = point;
	Vec3 outNormal = (point -m_position) / m_radius;
	_outHit.SetNormal(_ray, outNormal);
	_outHit.m_material = &m_material;
	return true;
}

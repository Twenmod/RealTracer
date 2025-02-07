#include "precomp.h"
#include "Common.h"
#include "Sphere.h"
#include "Material.h"

Sphere::Sphere(E_MATERIALS _material, Vec3 _position, float _radius)
{
	material = _material;
	position = _position;
	radius = _radius;
	m_boundingBox = AABB(_position - Vec3(radius), _position + Vec3(radius));
}

xs::batch_bool<float> Sphere::Intersect(const RayGroup& _ray, IntervalGroup _rayT, HitInfoGroup& _outHit) const
{
	Vec3Group oc;
	oc.x = position.x() - _ray.origin.x;
	oc.y = position.y() - _ray.origin.y;
	oc.z = position.z() - _ray.origin.z;
	xs::batch<float> a = _ray.direction.Length2();
	xs::batch<float> h = Dot(_ray.direction, oc);
	xs::batch<float> c = oc.Length2() - radius * radius;
	xs::batch<float> discriminant = h * h - a * c;

	xs::batch_bool<float> noIntersection = discriminant < xs::batch<float>(0.f);

	//All miss
	if (xs::all(noIntersection))
	{
		return !noIntersection; // return false for all
	}

	xs::batch<float> sqrtd = xs::sqrt(discriminant);

	//Find nearest root
	xs::batch<float> root1 = (h - sqrtd) / a;
	xs::batch_bool<float> root1Valid = _rayT.Surrounds(root1);

	xs::batch<float> root2 = (h + sqrtd) / a;
	xs::batch_bool<float> root2Valid = _rayT.Surrounds(root2);

	xs::batch<float> root = xs::select(root1Valid, root1, root2);
	xs::batch_bool<float> validRoot = root1Valid || root2Valid;

	if (xs::all(!validRoot))
	{
		return validRoot; // return false for all
	}

	_outHit.t = root;
	
	Vec3Group point = _ray.At(root);
	_outHit.point.x = xs::select(validRoot, point.x, _outHit.point.x);
	_outHit.point.y = xs::select(validRoot, point.y, _outHit.point.y);
	_outHit.point.z = xs::select(validRoot, point.z, _outHit.point.z);
	Vec3Group outNormal;
	outNormal.x = (point.x - position.x()) / radius;
	outNormal.y = (point.y - position.y()) / radius;
	outNormal.z = (point.z - position.z()) / radius;
	outNormal = Normalize(outNormal);
	_outHit.SetNormal(_ray, outNormal, validRoot);
	xs::batch<float> u, v;
	CalcUV(outNormal, u, v);
	_outHit.u = xs::select(validRoot, u, _outHit.u);
	_outHit.v = xs::select(validRoot, v, _outHit.v);

	_outHit.material = xs::select(xs::batch_bool_cast<int>(validRoot), xs::batch<int>(material), _outHit.material);
	return validRoot;
}

void Sphere::CalcUV(const Vec3Group& _direction, xs::batch<float>& _outU, xs::batch<float>& _outV) const
{
	xs::batch<float> theta = acos(-_direction.y);
	xs::batch<float> phi = atan2(-_direction.z, _direction.x) + PI;

	_outU = phi / (2 * PI);
	_outV = theta / PI;
}


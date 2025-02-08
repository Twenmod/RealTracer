#include "precomp.h"
#include "Quad.h"

Quad::Quad(E_MATERIALS _material, const Point3& _cornerPos, const Vec3& _u, const Vec3& _v) 
	: material(_material)
	, corner(_cornerPos)
	, u(_u)
	, v(_v)
{
	Vec3 n = cross(u, v);
	normal = Normalize(n);
	d = dot(normal, _cornerPos);
	w = n / dot(n, n);


	// Compute the bounding box around entire quad
	AABB bbox_diagonal1 = AABB(corner, corner + u + v);
	AABB bbox_diagonal2 = AABB(corner + u, corner + v);
	m_boundingBox = AABB(bbox_diagonal1, bbox_diagonal2);
}

xs::batch_bool<float> Quad::Intersect(const RayGroup& _ray, IntervalGroup _rayT, HitInfoGroup& _outHit) const
{
	Vec3Group normalGroup = normal.Group();
	xs::batch<float> denom = Dot(normalGroup, _ray.direction);

	// No hit if the ray is parallel to the plane.
	xs::batch_bool<float> parallel = abs(denom) < 1e-8;
	if (xs::all(parallel))
	{
		return ~parallel; // return false for all
	}

	// Return false if the hit point parameter t is outside the ray interval.
	xs::batch<float> t = (d - Dot(normalGroup, _ray.origin)) / denom;
	xs::batch_bool<float> intersects = _rayT.Contains(t);
	intersects = intersects & ~parallel;

	if (xs::all(!intersects))
	{
		return intersects;
	}

	// Determine if the hit point lies within the planar shape using its plane coordinates.
	Vec3Group intersection = _ray.At(t);
	Vec3Group planarHitptVector = intersection - corner.Group();
	xs::batch<float> alpha = Dot(w.Group(), Cross(planarHitptVector, v.Group()));
	xs::batch<float>beta = Dot(w.Group(), Cross(u.Group(), planarHitptVector));

	IntervalGroup unit_interval = IntervalGroup(xs::batch<float>(0), xs::batch<float>(1));
	// Given the hit point in plane coordinates, return false if it is outside the
	// primitive, otherwise set the hit record UV coordinates and return true.

	xs::batch_bool<float> outSidePrimitve = !unit_interval.Contains(alpha) | !unit_interval.Contains(beta);

	intersects = intersects & ~outSidePrimitve;
	if (xs::all(!intersects))
	{
		return intersects;
	}

	_outHit.u = xs::select(intersects, alpha, _outHit.u);
	_outHit.v = xs::select(intersects, beta, _outHit.v);

	// Ray hits the 2D shape; set the rest of the hit record and return true.
	_outHit.t = xs::select(intersects, t, _outHit.t);
	_outHit.point.x = xs::select(intersects, intersection.x, _outHit.point.x);
	_outHit.point.y = xs::select(intersects, intersection.y, _outHit.point.y);
	_outHit.point.z = xs::select(intersects, intersection.z, _outHit.point.z);
	_outHit.material = xs::select(xs::batch_bool_cast<int>(intersects), xs::batch<int>(material), _outHit.material);
	_outHit.SetNormal(_ray, normalGroup, intersects);

	return intersects;
}

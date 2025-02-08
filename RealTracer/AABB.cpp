#include "precomp.h"
#include "Common.h"
#include "AABB.h"

#include "Ray.h"


xs::batch_bool<float> AABB::Intersects(const RayGroup& _ray, IntervalGroup _rayT) const
{
	const Point3Group& origin = _ray.origin;
	const Vec3Group& direction = _ray.direction;

	xs::batch_bool<float> intersects(true);
	intersects = intersects & IntersectsAxis(origin.x, direction.x, _rayT, m_x);
	if (xs::all(!intersects)) return intersects;
	intersects = intersects & IntersectsAxis(origin.y, direction.y, _rayT, m_y);
	if (xs::all(!intersects)) return intersects;
	intersects = intersects & IntersectsAxis(origin.z, direction.z, _rayT, m_z);

	return intersects;
}

xs::batch_bool<float> AABB::IntersectsAxis(const xs::batch<float>& _rayAxisOrigin, const xs::batch<float>& _rayAxisDirection, IntervalGroup& _rayT, const IntervalGroup& _axisInterval) const
{
	const xs::batch<float> invDir = 1.f / _rayAxisDirection;

	xs::batch<float> t0 = (_axisInterval.min - _rayAxisOrigin) * invDir;
	xs::batch<float> t1 = (_axisInterval.max - _rayAxisOrigin) * invDir;

	xs::batch_bool<float> t0Smaller = t0 < t1;
	
	xs::batch_bool<float> t0Fits = (t0 > _rayT.min & t0Smaller) | (t0 < _rayT.max & ~t0Smaller);
	xs::batch_bool<float> t1Fits = (t1 > _rayT.min & ~t0Smaller) | (t1 < _rayT.max & t0Smaller);

	_rayT.min = xs::select(t0Smaller, xs::select(t0Fits,t0,_rayT.min), xs::select(t1Fits,t1,_rayT.min));
	_rayT.max = xs::select(t0Smaller, xs::select(t1Fits,t1,_rayT.max), xs::select(t0Fits,t0,_rayT.max));

	return _rayT.max > _rayT.min;
}

const AABB AABB::empty = AABB(IntervalGroup::empty, IntervalGroup::empty, IntervalGroup::empty);
const AABB AABB::universe = AABB(IntervalGroup::universe, IntervalGroup::universe, IntervalGroup::universe);
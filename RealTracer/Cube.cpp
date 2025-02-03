#include "precomp.h"
#include "Cube.h"
//
//xs::batch_bool<float> Cube::Intersect(const RayGroup& ray, IntervalGroup rayT, HitInfoGroup& outHit) const
//{
//	xs::batch_bool<float> xIntersects = IntersectsAxis(ray.direction.x, ray.origin.x, rayT, m_xInterval);
//	xs::batch_bool<float> yIntersects = IntersectsAxis(ray.direction.y, ray.origin.y, rayT, m_yInterval);
//	xs::batch_bool<float> zIntersects = IntersectsAxis(ray.direction.z, ray.origin.z, rayT, m_zInterval);
//
//	xs::batch<float> hitT
//
//
//
//	return xIntersects | yIntersects | zIntersects; // Return if any axis intersects
//
//}
//
//void Cube::UpdateIntervals()
//{
//}
//
//xs::batch_bool<float> Cube::IntersectsAxis(xs::batch<float> _rayAxisDirection, xs::batch<float> _rayAxisOrigin, IntervalGroup _rayInterval, IntervalGroup _cubeAxisInterval) const
//{
//	const xs::batch<float> adinv = 1.0 / _rayAxisDirection;
//
//	xs::batch<float> t0 = (_cubeAxisInterval.min - _rayAxisOrigin) * adinv;
//	xs::batch<float> t1 = (_cubeAxisInterval.max - _rayAxisOrigin) * adinv;
//
//	xs::batch_bool<float> t0Smaller = t0 < t1;
//
//	xs::batch_bool<float> t0InLimit = (t0Smaller & (t0 > _rayInterval.min)) || (~t0Smaller) & (t0 < _rayInterval.max);
//	xs::batch_bool<float> t1InLimit = (t0Smaller & (t1 < _rayInterval.max)) || (~t0Smaller) & (t1 > _rayInterval.min);
//
//	_rayInterval.min = xs::select(t0Smaller,
//		xs::select(t0InLimit, t0, _rayInterval.min),
//		xs::select(t1InLimit, t1, _rayInterval.min));
//	_rayInterval.max = xs::select(t0Smaller,
//		xs::select(t1InLimit, t1, _rayInterval.min),
//		xs::select(t0InLimit, t0, _rayInterval.min));
//
//	return !(_rayInterval.max <= _rayInterval.min);
//}

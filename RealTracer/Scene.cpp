#include "precomp.h"
#include "Common.h"
#include "Scene.h"

Scene::~Scene()
{
	for (size_t i = 0; i < m_objects.size(); i++)
	{
		delete m_objects[i];
	}
}

xs::batch_bool<float> Scene::Intersect(const RayGroup& ray, IntervalGroup _rayT, HitInfoGroup& outHit) const
{
	xs::batch_bool<float> hitAny(false);
	xs::batch<float> closestHit = _rayT.max;

	for (const Hittable* object : m_objects)
	{
		HitInfoGroup hits;
		xs::batch_bool<float> intersections = object->Intersect(ray, IntervalGroup(_rayT.min, closestHit), hits);

		hitAny = hitAny | intersections;

		closestHit = xs::select(intersections, hits.t, closestHit);
		outHit.point.x = xs::select(intersections, outHit.point.x, hits.point.x);
		outHit.point.y = xs::select(intersections, outHit.point.y, hits.point.y);
		outHit.point.z = xs::select(intersections, outHit.point.z, hits.point.z);
		outHit.normal.x = xs::select(intersections, outHit.normal.x, hits.normal.x);
		outHit.normal.y = xs::select(intersections, outHit.normal.y, hits.normal.y);
		outHit.normal.z = xs::select(intersections, outHit.normal.z, hits.normal.z);
		outHit.frontFace = intersections & hits.frontFace;
		outHit.t = xs::select(intersections, outHit.t, hits.t);
		outHit.material = xs::select(xs::batch_bool_cast<int>(intersections), outHit.material, hits.material);
		outHit = hits;

	}
	return hitAny;
}

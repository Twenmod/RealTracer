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

bool Scene::Intersect(const Ray& ray, Interval _rayT, HitInfo& outHit) const
{
	bool hitAny = false;
	float closestHit = _rayT.m_max;

	for (const Hittable* object : m_objects)
	{
		HitInfo hit;
		if (object->Intersect(ray, Interval(_rayT.m_min, closestHit), hit))
		{
			hitAny = true;
			closestHit = hit.m_t;
			outHit = hit;
		}
	}

	return hitAny;
}

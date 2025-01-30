#pragma once
#include "Hittable.h"

class Scene : public Hittable
{
public:
	Scene() {}
	~Scene();
	void Add(Hittable& _object) { m_objects.push_back(&_object); }
	void Clear() { m_objects.clear(); }

	xs::batch_bool<float> Intersect(const RayGroup& ray, IntervalGroup rayT, HitInfoGroup& outHit) const override;

private:
	std::vector<Hittable*> m_objects;
};


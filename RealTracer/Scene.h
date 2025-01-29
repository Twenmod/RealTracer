#pragma once
#include "Hittable.h"

class Scene : public Hittable
{
public:
	Scene() {}
	~Scene();
	void Add(Hittable& _object) { m_objects.push_back(&_object); }
	void Clear() { m_objects.clear(); }

	bool Intersect(const Ray& ray, Interval rayT, HitInfo& outHit) const override;

private:
	std::vector<Hittable*> m_objects;
};


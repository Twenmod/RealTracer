#pragma once
#include "Hittable.h"

class Scene : public Hittable
{
public:
	Scene() {}
	~Scene();
	void Add(Hittable& _object) { 
		m_objects.push_back(&_object); 
		m_boundingBox = AABB(m_boundingBox, _object.GetBoundingBox());
	}
	void Clear() { m_objects.clear(); }
	std::vector<Hittable*> GetObjects() { return m_objects; }
	xs::batch_bool<float> Intersect(const RayGroup& ray, IntervalGroup rayT, HitInfoGroup& outHit) const override;
	const AABB& GetBoundingBox() const override
	{
		return m_boundingBox;
	};

private:
	std::vector<Hittable*> m_objects;
	AABB m_boundingBox;
};


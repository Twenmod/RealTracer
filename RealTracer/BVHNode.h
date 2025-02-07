#pragma once
#include "Hittable.h"
#include "Scene.h"

class BVHNode : public Hittable
{
public:
	BVHNode(Scene& scene) : BVHNode(scene.GetObjects(), 0, scene.GetObjects().size()) { scene.Clear(); };

	BVHNode(std::vector<Hittable*> objects, size_t start, size_t end);

	xs::batch_bool<float> Intersect(const RayGroup& ray, IntervalGroup rayT, HitInfoGroup& outHit) const override;

	const AABB& GetBoundingBox() const override { return m_boundingBox; }

private:
	Hittable* left;
	Hittable* right;
	AABB m_boundingBox;

	static bool box_x_compare(const Hittable* a, const Hittable* b)
	{
		IntervalGroup a_axis_interval = a->GetBoundingBox().m_x;
		IntervalGroup b_axis_interval = b->GetBoundingBox().m_x;
		return (a_axis_interval.min < b_axis_interval.min).get(0); // in this case we can just get the first one since for this we dont use simd
	}

	static bool box_y_compare(const Hittable* a, const Hittable* b)
	{
		IntervalGroup a_axis_interval = a->GetBoundingBox().m_y;
		IntervalGroup b_axis_interval = b->GetBoundingBox().m_y;
		return (a_axis_interval.min < b_axis_interval.min).get(0); // in this case we can just get the first one since for this we dont use simd	
	}

	static bool box_z_compare(const Hittable* a, const Hittable* b)
	{
		IntervalGroup a_axis_interval = a->GetBoundingBox().m_z;
		IntervalGroup b_axis_interval = b->GetBoundingBox().m_z;
		return (a_axis_interval.min < b_axis_interval.min).get(0); // in this case we can just get the first one since for this we dont use simd	
	}
};


#include "precomp.h"
#include "Common.h"
#include "BVHNode.h"

BVHNode::BVHNode(std::vector<Hittable*> objects, size_t start, size_t end)
{
	//Make bounds
	m_boundingBox = AABB::empty;
	for (size_t object = start; object < end; object++)
		m_boundingBox = AABB(m_boundingBox, objects[object]->GetBoundingBox());


	uint axis = m_boundingBox.LongestAxis();
	auto comparator = (axis == 0) ? box_x_compare
		: (axis == 1) ? box_y_compare
		: box_z_compare;

	size_t objectsSize = end - start;

	if (objectsSize == 1)
	{
		left = objects[start];
		right = nullptr;
	}

	else if (objectsSize == 2)
	{
		left = objects[start];
		right = objects[start + 1];
	}
	else
	{
		std::sort(objects.begin() + start, objects.begin() + end, comparator);

		size_t mid = start + objectsSize / 2;
		left = new BVHNode(objects, start, mid);
		right = new BVHNode(objects, mid, end);
	}
	if (right == nullptr) m_boundingBox = left->GetBoundingBox();
	else m_boundingBox = AABB(left->GetBoundingBox(), right->GetBoundingBox());
}

xs::batch_bool<float> BVHNode::Intersect(const RayGroup& _ray, IntervalGroup _rayT, HitInfoGroup& _outHit) const
{
	xs::batch_bool<float> intersectsBB = m_boundingBox.Intersects(_ray, _rayT);
	if (xs::all(!intersectsBB)) return intersectsBB;

	xs::batch_bool<float> hitLeft = left->Intersect(_ray, _rayT, _outHit);
	xs::batch_bool<float> hitRight;
	if (right == nullptr) hitRight = xs::batch_bool<float>(false);
	else hitRight = right->Intersect(_ray, IntervalGroup(_rayT.min, xs::select(hitLeft, _outHit.t, _rayT.max)), _outHit); // only check till necesarry length if already hit left

	return (hitLeft | hitRight);
}

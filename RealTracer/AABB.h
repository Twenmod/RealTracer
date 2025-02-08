#pragma once

class RayGroup;

class AABB
{
public:
	AABB() {}
	AABB(const IntervalGroup& x, const IntervalGroup& y, const IntervalGroup& z) : m_x(x), m_y(y), m_z(z) {}
	AABB(const Point3& corner1, const Point3& corner2) :
		m_x(IntervalGroup(xs::batch<float>(std::min(corner1.x(), corner2.x())), xs::batch<float>(std::max(corner1.x(), corner2.x())))),
		m_y(IntervalGroup(xs::batch<float>(std::min(corner1.y(), corner2.y())), xs::batch<float>(std::max(corner1.y(), corner2.y())))),
		m_z(IntervalGroup(xs::batch<float>(std::min(corner1.z(), corner2.z())), xs::batch<float>(std::max(corner1.z(), corner2.z()))))
	{
		PadToMinimumSize();
	}

	AABB(const AABB& a, const AABB& b) :
		m_x(IntervalGroup(a.m_x, b.m_x)),
		m_y(IntervalGroup(a.m_y, b.m_y)),
		m_z(IntervalGroup(a.m_z, b.m_z))
	{
		PadToMinimumSize();
	}

	uint LongestAxis()
	{
		if (xs::all(m_x.Size() > m_y.Size()))
		{
			return xs::all(m_x.Size() > m_z.Size()) ? 0 : 2;
		}
		else
		{
			return xs::all(m_y.Size() > m_z.Size()) ? 1 : 2;
		}
	}
 
	xs::batch_bool<float> Intersects(const RayGroup& ray, IntervalGroup rayT) const;
	IntervalGroup m_x, m_y, m_z;

	static const AABB empty, universe;
private:
	xs::batch_bool<float> IntersectsAxis(const xs::batch<float>& rayAxisOrigin, const xs::batch<float>& rayAxisDirection, IntervalGroup& rayT, const IntervalGroup& axisInterval) const;
	//Adds padding to avoid having a AABB that has a size of 0
	void PadToMinimumSize()
	{
		const float minSize = 0.01f;
		m_x.min = xs::select(m_x.Size() < minSize, m_x.Expanded(minSize).min, m_x.min);
		m_x.max = xs::select(m_x.Size() < minSize, m_x.Expanded(minSize).max, m_x.max);
		m_y.min = xs::select(m_y.Size() < minSize, m_y.Expanded(minSize).min, m_y.min);
		m_y.max = xs::select(m_y.Size() < minSize, m_y.Expanded(minSize).max, m_y.max);
		m_z.min = xs::select(m_z.Size() < minSize, m_z.Expanded(minSize).min, m_z.min);
		m_z.max = xs::select(m_z.Size() < minSize, m_z.Expanded(minSize).max, m_z.max);
	}
};



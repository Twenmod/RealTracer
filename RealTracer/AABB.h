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
	}

	AABB(const AABB& a, const AABB& b) :
		m_x(IntervalGroup(a.m_x, b.m_x)),
		m_y(IntervalGroup(a.m_y, b.m_y)),
		m_z(IntervalGroup(a.m_z, b.m_z))
	{
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
	xs::batch_bool<float> IntersectsAxis(const xs::batch<float>& rayAxisOrigin, const xs::batch<float>& rayAxisDirection, IntervalGroup rayT, const IntervalGroup& axisInterval) const;
};



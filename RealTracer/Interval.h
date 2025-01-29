#pragma once
class Interval
{
public:
	Interval() {}
	Interval(float min, float max);

	float Size() const { return m_max - m_min; }
	float Contains(float x) const { return m_min <= x && x <= m_max; }
	float Surrounds(float x) const { return m_min < x && x < m_max; }

	float Clamp(float x) const { return std::max(std::min(x, m_max), m_min); }

	float m_min{ INFINITY };
	float m_max{ -INFINITY };



	static const Interval empty, universe;

private:
};



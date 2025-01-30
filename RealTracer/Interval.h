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


class IntervalGroup
{
public:
	IntervalGroup(xs::batch<float> _min, xs::batch<float> _max)
	{
		min = _min;
		max = _max;
	}

	xs::batch<float> Size() const { return max - min; }
	xs::batch_bool<float> Contains(xs::batch<float> x) const { return min <= x && x <= max; }
	xs::batch_bool<float> Surrounds(xs::batch<float> x) const { return min < x && x < max; }

	xs::batch<float> min;
	xs::batch<float> max;
};
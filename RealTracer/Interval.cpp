#include "precomp.h"
#include "Common.h"
#include "Interval.h"

Interval::Interval(float _min, float _max)
{
	m_min = _min;
	m_max = _max;
}


const Interval Interval::empty = Interval(INFINITY, -INFINITY);
const Interval Interval::universe = Interval(-INFINITY, INFINITY);

const IntervalGroup IntervalGroup::empty = IntervalGroup(INFINITY, -INFINITY);
const IntervalGroup IntervalGroup::universe = IntervalGroup(-INFINITY, INFINITY);
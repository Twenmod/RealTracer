#include "precomp.h"
#include "Common.h"
#include "Ray.h"


Ray::Ray()
{
}

Ray::Ray(const Point3& _origin, const Vec3& _direction) :
	m_origin(_origin),
	m_direction(_direction)
{
}

Point3 Ray::At(float t) const
{
	return m_origin + m_direction * t;
}

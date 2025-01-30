#include "precomp.h"
#include "Common.h"
#include "Ray.h"


Ray::Ray()
{
}

Ray::Ray(const xs::batch<float>& _origin, const xs::batch<float>& _direction) :
	origin(_origin),
	direction(_direction)
{
}

xs::batch<float> Ray::At(float t) const
{
	return origin + direction * t;
}

Vec3 RayGroup::At(xs::batch<float> t) const
{
	return origin + direction * t;
}

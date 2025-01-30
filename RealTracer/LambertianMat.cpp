#include "precomp.h"
#include "Common.h"
#include "LambertianMat.h"
#include "Ray.h"
#include "Hittable.h"

LambertianMat::LambertianMat(const Color& _albedo)
{
	m_albedo = _albedo;
}

bool LambertianMat::Scatter(const RayGroup& _ray, const HitInfoGroup& _hitInfo, Color& _attentuationOut, RayGroup& _rayOut) const
{
	//Vec3 scatterDirection = Reflect(_ray.direction, _hitInfo.normal);
	Vec3 scatterDirection = _hitInfo.normal + RandomUnitVector();

	_rayOut = RayGroup(_hitInfo.point, scatterDirection);
	_attentuationOut = m_albedo;
	return true;
}

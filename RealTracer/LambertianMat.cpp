#include "precomp.h"
#include "Common.h"
#include "LambertianMat.h"
#include "Ray.h"
#include "Hittable.h"

LambertianMat::LambertianMat(const Color& _albedo)
{
	m_albedo = _albedo;
}

bool LambertianMat::Scatter(const Ray&, const HitInfo& _hitInfo, Color& _attentuationOut, Ray& _rayOut) const
{
	Vec3 scatterDirection = _hitInfo.m_normal + RandomUnitVector();

	if (scatterDirection.NearZero()) scatterDirection = _hitInfo.m_normal;


	_rayOut = Ray(_hitInfo.m_point, scatterDirection);
	_attentuationOut = m_albedo;
	return true;
}

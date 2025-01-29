#include "precomp.h"
#include "Common.h"
#include "MetalMat.h"
#include "Ray.h"
#include "Hittable.h"

MetalMat::MetalMat(const Color& _albedo, const float _glossiness)
{
	m_albedo = _albedo;
	m_glossiness = min(_glossiness,1.f);
}

bool MetalMat::Scatter(const Ray& _rayIn, const HitInfo& _hitInfo, Color& _attentuation, Ray& _rayOut) const
{
	Vec3 reflectDir = Reflect(_rayIn.GetDirection(), _hitInfo.m_normal);
	reflectDir = Normalize(reflectDir) + (m_glossiness * RandomUnitVector());
	_rayOut = Ray(_hitInfo.m_point, reflectDir);
	_attentuation = m_albedo;
	return (dot(reflectDir, _hitInfo.m_normal) > 0);
}

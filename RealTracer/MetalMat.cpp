#include "precomp.h"
#include "Common.h"
#include "MetalMat.h"
#include "Ray.h"
#include "Hittable.h"
//
//MetalMat::MetalMat(const Color& _albedo, const float _glossiness)
//{
//	m_albedo = _albedo;
//	m_glossiness = min(_glossiness,1.f);
//}
//
//bool MetalMat::Scatter(const Ray& _rayIn, const HitInfo& _hitInfo, Color& _attentuation, Ray& _rayOut) const
//{
//	Vec3 reflectDir = Reflect(_rayIn.GetDirection(), _hitInfo.m_normal);
//	reflectDir = Normalize(reflectDir) + (m_glossiness * RandomUnitVector());
//	_rayOut = Ray(_hitInfo.m_point, reflectDir);
//	_attentuation = m_albedo;
//	return (dot(reflectDir, _hitInfo.m_normal) > 0);
//}

MetalMat::MetalMat(const Color& _albedo, const float _glossiness)
{
	albedo = _albedo;
	glossiness = _glossiness;
}

xs::batch_bool<float> MetalMat::Scatter(const RayGroup& _rayIn, const HitInfoGroup& _hitInfo, Color& _attentuation, RayGroup& _rayOut) const
{
	Vec3 reflectDir = Reflect(_rayIn.direction, _hitInfo.normal);
	reflectDir = Normalize(reflectDir) + (glossiness * RandomUnitVector());
	_rayOut = RayGroup(_hitInfo.point, reflectDir);
	_attentuation = albedo;
	return (Dot(reflectDir, _hitInfo.normal) > 0);
}

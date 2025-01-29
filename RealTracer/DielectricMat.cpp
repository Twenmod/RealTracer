#include "precomp.h"
#include "Common.h"
#include "DielectricMat.h"
#include "Hittable.h"
#include "Ray.h"

DielectricMat::DielectricMat(const float _indexOfRefraction)
{
	m_indexOfRefraction = _indexOfRefraction;
}

bool DielectricMat::Scatter(const Ray& _rayIn, const HitInfo& _hitInfo, Color& _attentuation, Ray& _rayOut) const
{
	_attentuation = Color(1.f);
	float relativeIOR = _hitInfo.m_frontFace ? (1.f / m_indexOfRefraction) : m_indexOfRefraction;
	
	Vec3 rayDir = Normalize(_rayIn.GetDirection());
	float cosTheta = min(dot(-rayDir, _hitInfo.m_normal), 1.f);
	float sinTheta = sqrt(1.f - cosTheta * cosTheta);

	bool cannotRefract = relativeIOR * sinTheta > 1.f;

	Vec3 refractDir;
	
	if (cannotRefract || Reflectance(cosTheta,relativeIOR) > RandomFloat())
		refractDir = Reflect(rayDir, _hitInfo.m_normal);
	else
		refractDir = Refract(rayDir, _hitInfo.m_normal, relativeIOR);

	_rayOut = Ray(_hitInfo.m_point, refractDir);

	return true;
}

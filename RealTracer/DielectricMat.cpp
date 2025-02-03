#include "precomp.h"
#include "Common.h"
#include "DielectricMat.h"
#include "Hittable.h"
#include "Ray.h"
//
//DielectricMat::DielectricMat(const float _indexOfRefraction)
//{
//	m_indexOfRefraction = _indexOfRefraction;
//}
//
//bool DielectricMat::Scatter(const Ray& _rayIn, const HitInfo& _hitInfo, Color& _attentuation, Ray& _rayOut) const
//{
//	_attentuation = Color(1.f);
//	float relativeIOR = _hitInfo.m_frontFace ? (1.f / m_indexOfRefraction) : m_indexOfRefraction;
//	
//	Vec3 rayDir = Normalize(_rayIn.GetDirection());
//	float cosTheta = min(dot(-rayDir, _hitInfo.m_normal), 1.f);
//	float sinTheta = sqrt(1.f - cosTheta * cosTheta);
//
//	bool cannotRefract = relativeIOR * sinTheta > 1.f;
//
//	Vec3 refractDir;
//	
//	if (cannotRefract || Reflectance(cosTheta,relativeIOR) > RandomFloat())
//		refractDir = Reflect(rayDir, _hitInfo.m_normal);
//	else
//		refractDir = Refract(rayDir, _hitInfo.m_normal, relativeIOR);
//
//	_rayOut = Ray(_hitInfo.m_point, refractDir);
//
//	return true;
//}

DielectricMat::DielectricMat(const float _IOR)
{
	IOR = _IOR;
}

xs::batch_bool<float> DielectricMat::Scatter(const RayGroup& _rayIn, const HitInfoGroup& _hitInfo, ColorGroup& _attentuation, RayGroup& _rayOut) const
{
	_attentuation = ColorGroup(xs::batch<float>(1.f));
	xs::batch<float> relativeIOR = xs::select(_hitInfo.frontFace, xs::batch<float>(1.f / IOR), xs::batch<float>(IOR));

	Vec3Group rayDir = Normalize(_rayIn.direction);
	xs::batch<float> cosTheta = xs::min(Dot(-rayDir, _hitInfo.normal), xs::batch<float>(1.f));
	xs::batch<float> sinTheta = xs::max(xs::batch<float> (0.f),sqrt(1.f - cosTheta * cosTheta));

	xs::batch_bool<float> cannotRefract = relativeIOR * sinTheta > xs::batch<float>(1.f);


	xs::batch_bool<float> reflect = cannotRefract | (Reflectance(cosTheta, relativeIOR) > RandomBatch());

	Vec3Group reflection = Reflect(rayDir, _hitInfo.normal);
	Vec3Group refraction = Refract(rayDir, _hitInfo.normal, relativeIOR);

	Vec3Group refractDir;
	refractDir.x = xs::select(reflect, reflection.x, refraction.x);
	refractDir.y = xs::select(reflect, reflection.y, refraction.y);
	refractDir.z = xs::select(reflect, reflection.z, refraction.z);

	_rayOut = RayGroup(_hitInfo.point, refractDir);

	return xs::batch_bool<float>(true);
}

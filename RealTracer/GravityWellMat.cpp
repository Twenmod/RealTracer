#include "precomp.h"
#include "Common.h"
#include "GravityWellMat.h"
#include "Hittable.h"
#include "Ray.h"

GravityWellMat::GravityWellMat(const float radius, const float gravity)
{
	m_gravity = gravity;
	m_radius = radius;
}

bool GravityWellMat::Scatter(const Ray& _rayIn, const HitInfo& _hitInfo, Color& _attentuation, Ray& _rayOut) const
{
	_attentuation = Color(1.f);

	Vec3 rayDir = Normalize(_rayIn.GetDirection());

	//Pull the ray towards the center
	rayDir += -_hitInfo.m_normal * m_gravity;


	_rayOut = Ray(_hitInfo.m_point, rayDir);
	return true;
}

bool EventHorizonMat::Scatter(const Ray& _rayIn, const HitInfo& _hitInfo, Color& _attentuation, Ray& _rayOut) const
{
	//Any ray entering the event horizon gets eaten
	_attentuation = Color(0.f);
	_rayOut = Ray(_hitInfo.m_point, Reflect(_rayIn.GetDirection(), _hitInfo.m_normal));
	return true;
}

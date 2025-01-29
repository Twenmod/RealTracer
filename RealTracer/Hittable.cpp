#include "precomp.h"
#include "Common.h"
#include "Hittable.h"

void HitInfo::SetNormal(const Ray& _ray, const Vec3& _normal)
{
	m_frontFace = dot(_ray.GetDirection(), _normal) < 0;
	m_normal = m_frontFace ? _normal : -_normal;
}

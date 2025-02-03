#include "precomp.h"
#include "Common.h"
#include "LambertianMat.h"
#include "Ray.h"
#include "Hittable.h"
#include "Texture.h"

LambertianMat::LambertianMat(const Texture& _texture) :
	m_texture(_texture)
{
}

xs::batch_bool<float> LambertianMat::Scatter(const RayGroup&, const HitInfoGroup& _hitInfo, ColorGroup& _attentuationOut, RayGroup& _rayOut) const
{
	//Vec3 scatterDirection = Reflect(_ray.direction, _hitInfo.normal);
	Vec3Group scatterDirection = _hitInfo.normal + RandomUnitVector();

	_rayOut = RayGroup(_hitInfo.point, scatterDirection);
	_attentuationOut = m_texture.Sample(_hitInfo.u,_hitInfo.v,_hitInfo.point);
	return xs::batch_bool<float>(true);
}

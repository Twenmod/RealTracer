#include "precomp.h"
#include "Common.h"
#include "EmissiveMat.h"

#include "Ray.h"
#include "Hittable.h"


EmissiveMat::EmissiveMat(ColorGroup _emission)
{
	m_emission = _emission;
}

xs::batch_bool<float> EmissiveMat::Scatter(const RayGroup& _rayIn, const HitInfoGroup& _hitInfo, ColorGroup& _attentuationOut, RayGroup& _rayOut) const
{
	return xs::batch_bool<float>(false);
}

ColorGroup EmissiveMat::Emitted() const
{
	return m_emission;
}

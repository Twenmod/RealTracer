#pragma once
#include "Material.h"
class LambertianMat :
    public Material
{
public:
	LambertianMat(const Color& albedo);

	xs::batch_bool<float>Scatter(const RayGroup& rayIn, const HitInfoGroup& hitInfo, Color& attentuation, RayGroup& rayOut) const override;

private:
	Color m_albedo;
};


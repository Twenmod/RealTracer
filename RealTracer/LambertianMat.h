#pragma once
#include "Material.h"
class LambertianMat :
    public Material
{
public:
	LambertianMat(const ColorGroup& albedo);

	xs::batch_bool<float>Scatter(const RayGroup& rayIn, const HitInfoGroup& hitInfo, ColorGroup& attentuation, RayGroup& rayOut) const override;

private:
	ColorGroup m_albedo;
};


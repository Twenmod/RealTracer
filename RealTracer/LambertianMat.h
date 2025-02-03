#pragma once
#include "Material.h"

class Texture;

class LambertianMat :
    public Material
{
public:
	LambertianMat(const Texture& texture);

	xs::batch_bool<float>Scatter(const RayGroup& rayIn, const HitInfoGroup& hitInfo, ColorGroup& attentuation, RayGroup& rayOut) const override;

private:
	const Texture& m_texture;
};


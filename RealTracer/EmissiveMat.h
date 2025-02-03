#pragma once
#include "Material.h"
class EmissiveMat :
    public Material
{
public:
	EmissiveMat(ColorGroup emission);
	xs::batch_bool<float> Scatter(const RayGroup& rayIn, const HitInfoGroup& hitInfo, ColorGroup& attentuation, RayGroup& rayOut) const override;
	ColorGroup Emitted() const override;
private:
	ColorGroup m_emission;

};


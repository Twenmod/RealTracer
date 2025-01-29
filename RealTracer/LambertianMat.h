#pragma once
#include "Material.h"
class LambertianMat :
    public Material
{
public:
	LambertianMat(const Color& albedo);

	bool Scatter(const Ray& rayIn, const HitInfo& hitInfo, Color& attentuation, Ray& rayOut) const override;

private:
	Color m_albedo;
};


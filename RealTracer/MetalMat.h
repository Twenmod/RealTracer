#pragma once
#include "Material.h"
//class MetalMat :
//    public Material
//{
//public:
//	MetalMat(const Color& albedo, const float glossiness);
//
//	bool Scatter(const Ray& rayIn, const HitInfo& hitInfo, Color& attentuation, Ray& rayOut) const override;
//
//private:
//	Color m_albedo;
//	float m_glossiness;
//};
//
class MetalMat :
	public Material
{
public:
	MetalMat(const Color& albedo, const float glossiness);

	xs::batch_bool<float>Scatter(const RayGroup& rayIn, const HitInfoGroup& hitInfo, Color& attentuation, RayGroup& rayOut) const override;

private:
	Color albedo;
	float glossiness;
};


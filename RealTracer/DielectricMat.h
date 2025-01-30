#pragma once
#include "Material.h"
//class DielectricMat :
//    public Material
//{
//public:
//	DielectricMat(const float indexOfRefraction);
//
//	bool Scatter(const Ray& rayIn, const HitInfo& hitInfo, Color& attentuation, Ray& rayOut) const override;
//
//private:
//	float m_indexOfRefraction;
//
//	static double Reflectance(double cosine, double refraction_index)
//	{
//		//Schlick's approximation for reflectance.
//		auto r0 = (1 - refraction_index) / (1 + refraction_index);
//		r0 = r0 * r0;
//		return r0 + (1 - r0) * std::pow((1 - cosine), 5);
//	}
//
//};
//

#pragma once
#include "Material.h"
class GravityWellMat :
    public Material
{
public:
	GravityWellMat(const float radius, const float gravity);

	bool Scatter(const Ray& rayIn, const HitInfo& hitInfo, Color& attentuation, Ray& rayOut) const override;

private:
	float m_gravity;
	float m_radius;
};

class EventHorizonMat :
	public Material
{
public:
	EventHorizonMat() {};

	bool Scatter(const Ray& rayIn, const HitInfo& hitInfo, Color& attentuation, Ray& rayOut) const override;

private:
};


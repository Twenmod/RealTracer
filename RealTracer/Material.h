#pragma once

class Ray;
class HitInfo;

class Material
{
public:
	virtual ~Material() = default;

	//Scatters material returns true if scattered and false when absorbed
	virtual bool Scatter(const Ray& rayIn, const HitInfo& hitInfo, Color& attentuation, Ray& rayOut) const = 0;

private:
};


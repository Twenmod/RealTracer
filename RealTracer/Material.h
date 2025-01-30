#pragma once

class RayGroup;
class HitInfoGroup;

class Material
{
public:
	virtual ~Material() = default;

	//Scatters material returns true if scattered and false when absorbed
	virtual xs::batch_bool<float> Scatter(const RayGroup& rayIn, const HitInfoGroup& hitInfo, Color& attentuation, RayGroup& rayOut) const = 0;

private:
};


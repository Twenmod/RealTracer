#pragma once

class RayGroup;
class HitInfoGroup;

class Material
{
public:
	virtual ~Material() = default;

	//Scatters material returns true if scattered and false when absorbed
	virtual xs::batch_bool<float> Scatter(const RayGroup& rayIn, const HitInfoGroup& hitInfo, ColorGroup& attentuation, RayGroup& rayOut) const {
		return xs::batch_bool<float>(false); // default absorb
	};
	virtual ColorGroup Emitted() const
	{
		return ColorGroup(xs::batch<float>(0.f)); // Default no emission
	};

private:
};


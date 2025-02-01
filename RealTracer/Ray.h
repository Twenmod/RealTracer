#pragma once


class Ray
{
public:
	Ray();
	Ray(const xs::batch<float>& _origin, const xs::batch<float>& _direction);

	xs::batch<float> At(float t) const;

	xs::batch<float> origin;
	xs::batch<float> direction;
private:
	Point3Group m_origin;
	Vec3Group m_direction;
};

class RayGroup
{
public:
	RayGroup() {}
	RayGroup(Vec3Group _origin, Vec3Group _direction)
	{
		origin = _origin;
		direction = _direction;
	}

	Vec3Group origin;
	Vec3Group direction;
	Vec3Group At(xs::batch<float> t) const;
};
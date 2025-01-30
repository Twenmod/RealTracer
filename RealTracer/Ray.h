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
	Point3 m_origin;
	Vec3 m_direction;
};

class RayGroup
{
public:
	RayGroup() {}
	RayGroup(Vec3 _origin, Vec3 _direction)
	{
		origin = _origin;
		direction = _direction;
	}

	Vec3 origin;
	Vec3 direction;
	Vec3 At(xs::batch<float> t) const;
};
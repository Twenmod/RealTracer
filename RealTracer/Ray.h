#pragma once

class Ray
{
public:
	Ray();
	Ray(const Point3& origin, const Vec3& direction);

	const Point3& GetOrigin() const { return m_origin; }
	const Vec3& GetDirection() const { return m_direction; }

	Point3 At(float t) const;

private:
	Point3 m_origin;
	Vec3 m_direction;

};


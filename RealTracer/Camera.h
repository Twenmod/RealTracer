#pragma once


#include "Jobmanager.h"

class Hittable;
class Ray;
class Material;

class Camera;

class RayJob : public Job
{
public:
	void Main() override;
	Camera* camera;
	int pixelPos;
	uint maxBounces;
	Color* pixels;
	const Hittable* scene;
	int pixelAmount;
};


class Camera
{
public:
	Camera(Point3 position = Point3(0.f));

	void Render(const Hittable& scene);

	float m_defocusAngle = 3;
	float m_focusDistance = 1;
	Point3 m_position{Point3(0.f)};
	Vec3 m_direction{Vec3(0.f,0.f,-1.f)};
	Vec3 m_up{ Vec3(0.f,1.f,0.f) };
	float m_verticalFOV{ 90 };
	Color ShootRay(const Ray& ray, uint maxBounces, const Hittable& scene) const;
	Ray GetRay(int pixelX, int pixelY) const;
private:

	void Initialize();

	Point3 SampleDefocusDisk() const
	{
		// Returns a random point in the camera defocus disk.
		Vec3 p = RandomCircleUnitVector();
		return m_position + (p.x() * defocusDiskU) + (p.y() * defocusDiskV);
	}

	//Viewport
	Vec3 viewportU, viewportV;

	Vec3 pixelDeltaU, pixelDeltaV;
	Vec3 u, v, w;
	Vec3 viewportTopLeft;
	Vec3 pixel00;
	Vec3 defocusDiskU;
	Vec3 defocusDiskV;

};


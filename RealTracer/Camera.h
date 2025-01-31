#pragma once


#include "Jobmanager.h"

class Hittable;
class Ray;
class Material;
class RayGroup;

class Camera;

class RayJob : public Job
{
public:
	void Main() override;
	Camera* camera{nullptr};
	int pixelPos{0};
	uint maxBounces{0};
	std::vector<Vec3Single>* pixels{nullptr};
	std::vector<Vec3Single>* primaryNormals{nullptr};
	const Hittable* scene{nullptr};
	int pixelAmount{0};
	int samples{0};
};


class Camera
{
public:
	Camera(Vec3Single position = Vec3Single(0.f));

	std::vector<Vec3Single> Render(const Hittable& scene, int sample, std::vector<Vec3Single>* outNormal = nullptr);

	float m_defocusAngle = 3;
	float m_focusDistance = 1;
	Vec3Single m_position{Vec3Single(0.f)};
	Vec3Single m_direction{ Vec3Single(0.f,0.f,-1.f)};
	Vec3Single m_up{ Vec3Single(0.f,1.f,0.f) };
	float m_verticalFOV{ 90 };
	Color ShootRay(const RayGroup& ray, xs::batch<int> maxBounces, const Hittable& scene, Color* primaryNormalOut = nullptr) const;
	RayGroup GetRay(xs::batch<float> pixelX, xs::batch<float> pixelY) const;
	void GetPrimaryRay(float pixelX, float pixelY, Vec3Single* origin, Vec3Single* direction) const;
	std::vector<Material*> materials;
private:

	void Initialize();

	Point3 SampleDefocusDisk() const
	{
		// Returns a random point in the camera defocus disk.
		Vec3 p = RandomCircleUnitVector();
		Vec3 out;
		out.x = m_position.x() + (p.x * defocusDiskU.x() + (p.y * defocusDiskV.x()));
		out.y = m_position.y() + (p.x * defocusDiskU.y() + (p.y * defocusDiskV.y()));
		out.z = m_position.z() + (p.x * defocusDiskU.z() + (p.y * defocusDiskV.z()));
		return out;
	}

	//Viewport
	Vec3Single viewportU, viewportV;

	Vec3Single pixelDeltaU, pixelDeltaV;
	Vec3Single u, v, w;
	Vec3Single viewportTopLeft;
	Vec3Single pixel00;
	Vec3Single defocusDiskU;
	Vec3Single defocusDiskV;

};


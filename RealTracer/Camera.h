

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
	std::vector<Vec3>* pixels{nullptr};
	std::vector<Vec3>* primaryNormals{nullptr};
	std::vector<Vec3>* primaryPositions{nullptr};
	const Hittable* scene{nullptr};
	int pixelAmount{0};
	int samples{0};
};


class Camera
{
public:
	Camera(Vec3 position = Vec3(0.f));

	std::vector<Vec3> Render(const Hittable& scene, int sample, std::vector<Vec3>* outNormal = nullptr, std::vector<Vec3>* outPositions = nullptr);

	float m_defocusAngle = 3;
	float m_focusDistance = 1;
	Vec3 m_position{Vec3(0.f)};
	Vec3 m_direction{ Vec3(0.f,0.f,-1.f)};
	Vec3 m_right{ Vec3(1,0,0) };
	Vec3 m_up{ Vec3(0,1,0) };
	Vec3 m_worldUp{ Vec3(0.f,1.f,0.f) };
	float m_verticalFOV{ 90 };
	void SetDirection(Vec3 direction)
	{
		m_direction = direction;
		m_right = cross(direction, m_worldUp);
		m_up = cross(m_right, m_direction);
	}
	ColorGroup ShootRay(const RayGroup& ray, xs::batch<int> maxBounces, const Hittable& scene, ColorGroup* primaryNormalOut = nullptr, Point3Group* _posOut = nullptr) const;
	RayGroup GetRay(xs::batch<float> pixelX, xs::batch<float> pixelY) const;
	void GetPrimaryRay(float pixelX, float pixelY, Vec3* origin, Vec3* direction) const;
	std::vector<Material*> materials;
	void GetProjections(glm::mat4& view, glm::mat4& projection) const
	{
		glm::vec3 pos = glm::vec3(m_position.x(), m_position.y(), m_position.z());
		view = glm::lookAtRH(pos, pos + glm::vec3(m_direction.x(), m_direction.y(), m_direction.z()), glm::vec3(m_worldUp.x(),m_worldUp.y(),m_worldUp.z()));
		Vec3 pixelCenter = pixel00 + 0.5f * (pixelDeltaU * IMAGE_WIDTH + pixelDeltaV * IMAGE_HEIGHT);
		float nearPlane = 1.0f / (2.0f * tan(glm::radians(m_verticalFOV) / 2.0f)); // Distance to image plane		
		projection = glm::perspective(glm::radians(m_verticalFOV), ASPECT_RATIO, nearPlane, 1000.f);
	}
private:

	void Initialize();

	Point3Group SampleDefocusDisk() const
	{
		// Returns a random point in the camera defocus disk.
		Vec3Group p = RandomCircleUnitVector();
		Vec3Group out;
		out.x = m_position.x() + (p.x * defocusDiskU.x() + (p.y * defocusDiskV.x()));
		out.y = m_position.y() + (p.x * defocusDiskU.y() + (p.y * defocusDiskV.y()));
		out.z = m_position.z() + (p.x * defocusDiskU.z() + (p.y * defocusDiskV.z()));
		return out;
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
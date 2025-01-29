#include "precomp.h"
#include "Common.h"
#include "Camera.h"

#include "Ray.h"
#include "Hittable.h"
#include "Material.h"

std::mutex pixelMutex;  // Declare a global or class-scoped mutex

Camera::Camera(Point3 _position)
{
	m_position = _position;



}

void Camera::Render(const Hittable& scene)
{
	Initialize();

	JobManager::CreateJobManager(32);

	JobManager* jobManager = JobManager::GetJobManager();

	std::vector<RayJob*> jobs;
	Color* pixels;
	pixels = new Color[IMAGE_WIDTH * IMAGE_HEIGHT];

	int maxThreads = jobManager->MaxConcurrent();
	int pixelsPerThread = floor((IMAGE_WIDTH * IMAGE_HEIGHT) / maxThreads);

	int jobIndex = 0;
	for (int i = 0; i < maxThreads; i++)
	{
		RayJob* job = new RayJob();
		jobs.push_back(job);
		job->camera = this;
		job->maxBounces = MAX_BOUNCES;
		job->scene = &scene;

		job->pixelPos = jobIndex;

		//std::clog << "start with x" << job->x << " y" << job->y << " till x" << xPos << " y" << yPos << '\n';

		job->pixelAmount = min(pixelsPerThread, IMAGE_WIDTH * IMAGE_HEIGHT - jobIndex);
		job->pixels = pixels;
		jobManager->AddJob2(job);

		jobIndex += (pixelsPerThread);

	}
	//for (int i = 0; i < jobs.size(); i++)
	//{
	//	jobs[i]->Main();
	//}


	jobManager->RunJobs();

	for (int i = 0; i < jobs.size(); i++)
	{
		delete jobs[i];
	}
	std::cout << "P3\n" << IMAGE_WIDTH << ' ' << IMAGE_HEIGHT << "\n255\n";

	for (int x = 0; x < IMAGE_WIDTH * IMAGE_HEIGHT; x++)
	{
		WriteColor(std::cout, pixels[x]);
	}
	std::clog << "\rDone.                 \n";
}

Ray Camera::GetRay(int _pixelX, int _pixelY) const
{
	Vec3 offset = Vec3(RandomFloat() - 0.5f, RandomFloat() - 0.5f, 0);
	Vec3 sample = pixel00 + ((_pixelX + offset.x()) * pixelDeltaU) + ((_pixelY + offset.y()) * pixelDeltaV);
	Point3 rayOrigin = (m_defocusAngle <= 0) ? m_position : SampleDefocusDisk();
	Vec3 rayDirection = sample - rayOrigin;
	return Ray(rayOrigin, rayDirection);
}

Color Camera::ShootRay(const Ray& _ray, uint _maxBounces, const Hittable& _scene) const
{
	if (_maxBounces <= 0)
	{
		return Color(0.f);
	}
	HitInfo hit;
	if (_scene.Intersect(_ray, Interval(MIN_INTERSECTION_DEPTH, INFINITY), hit))
	{
		Ray scattered;
		Color attentuation;
		if (hit.m_material->Scatter(_ray, hit, attentuation, scattered))
		{
			return attentuation * ShootRay(scattered, _maxBounces - 1, _scene);
		}
	}

	Vec3 direction = Normalize(_ray.GetDirection());
	float a = 0.5f * direction.y() + 1.0f;
	return Lerp(Color(1.f), Color(0.5f, 0.7f, 1.0f), a);
}

void Camera::Initialize()
{
	auto theta = DegToRad(m_verticalFOV);
	auto h = std::tan(theta / 2);
	float viewportHeight = 2 * h * m_focusDistance;
	float viewportWidth = viewportHeight * ASPECT_RATIO;

	w = Normalize(-m_direction);
	u = Normalize(cross(m_up, w));
	v = cross(w, u);


	//Viewport
	viewportU = viewportWidth * u;
	viewportV = viewportHeight * -v;

	pixelDeltaU = viewportU / IMAGE_WIDTH;
	pixelDeltaV = viewportV / IMAGE_HEIGHT;

	viewportTopLeft = m_position - m_focusDistance * w - viewportU * 0.5f - viewportV * 0.5f;
	pixel00 = viewportTopLeft + 0.5 * (pixelDeltaU + pixelDeltaV);

	float defocus_radius = m_focusDistance * std::tan(DegToRad(m_defocusAngle / 2));
	defocusDiskU = u * defocus_radius;
	defocusDiskV = v * defocus_radius;
}

void RayJob::Main()
{
	for (int i = 0; i < pixelAmount; i++)
	{
		int pos = pixelPos + i;
		int yPos = floor(pos / IMAGE_WIDTH);
		int xPos = pos - yPos * IMAGE_WIDTH;

		Color pixelColor;
		for (size_t sample = 0; sample < SAMPLES_PER_PIXEL; sample++)
		{
			Ray ray = camera->GetRay(xPos, yPos);
			pixelColor += camera->ShootRay(ray, maxBounces, *scene);
		}

		Color& colorOut = pixels[pos];  // Assign color to the correct pixel
		colorOut = pixelColor * PIXEL_SAMPLES_SCALE;

		//		Color pixelColor(0.f);
//		for (size_t sample = 0; sample < SAMPLES_PER_PIXEL; sample++)
//		{
//			Ray ray = GetRay(i, j);
//			pixelColor += ShootRay(ray, MAX_BOUNCES, scene);
//		}
//		WriteColor(std::cout, pixelColor*PIXEL_SAMPLES_SCALE);


		// Update the color values for this pixel


	}
}

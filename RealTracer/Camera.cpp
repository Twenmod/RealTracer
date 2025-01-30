#include "precomp.h"
#include "Common.h"
#include "Camera.h"

#include "Ray.h"
#include "Hittable.h"
#include "Material.h"

#include "xsimd/xsimd.hpp"
namespace xs = xsimd;

Camera::Camera(Vec3Single _position)
{
	m_position = _position;
}

void Camera::Render(const Hittable& scene)
{
	Initialize();

	JobManager::CreateJobManager(32);

	JobManager* jobManager = JobManager::GetJobManager();

	std::vector<RayJob*> jobs;
	float* pixelsR;
	float* pixelsG;
	float* pixelsB;
	pixelsR = new float[IMAGE_WIDTH * IMAGE_HEIGHT];
	pixelsG = new float[IMAGE_WIDTH * IMAGE_HEIGHT];
	pixelsB = new float[IMAGE_WIDTH * IMAGE_HEIGHT];

	for (int i = 0; i < IMAGE_WIDTH * IMAGE_HEIGHT; i++)
	{
		pixelsR[i] = 0;
		pixelsG[i] = 0;
		pixelsB[i] = 0;
	}


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

		job->pixelAmount = std::min(pixelsPerThread, IMAGE_WIDTH * IMAGE_HEIGHT - jobIndex);
		job->pixelsR = pixelsR;
		job->pixelsG = pixelsG;
		job->pixelsB = pixelsB;
#ifdef MULTITHREAD
		jobManager->AddJob2(job);
#endif
		jobIndex += (pixelsPerThread);

	}
#ifdef MULTITHREAD
	jobManager->RunJobs();
#endif

	for (int i = 0; i < jobs.size(); i++)
	{
#ifndef MULTITHREAD
		jobs[i]->Main();
#endif
		delete jobs[i];
	}
	std::cout << "P3\n" << IMAGE_WIDTH << ' ' << IMAGE_HEIGHT << "\n255\n";

	for (int x = 0; x < IMAGE_WIDTH * IMAGE_HEIGHT; x++)
	{
		WriteColor(std::cout, pixelsR[x], pixelsG[x], pixelsB[x]);
	}
	std::clog << "\rDone.                 \n";
}

RayGroup Camera::GetRay(xs::batch<float> _pixelX, xs::batch<float> _pixelY) const
{
	Vec3 offset = Vec3(RandomBatch() - 0.5f, RandomBatch() - 0.5f, 0);
	Vec3 sample;
	sample.x = pixel00.x() + ((_pixelX + offset.x) * pixelDeltaU.x()) + ((_pixelY + offset.y) * pixelDeltaV.x());
	sample.y = pixel00.y() + ((_pixelX + offset.x) * pixelDeltaU.y()) + ((_pixelY + offset.y) * pixelDeltaV.y());
	sample.z = pixel00.z() + ((_pixelX + offset.x) * pixelDeltaU.z()) + ((_pixelY + offset.y) * pixelDeltaV.z());
	Vec3 pos(xs::batch<float> (m_position.x()), xs::batch<float>(m_position.y()), xs::batch<float>(m_position.z()));
	Point3 rayOrigin = (m_defocusAngle <= 0) ? pos : SampleDefocusDisk();
	Vec3 rayDirection = sample - rayOrigin;
	return RayGroup(rayOrigin, rayDirection);
}

Color Camera::ShootRay(const RayGroup& _ray, xs::batch<int> _maxBounces, const Hittable& _scene) const
{
	xs::batch_bool<int> noBounces = _maxBounces <= 0;
	if (xs::all(noBounces))
	{
		return Color(xs::batch<float>(0.f));
	}
	HitInfoGroup hit;
	xs::batch_bool<float> intersections = _scene.Intersect(_ray, IntervalGroup(MIN_INTERSECTION_DEPTH, INFINITY), hit);

	Color attentuation(xs::batch<float>(1.f));
	if (xs::any(intersections))
	{
		RayGroup scattered;


		for (int materialIndex = 0; materialIndex < materials.size(); materialIndex++)
		{
			xs::batch_bool<float> materialMask = intersections & xs::batch_bool_cast<float>(hit.material == xs::batch<int>(materialIndex));

			if (xs::any(materialMask))
			{
				const Material* mat = materials[materialIndex];

				Color scatterAttent = attentuation;

				xs::batch_bool<float> scatterMask = mat->Scatter(_ray, hit, scatterAttent, scattered);
				attentuation.x = xs::select(scatterMask, scatterAttent.x, attentuation.x);
				attentuation.y = xs::select(scatterMask, scatterAttent.y, attentuation.y);
				attentuation.z = xs::select(scatterMask, scatterAttent.z, attentuation.z);


				Color reflectAttent = ShootRay(scattered, _maxBounces - 1, _scene);
				reflectAttent = attentuation * reflectAttent;
				attentuation.x = xs::select(scatterMask, reflectAttent.x, attentuation.x);
				attentuation.y = xs::select(scatterMask, reflectAttent.y, attentuation.y);
				attentuation.z = xs::select(scatterMask, reflectAttent.z, attentuation.z);

			}
		}
	}

	Vec3 direction = Normalize(_ray.direction);
	xs::batch<float> a = xs::batch<float>(0.5f) * direction.y + 1.0f;
	Color backgroundColor = Lerp(Color(xs::batch<float> (1.f)), Color(xs::batch<float> (0.5f), xs::batch<float> (0.7f), xs::batch<float> (1.0f)), a);

	attentuation.x = xs::select(intersections, attentuation.x, backgroundColor.x);
	attentuation.y = xs::select(intersections, attentuation.y, backgroundColor.y);
	attentuation.z = xs::select(intersections, attentuation.z, backgroundColor.z);

	//No bounces
	attentuation.x = xs::select(xs::batch_bool_cast<float>(noBounces), xs::batch<float> (0.f), attentuation.x);
	attentuation.y = xs::select(xs::batch_bool_cast<float>(noBounces), xs::batch<float> (0.f), attentuation.y);
	attentuation.z = xs::select(xs::batch_bool_cast<float>(noBounces), xs::batch<float> (0.f), attentuation.z);
	return attentuation;

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

		Color pixelColor(0);
		for (size_t sample = 0; sample < SAMPLES_PER_PIXEL; sample++)
		{
			RayGroup ray = camera->GetRay(xPos, yPos);

			xs::batch<Ray> rayGroup;

			pixelColor += camera->ShootRay(ray, maxBounces, *scene);
		}

		float& outR = pixelsR[pos];
		float& outG = pixelsG[pos];
		float& outB = pixelsB[pos];
		float pixelColorR = 0;
		float pixelColorG = 0;
		float pixelColorB = 0;
		for (size_t i = 0; i < SIMD_SIZE; i++)
		{
			pixelColorR += pixelColor.x.get(i);
			pixelColorG += pixelColor.y.get(i);
			pixelColorB += pixelColor.z.get(i);
		}
		outR = pixelColorR * PIXEL_SAMPLES_SCALE;
		outG = pixelColorG * PIXEL_SAMPLES_SCALE;
		outB = pixelColorB * PIXEL_SAMPLES_SCALE;

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

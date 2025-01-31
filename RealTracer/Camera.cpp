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

std::vector<Vec3Single> Camera::Render(const Hittable& scene, int samples, std::vector<Vec3Single>* outNormal)
{
	Initialize();


	JobManager* jobManager = JobManager::GetJobManager();

	std::vector<RayJob*> jobs;

	std::vector<Vec3Single> pixels(IMAGE_WIDTH * IMAGE_HEIGHT);

	int maxThreads = jobManager->MaxConcurrent();
	int pixelsPerThread = static_cast<int>(floor((IMAGE_WIDTH * IMAGE_HEIGHT) / maxThreads));

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
		job->samples = samples;
		job->pixels = &pixels;
		job->primaryNormals = outNormal;

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

	return pixels;

}

RayGroup Camera::GetRay(xs::batch<float> _pixelX, xs::batch<float> _pixelY) const
{
	Vec3 offset = Vec3(RandomBatch() - 0.5f, RandomBatch() - 0.5f, 0);
	Vec3 sample;
	sample.x = pixel00.x() + ((_pixelX + offset.x) * pixelDeltaU.x()) + ((_pixelY + offset.y) * pixelDeltaV.x());
	sample.y = pixel00.y() + ((_pixelX + offset.x) * pixelDeltaU.y()) + ((_pixelY + offset.y) * pixelDeltaV.y());
	sample.z = pixel00.z() + ((_pixelX + offset.x) * pixelDeltaU.z()) + ((_pixelY + offset.y) * pixelDeltaV.z());
	Vec3 pos(xs::batch<float>(m_position.x()), xs::batch<float>(m_position.y()), xs::batch<float>(m_position.z()));
	Point3 rayOrigin = (m_defocusAngle <= 0) ? pos : SampleDefocusDisk();
	Vec3 rayDirection = sample - rayOrigin;
	return RayGroup(rayOrigin, rayDirection);
}

void Camera::GetPrimaryRay(float _pixelX, float _pixelY, Vec3Single* _origin, Vec3Single* _direction) const
{
	Vec3Single sample;
	sample.setX(pixel00.x() + ((_pixelX) * pixelDeltaU.x()) + ((_pixelY) * pixelDeltaV.x()));
	sample.setY(pixel00.y() + ((_pixelX) * pixelDeltaU.y()) + ((_pixelY) * pixelDeltaV.y()));
	sample.setZ(pixel00.z() + ((_pixelX) * pixelDeltaU.z()) + ((_pixelY) * pixelDeltaV.z()));
	_origin->setX(m_position.x());
	_origin->setY(m_position.y());
	_origin->setZ(m_position.z());
	_direction->setX(sample.x() - m_position.x());
	_direction->setY(sample.y() - m_position.y());
	_direction->setZ(sample.z() - m_position.z());
}

Color Camera::ShootRay(const RayGroup& _ray, xs::batch<int> _maxBounces, const Hittable& _scene, Color* _primaryNormalOut) const
{
	xs::batch_bool<int> noBounces = _maxBounces <= 0;
	if (xs::all(noBounces))
	{
		return Color(xs::batch<float>(1.f));
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
	Color backgroundColor = Lerp(Color(xs::batch<float>(1.f)), Color(xs::batch<float>(0.6f), xs::batch<float>(0.8f), xs::batch<float>(1.0f)), a);

	attentuation.x = xs::select(intersections, attentuation.x, backgroundColor.x);
	attentuation.y = xs::select(intersections, attentuation.y, backgroundColor.y);
	attentuation.z = xs::select(intersections, attentuation.z, backgroundColor.z);

	//No bounces
	attentuation.x = xs::select(xs::batch_bool_cast<float>(noBounces), xs::batch<float>(0.f), attentuation.x);
	attentuation.y = xs::select(xs::batch_bool_cast<float>(noBounces), xs::batch<float>(0.f), attentuation.y);
	attentuation.z = xs::select(xs::batch_bool_cast<float>(noBounces), xs::batch<float>(0.f), attentuation.z);

	//If first bounce return primary normal
	xs::batch_bool<int> firstHit = _maxBounces == xs::batch<int>(MAX_BOUNCES);
	if (xs::all(firstHit) && _primaryNormalOut != nullptr)
	{
		_primaryNormalOut->x = xs::select(intersections, hit.normal.x, xs::batch<float>(0.f));
		_primaryNormalOut->y = xs::select(intersections, hit.normal.y, xs::batch<float>(0.f));
		_primaryNormalOut->z = xs::select(intersections, hit.normal.z, xs::batch<float>(0.f));
	}

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
		int yPos = static_cast<int>(floor(pos / IMAGE_WIDTH));
		int xPos = pos - yPos * IMAGE_WIDTH;

		Color pixelColor(0);
		Color primaryNormal;
		for (size_t sample = 0; sample < samples; sample++)
		{
			RayGroup ray = camera->GetRay(xs::batch<float> (static_cast<float>(xPos)), xs::batch<float> (static_cast<float>(yPos)));

			//Primary ray is const to make normals more consistent
			Color rayColors;
			if (sample == 0)
			{
#ifdef SSE2
				xs::batch_bool<float> firstRay = { true, false, false, false };
#endif
#ifdef AVX2
				xs::batch_bool<float> firstRay = { true, false, false, false , false, false, false, false };
#endif
#ifdef AVX512
				xs::batch_bool<float> firstRay = { true, false, false, false , false, false, false, false, false, false, false, false , false, false, false, false };
#endif

				Vec3Single primaryOrigin;
				Vec3Single primaryDirection;
				camera->GetPrimaryRay(static_cast<float>(xPos), static_cast<float>(yPos), &primaryOrigin, &primaryDirection);

				ray.origin.x = xs::select(firstRay, xs::batch<float> (primaryOrigin.x()), ray.origin.x);
				ray.origin.y = xs::select(firstRay, xs::batch<float> (primaryOrigin.y()), ray.origin.y);
				ray.origin.z = xs::select(firstRay, xs::batch<float> (primaryOrigin.z()), ray.origin.z);				
				ray.direction.x = xs::select(firstRay, xs::batch<float> (primaryDirection.x()), ray.direction.x);
				ray.direction.y = xs::select(firstRay, xs::batch<float> (primaryDirection.y()), ray.direction.y);
				ray.direction.z = xs::select(firstRay, xs::batch<float> (primaryDirection.z()), ray.direction.z);
				rayColors = camera->ShootRay(ray, maxBounces, *scene, &primaryNormal);

			}
			else
			{
				rayColors = camera->ShootRay(ray, maxBounces, *scene);
			}
			
			pixelColor += rayColors;
		}

		Vec3Single& outColor = (*pixels)[pos];
		float pixelColorR = 0;
		float pixelColorG = 0;
		float pixelColorB = 0;

		for (size_t j = 0; j < SIMD_SIZE; j++)
		{
			pixelColorR += pixelColor.x.get(j);
			pixelColorG += pixelColor.y.get(j);
			pixelColorB += pixelColor.z.get(j);
		}
		float sampleScale = (1.f / (static_cast<float>(samples))) / SIMD_SIZE;

		outColor.setX(pixelColorR * sampleScale);
		outColor.setY(pixelColorG * sampleScale);
		outColor.setZ(pixelColorB * sampleScale);		
		Vec3Single& outNormal = (*primaryNormals)[pos];
		outNormal.setX((primaryNormal.x.get(0) + 1) * 0.5f);
		outNormal.setY((primaryNormal.y.get(0) + 1) * 0.5f);
		outNormal.setZ((primaryNormal.z.get(0) + 1) * 0.5f);
	}
}

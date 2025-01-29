
#include "precomp.h"
#include "Common.h"
#include "Ray.h"
#include "Hittable.h"
#include "Scene.h"
#include "Sphere.h"
#include "Camera.h"

#include "LambertianMat.h"
#include "MetalMat.h"
#include "DielectricMat.h"

int main()
{

	Material* ground = new LambertianMat(Color(0.0f, 0.8f, 0.f));
	Material* middle = new LambertianMat(Color(0.3f, 0.3f, 0.f));
	Material* glass = new DielectricMat(1.5f);
	Material* glassAirBubble = new DielectricMat(1.f/1.5f);
	Material* metalRight = new MetalMat(Color(0.8f, 0.2f, 0.8f), 1.f);

	//Scene
	Scene scene;
	scene.Add(*new Sphere(*middle,Vec3(0.f, 0.f, -1.f), 0.5f));
	scene.Add(*new Sphere(*glass,Vec3(-1.f, 0.f, -1.f), 0.5f));
	scene.Add(*new Sphere(*metalRight,Vec3(1.f, 0.f, -1.f), 0.5f));
	scene.Add(*new Sphere(*ground,Vec3(0.f, -100.5f, -1.f), 100.f));

	for (size_t i = 0; i < 100; i++)
	{
		Point3 pos = Point3(Rand(-5.f, 5.f), -0.42f, Rand(-5.f, 5.f));
		Material* mat = new LambertianMat(Color(RandomFloat(), RandomFloat(), RandomFloat()));

		scene.Add(*new Sphere(*mat, pos, 0.15f));


	}

	//Camera
	Camera mainCam;
	mainCam.m_verticalFOV = 20;
	mainCam.m_position = Point3(13, 2, 3);
	mainCam.m_direction = Normalize(Point3(0, 0, 0) - mainCam.m_position);
	
	mainCam.m_defocusAngle = 0.6f;
	mainCam.m_focusDistance = 10.0f;

	// Render

	
	auto time = std::chrono::system_clock::now();


	mainCam.Render(scene);

	std::chrono::duration<float> delta = std::chrono::system_clock::now()-time;
	std::clog << "Rendering took: " << (delta.count()) << "s\n";

	while (std::cin.get() != '\n');


	delete ground;
	delete middle;
	delete glass;
	delete metalRight;

}
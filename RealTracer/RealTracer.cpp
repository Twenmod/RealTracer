
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
	Material* mat = new LambertianMat(Color(0.5f, 0.5f, 0.5f));
	//Material* glass = new DielectricMat(1.5f);
	//Material* glassAirBubble = new DielectricMat(1.f/1.5f);
	//Material* metalRight = new MetalMat(Color(0.8f, 0.2f, 0.8f), 1.f);

	//Scene
	Scene scene;
	scene.Add(*new Sphere(diffuse,0.f, 0.f, -2.f, 0.5f));
	scene.Add(*new Sphere(diffuse,2.f, 0.f, -1.f, 0.5f));
	//scene.Add(*new Sphere(*glass,Vec3(-1.f, 0.f, -1.f), 0.5f));
	//scene.Add(*new Sphere(*metalRight,Vec3(1.f, 0.f, -1.f), 0.5f));
	scene.Add(*new Sphere(diffuse,0.f, -100.5f, -1.f, 100.f));

	//Camera
	Camera mainCam;
	mainCam.materials.push_back(mat);
	mainCam.m_verticalFOV = 20;
	mainCam.m_position = Vec3Single(13, 2, 3);
	mainCam.m_direction = Normalize(Vec3Single(0, 0, 0) - mainCam.m_position);
	
	mainCam.m_defocusAngle = 0.6f;
	mainCam.m_focusDistance = 10.0f;

	// Render

	
	auto time = std::chrono::system_clock::now();


	mainCam.Render(scene);

	std::chrono::duration<float> delta = std::chrono::system_clock::now()-time;
	std::clog << "Rendering took: " << (delta.count()) << "s\n";

	while (std::cin.get() != '\n');

}

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
	std::clog << "\x1B[35m" << R"(
Starting:
__________              ._____________                                 
\______   \ ____ _____  |  \__    ___/___________    ____  ___________ 
 |       _// __ \\__  \ |  | |    |  \_  __ \__  \ _/ ___\/ __ \_  __ \
 |    |   \  ___/ / __ \|  |_|    |   |  | \// __ \\  \__\  ___/|  | \/
 |____|_  /\___  >____  /____/____|   |__|  (____  /\___  >___  >__|   
        \/     \/     \/                         \/     \/    \/       
)";
	uint cores = 0;
	uint logical = 0;
	JobManager::GetProcessorCount(cores, logical);
	JobManager::CreateJobManager(logical*2);
std::clog << "\x1B[36mSystem: \n"
<< "\x1B[36m  Cores: \x1B[96m" << cores << '\n'
<< "\x1B[36m   Logical: \x1B[96m" << logical << '\n'
<< "\x1B[36m  Threads: \x1B[96m" << JobManager::GetJobManager()->MaxConcurrent() << '\n'
<< "\x1B[36m  SIMD size\x1B[96m: " << SIMD_SIZE << '\n'
<< "\x1B[36mSettings: \n"
<< "\x1B[36m  Image Size\x1B[96m: " << IMAGE_WIDTH << "x" << IMAGE_HEIGHT << '\n'
<< "\x1B[36m  Samples: \x1B[96m" << SAMPLES_PER_PIXEL*SIMD_SIZE << '\n';

Logger::CreateLogger();

//Set up a window
if (!glfwInit())
{
	Logger::LogWarning("GLFW Failed to Init", WARNING_SEVERITY::FATAL);
}













	Material* mat = new LambertianMat(Color(0.5f, 0.5f, 0.5f));
	Material* mat2 = new LambertianMat(Color(0.8f, 0.5f, 0.5f));
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
	
	mainCam.m_defocusAngle = 0.0f;
	mainCam.m_focusDistance = 10.0f;

	// Render

	
	auto time = std::chrono::system_clock::now();

	std::clog << "\x1b[0mStarting Render ";
#ifdef MULTITHREAD
	std::clog << "on " << JobManager::GetJobManager()->MaxConcurrent() << " threads";
#endif
	std::clog << '\n';

	mainCam.Render(scene);

	std::chrono::duration<float> delta = std::chrono::system_clock::now()-time;
	std::clog << "Rendering took: " << (delta.count()) << "s\n";

	while (std::cin.get() != '\n');


	Logger::DeleteLogger();
	JobManager::DeleteJobManager();

}
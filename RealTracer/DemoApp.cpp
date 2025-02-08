#include "precomp.h"
#include "Common.h"
#include "DemoApp.h"

#include "Scene.h"
#include "Sphere.h"
#include "Quad.h"
#include "LambertianMat.h"
#include "MetalMat.h"
#include "DielectricMat.h"
#include "EmissiveMat.h"

#include "Camera.h"

#include "Texture.h"
#include "BVHNode.h"

void DemoApp::Init()
{
	Scene scene;
	//Add objects to scene
	scene.Add(*new Quad(white, Point3(-2, -1, 2), Vec3(4, 0, 0), Vec3(0, 0, -4))); //floor
	////scene.Add(*new Quad(white, Point3(-2, 1, 2), Vec3(4, 0, 0), Vec3(0, 0, -4))); //ceiling
	////scene.Add(*new Quad(light, Point3(-0.5, 0.99, 0.5), Vec3(1, 0, 0), Vec3(0, 0, -1))); //ceiling light
	////scene.Add(*new Quad(white, Point3(-2, -1, -2), Vec3(4, 0, 0), Vec3(0, 5, 0))); //wall
	scene.Add(*new Quad(red, Point3(-2, -1, 2), Vec3(0, 5, 0), Vec3(0, 0, -4)));
	scene.Add(*new Quad(blue, Point3(2, -1, 2), Vec3(0, 5, 0), Vec3(0, 0, -4)));
	
	for (size_t i = 0; i < 40; i++)
	{
		scene.Add(*new Sphere(metal, Vec3(Rand(-2.f,2.f), Rand(-1.f, 1.f), Rand(-2.f, 2.f)), 0.2f));
	}

	
	renderScene = new BVHNode(scene);

	//Texture* checkerTexture = new CheckerTexture3D(1.f, ColorGroup(0.f), ColorGroup(1.f, 0, 1.f));
	Texture* whiteTexture = new SolidColorTexture(ColorGroup(1.f));
	Texture* redTexture = new SolidColorTexture(ColorGroup(1.f,0.f,0.f));
	Texture* blueTexture = new SolidColorTexture(ColorGroup(0.f,0.f,1.f));
	//Texture* imageTexture = new ImageTexture("./assets/texture.jpg");


	//Add material
	mainCam->materials.push_back(new LambertianMat(*whiteTexture));
	mainCam->materials.push_back(new LambertianMat(*redTexture));
	mainCam->materials.push_back(new LambertianMat(*blueTexture));
	mainCam->materials.push_back(new MetalMat(ColorGroup(xs::batch<float>(0.8f)), 0.8f));
	mainCam->materials.push_back(new EmissiveMat(ColorGroup(xs::batch<float>(1.f))));


	mainCam->m_position = Vec3(0, 0.5, 3);

	mainCam->m_verticalFOV = 40;
	mainCam->m_defocusAngle = 0.0f;
	mainCam->m_focusDistance = 10.0f;
}

DemoApp::DemoApp(GLFWwindow& window, EngineSettings& settings) : App(window, settings), mainCam(new Camera())
{
	m_deltaTime = 0;
}

void DemoApp::Tick(float _deltaTime)
{
	m_traceDeltaTime = _deltaTime;

	if (frameRates.size() > frameRateSize) frameRates.erase(frameRates.begin());
	frameRates.push_back(1.f / _deltaTime);

	if (m_settings.animate) timer += _deltaTime;

	//dynamic_cast<Sphere*>(scene.GetObjects()[0])->posY = sin(timer) * 0.5f + 0.5f;
	//dynamic_cast<Sphere*>(scene.GetObjects()[2])->posZ = sin(timer * 0.6f) * 2.f;

	if (glfwGetKey(&m_window, GLFW_KEY_W)) mainCam->m_position += mainCam->m_direction *_deltaTime;
	if (glfwGetKey(&m_window, GLFW_KEY_S)) mainCam->m_position += -mainCam->m_direction * _deltaTime;
	if (glfwGetKey(&m_window, GLFW_KEY_D)) mainCam->m_position += mainCam->m_right * _deltaTime;
	if (glfwGetKey(&m_window, GLFW_KEY_A)) mainCam->m_position += -mainCam->m_right * _deltaTime;
	if (glfwGetKey(&m_window, GLFW_KEY_SPACE)) mainCam->m_position += mainCam->m_up * _deltaTime;
	if (glfwGetKey(&m_window, GLFW_KEY_C)) mainCam->m_position += -mainCam->m_up * _deltaTime;
	//mainCam->m_position = Vec3(0, .5f, 1);
	mainCam->SetDirection(Normalize(Vec3(0, 0, 0) - mainCam->m_position));

}

void DemoApp::FastTick(float _deltaTime)
{
	m_deltaTime = _deltaTime;

}

void DemoApp::Trace(std::vector<Vec3>& _colorOut, std::vector<Vec3>& _normalOut, std::vector<Vec3>& _posOut)
{
	_colorOut = mainCam->Render(*renderScene, m_settings.samples, &_normalOut, &_posOut);
}

void DemoApp::Render()
{
	static bool open = true;
	ImGui::SetNextWindowSize(ImVec2(220, 400), ImGuiCond_Appearing);
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Appearing);
	ImGui::Begin("Debug", &open);
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	if (ImGui::TreeNode("Statistic"))
	{
		ImGui::PlotHistogram("##1", frameRates.data(), static_cast<int>(frameRates.size()));
		float tot = 0;
		for (float& frame : frameRates)
		{
			tot += frame;
		}
		tot /= frameRates.size();
		if (m_traceDeltaTime > 0.2f)
		{
			ImGui::Text("SPF: %.2fs (%.2fms)", (m_traceDeltaTime), m_traceDeltaTime * 1000.f);
		}
		else
		{
			ImGui::Text("FPS: %.f (%.2fms)", tot, m_traceDeltaTime * 1000.f);
		}
		ImGui::TreePop();
	}
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	if (ImGui::TreeNode("Settings"))
	{
		ImGui::Checkbox("Animate", &m_settings.animate);
		ImGui::SliderInt("Samples", &m_settings.samples, 1, 128, (std::to_string(m_settings.samples * SIMD_SIZE).c_str()));
		ImGui::Checkbox("Accumulate&Reproject", &m_settings.accumulatorOn);
		if (m_settings.accumulatorOn)
		{
			float baseTreshold = m_settings.overrideTreshold;
			if (ImGui::SliderFloat("  Treshold", &baseTreshold, 0.0001f, 1.0f, "%.4f"))
			{
				m_settings.overrideTreshold = baseTreshold;
			}
			if (ImGui::SliderFloat("  Smoothing", &m_settings.smoothingFactor, 0, 0.9999f, "%.4f"))
			{
			}			
			if (ImGui::SliderFloat("  Buffer Duration", &m_settings.updateBufferTimer, 0, 1.f, "%.2f"))
			{
			}
		}
		ImGui::Checkbox("Denoise", &m_settings.denoise);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Debug"))
	{
		if (ImGui::Checkbox("Show Normals", &m_settings.showNormals))
		{
			m_settings.showPositions = false;
			m_settings.accumulatorOn = false;
		}
		if (ImGui::Checkbox("Show Positions", &m_settings.showPositions))
		{
			m_settings.showNormals = false;
			m_settings.accumulatorOn = false;
		}
		if (m_settings.accumulatorOn)
		{
			ImGui::Text("Accumulator");
			ImGui::Checkbox("Draw Updates", &m_settings.showChange);
		}
		ImGui::TreePop();
	}
	ImGui::End();
}

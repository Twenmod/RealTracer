#include "precomp.h"
#include "Common.h"
#include "DemoApp.h"

#include "Scene.h"
#include "Sphere.h"
#include "LambertianMat.h"
#include "MetalMat.h"
#include "DielectricMat.h"
#include "EmissiveMat.h"

#include "Camera.h"

#include "Texture.h"

void DemoApp::Init()
{

	//Add objects to scene
	//scene.Add(*new Sphere(red, 1.f, 0.f, -1.f, 0.5f));
	scene.Add(*new Sphere(diffuse, 0.f, 0.f, -0.2f, 0.5f));
	scene.Add(*new Sphere(metal, 1.f, 0.0f, -0.f, 0.5f));
	scene.Add(*new Sphere(glass, -1.f, 0.f, -0.f, 0.5f));
	//scene.Add(*new Sphere(*glass,Vec3(-1.f, 0.f, -1.f), 0.5f));
	//scene.Add(*new Sphere(*metalRight,Vec3(1.f, 0.f, -1.f), 0.5f));
	scene.Add(*new Sphere(grass, 0.f, -100.5f, 0.f, 100.f));
	//scene.Add(*new Sphere(light, 0.f, 3.f, -4.f, 1.f));

	Texture* checkerTexture = new CheckerTexture3D(1.f, ColorGroup(0.f), ColorGroup(1.f, 0, 1.f));
	Texture* imageTexture = new ImageTexture("./assets/texture.jpg");


	//Add material
	mainCam->materials.push_back(new LambertianMat(*imageTexture));
	mainCam->materials.push_back(new LambertianMat(*checkerTexture));
	mainCam->materials.push_back(new DielectricMat(1.5f));
	mainCam->materials.push_back(new MetalMat(ColorGroup(xs::batch<float>(0.8f)), 0.8f));
	mainCam->materials.push_back(new EmissiveMat(ColorGroup(xs::batch<float>(50.f))));


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

	if (glfwGetKey(&m_window, GLFW_KEY_W)) mainCam->m_position += Vec3(1, 0, 0)*_deltaTime;
	if (glfwGetKey(&m_window, GLFW_KEY_S)) mainCam->m_position += Vec3(-1, 0, 0) * _deltaTime;
	if (glfwGetKey(&m_window, GLFW_KEY_D)) mainCam->m_position += Vec3(0, 0, 1) * _deltaTime;
	if (glfwGetKey(&m_window, GLFW_KEY_A)) mainCam->m_position += Vec3(0, 0, -1) * _deltaTime;
	if (glfwGetKey(&m_window, GLFW_KEY_C)) mainCam->m_position += Vec3(0, -1, 0) * _deltaTime;
	if (glfwGetKey(&m_window, GLFW_KEY_SPACE)) mainCam->m_position += Vec3(0, 1, 0) * _deltaTime;
	//mainCam->m_position = Vec3(0, .5f, 1);
	//mainCam->m_direction = Normalize(Vec3(0, 0, 0) - mainCam->m_position);

}

void DemoApp::FastTick(float _deltaTime)
{
	m_deltaTime = _deltaTime;

}

void DemoApp::Trace(std::vector<Vec3>& _colorOut, std::vector<Vec3>& _normalOut, std::vector<Vec3>& _posOut)
{
	_colorOut = mainCam->Render(scene, m_settings.samples, &_normalOut, &_posOut);
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
		ImGui::Checkbox("Accumulator", &m_settings.accumulatorOn);
		if (m_settings.accumulatorOn)
		{
			float baseTreshold = m_settings.overrideTreshold;
			if (ImGui::SliderFloat("  Treshold", &baseTreshold, 0.0001f, 1.0f, "%.4f"))
			{
				m_settings.overrideTreshold = baseTreshold;
			}
			float invSmooth = 1.f - m_settings.smoothingFactor;
			if (ImGui::SliderFloat("  Smoothing", &invSmooth, 0, 0.9999f, "%.4f"))
			{
				m_settings.smoothingFactor = 1.f - invSmooth;
			}
			ImGui::SliderFloat("  Update margin", &m_settings.updateTimer, 0, 0.4f, "%.3f");
		}
		ImGui::Checkbox("Denoise", &m_settings.denoise);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Debug"))
	{
		if (ImGui::Checkbox("Show Normals", &m_settings.showNormals))
		{
			m_settings.showPositions = false;
		}
		if (ImGui::Checkbox("Show Positions", &m_settings.showPositions))
		{
			m_settings.showNormals = false;
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

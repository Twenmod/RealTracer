#include "precomp.h"
#include "Common.h"
#include "DemoApp.h"

#include "Scene.h"
#include "Sphere.h"
#include "LambertianMat.h"
#include "MetalMat.h"
#include "DielectricMat.h"

void DemoApp::Init()
{

	//Add objects to scene
	scene.Add(*new Sphere(diffuse, 0.f, 0.f, 1.f, 0.5f));
	scene.Add(*new Sphere(red, 1.f, 0.f, -1.f, 0.5f));
	scene.Add(*new Sphere(metal, 3.f, 0.3f, 0.f, 0.8f));
	scene.Add(*new Sphere(mirror, 0.f, 0.f, 0.f, 0.5f));
	//scene.Add(*new Sphere(*glass,Vec3(-1.f, 0.f, -1.f), 0.5f));
	//scene.Add(*new Sphere(*metalRight,Vec3(1.f, 0.f, -1.f), 0.5f));
	scene.Add(*new Sphere(grass, 0.f, -100.5f, -1.f, 100.f));

	//Add material
	mainCam.materials.push_back(new LambertianMat(Color(xs::batch<float>(0.5f), xs::batch<float>(0.5f), xs::batch<float>(0.5f))));
	mainCam.materials.push_back(new LambertianMat(Color(xs::batch<float>(0.9f), xs::batch<float>(0.3f), xs::batch<float>(0.3f))));
	mainCam.materials.push_back(new LambertianMat(Color(xs::batch<float>(0.2f), xs::batch<float>(0.8f), xs::batch<float>(0.2f))));
	mainCam.materials.push_back(new DielectricMat(1.1f));
	mainCam.materials.push_back(new MetalMat(Color(xs::batch<float>(0.8f)), 0.3f));
	mainCam.materials.push_back(new MetalMat(Color(xs::batch<float>(0.9f)), 0.0f));


	mainCam.m_verticalFOV = 20;
	mainCam.m_defocusAngle = 0.0f;
	mainCam.m_focusDistance = 10.0f;
}

void DemoApp::Tick(float _deltaTime)
{
	if (frameRates.size() > frameRateSize) frameRates.erase(frameRates.begin());
	frameRates.push_back(1.f / _deltaTime);
	m_deltaTime = _deltaTime;

	if (m_settings.animate) timer += _deltaTime;

	dynamic_cast<Sphere*>(scene.GetObjects()[0])->posY = sin(timer) * 0.5 + 0.5;
	dynamic_cast<Sphere*>(scene.GetObjects()[2])->posZ = sin(timer * 0.6) * 2;

	mainCam.m_position = Vec3Single(sin(timer * 0.5) * 10, 2, cos(timer * 0.5) * 10);
	mainCam.m_direction = Normalize(Vec3Single(0, 0, 0) - mainCam.m_position);

}

void DemoApp::Render(std::vector<Vec3Single>& _colorOut, std::vector<Vec3Single>& _normalOut)
{

	static bool open = true;
	ImGui::SetNextWindowSize(ImVec2(220, 400), ImGuiCond_Appearing);
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Appearing);
	ImGui::Begin("Debug", &open);
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	if (ImGui::TreeNode("Statistic"))
	{
		ImGui::PlotHistogram("##1", frameRates.data(), frameRates.size());
		float tot = 0;
		for (float& frame : frameRates)
		{
			tot += frame;
		}
		tot /= frameRates.size();
		ImGui::Text("FPS: %.f (%.2fms)", tot, m_deltaTime * 1000.f);

		ImGui::TreePop();
	}
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	if (ImGui::TreeNode("Settings"))
	{
		ImGui::Checkbox("Animate", &m_settings.animate);
		ImGui::SliderInt("Samples", &m_settings.samples, 1, 64, (std::to_string(m_settings.samples * SIMD_SIZE).c_str()));
		ImGui::Checkbox("Accumulator", &m_settings.accumulatorOn);
		if (m_settings.accumulatorOn)
		{
			float baseTreshold = m_settings.overrideTreshold / 0xff;
			if (ImGui::SliderFloat("  Treshold", &baseTreshold, 0.0001, 1.0f, "%.4f"))
			{
				m_settings.overrideTreshold = baseTreshold * 0xff;
			}
			float invSmooth = 1.f - m_settings.smoothingFactor;
			if (ImGui::SliderFloat("  Smoothing", &invSmooth, 0, 0.9999f, "%.4f"))
			{
				m_settings.smoothingFactor = 1.f - invSmooth;
			}
			if (ImGui::SliderFloat("  Update margin", &m_settings.updateTimer, 0, 0.4f, "%.3f"));
		}
		ImGui::Checkbox("Denoise", &m_settings.denoise);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Debug"))
	{
		ImGui::Checkbox("Show Normals", &m_settings.showNormals);
		if (m_settings.accumulatorOn)
		{
			ImGui::Text("Accumulator");
			ImGui::Checkbox("Draw Updates", &m_settings.showChange);
		}
		ImGui::TreePop();
	}
	ImGui::End();

	_colorOut = mainCam.Render(scene, m_settings.samples, &_normalOut);
}

void DemoApp::LateRender()
{
}

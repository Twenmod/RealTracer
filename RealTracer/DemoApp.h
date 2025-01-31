#pragma once
#include "App.h"
#include "Camera.h"
#include "Scene.h"

class DemoApp :
    public App
{
public:
	DemoApp(GLFWwindow& window, EngineSettings& settings) : App(window, settings) {}
	void Init() override;
	void Tick(float deltaTime) override;
	void Render(std::vector<Vec3Single>& colorOut, std::vector<Vec3Single>& normalOut) override;
	void LateRender() override;
private:
	Camera mainCam;
	Scene scene;

	int frameRateSize = 30;
	std::vector<float> frameRates;
	float m_deltaTime;
	float timer = 0;
};


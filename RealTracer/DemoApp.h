#pragma once
#include "App.h"
#include "Camera.h"
#include "Scene.h"

class DemoApp :
	public App
{
public:
	DemoApp(GLFWwindow& window, EngineSettings& settings) : App(window, settings) { m_deltaTime = 0; }
	void Init() override;
	void Tick(float deltaTime) override;
	void Trace(std::vector<Vec3Single>& colorOut, std::vector<Vec3Single>& normalOut, float deltaTime) override;
	void Render() override;
private:
	Camera mainCam;
	Scene scene;

	int frameRateSize{ 30 };
	std::vector<float> frameRates{};
	float m_deltaTime{ 0 };
	float m_traceDeltaTime{ 0 };
	float timer{ 0 };
};


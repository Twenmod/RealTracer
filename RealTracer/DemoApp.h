#pragma once
#include "App.h"
#include "Scene.h"

class Camera;

class DemoApp :
	public App
{
public:
	DemoApp(GLFWwindow& window, EngineSettings& settings);
	void Init() override;
	void Tick(float deltaTime) override;
	void FastTick(float deltaTime) override;
	void Trace(std::vector<Vec3>& colorOut, std::vector<Vec3>& normalOut, std::vector<Vec3>& posOut) override;
	void Render() override;
	const Camera& GetCamera() const override
	{
		return *mainCam;
	}
private:
	Camera* mainCam;
	Hittable* renderScene;

	int frameRateSize{ 30 };
	std::vector<float> frameRates{};
	float m_deltaTime{ 0 };
	float m_traceDeltaTime{ 0 };
	float timer{ 0 };
};


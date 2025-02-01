#pragma once

struct GLFWwindow;

struct EngineSettings
{
	bool accumulatorOn = true;
	bool animate = true;
	bool denoise = true;
	bool showNormals = false;
	bool showChange = false;
	int samples = SAMPLES_PER_PIXEL;
	float overrideTreshold = 0xff * 0.02f;
	float smoothingFactor = 0.12f;
	float updateTimer = 0.07f;
};

class App
{
public:
	App(GLFWwindow& window, EngineSettings& settings);
	virtual ~App() {};
	virtual void Init() = 0;
	virtual void Tick(float deltaTime) = 0;
	virtual void Trace(std::vector<Vec3Single>& colorOut, std::vector<Vec3Single>& normalOut, float _deltaTime) = 0; // Handles drawing of the screen should fill in pixelcolors and normals
	virtual void Render() {}; // Called after rendering the raytracer but before swapping the buffers
	virtual void PostRender() {}; // Called after rendering the raytracer but before swapping the buffers
protected:
	GLFWwindow& m_window;
	EngineSettings& m_settings;
};
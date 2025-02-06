#pragma once

class Camera;

struct GLFWwindow;

struct EngineSettings
{
	bool accumulatorOn = true;
	bool animate = true;
	bool denoise = true;
	bool showNormals = false;
	bool showPositions = false;
	bool showChange = false;
	int samples = SAMPLES_PER_PIXEL;
	float overrideTreshold = 0.4f;
	float smoothingFactor = 0.10f;
};

class App
{
public:
	App(GLFWwindow& window, EngineSettings& settings);
	virtual ~App() {};
	virtual void Init() = 0;
	virtual void Tick(float deltaTime) = 0; // Happens every trace tick
	virtual void FastTick(float deltaTime) = 0; // Happens every window update (watch out this can cause race conditions when updating rendering information)
	virtual void Trace(std::vector<Vec3>& colorOut, std::vector<Vec3>& normalOut, std::vector<Vec3>& posOut) = 0; // Handles tracing of the screen should fill in pixelcolors, normals and worldpositions
	virtual void Render() {}; // Called after rendering the raytracer but before swapping the buffers happens every window update
	virtual void PostRender() {}; // Called after rendering the raytracer but before swapping the buffers
	virtual const Camera& GetCamera() const = 0;
protected:
	GLFWwindow& m_window;
	EngineSettings& m_settings;
};
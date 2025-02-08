#pragma once

class App;
struct EngineSettings;

class Renderer
{
public:
	Renderer();
	~Renderer();
	void Init(App& m_renderApplication, EngineSettings& settings);
	bool GetFrameReady() const { return m_frameReady; };
	//Copy data buffers returns false if failed because no frame was present
	bool CopyBuffer(std::vector<unsigned char>* frameColorData, std::vector<unsigned char>* frameNormalData, std::vector<Vec3>* framePosData);
private:
	void RenderThreadMain();

	glm::vec2 WorldToScreen(Vec3 worldPos, glm::mat4 view, glm::mat4 projection);
	void AccumulateFrame(float deltaTime, const std::vector<Vec3>& frame, const std::vector<Vec3>& frameNormal, const std::vector<Vec3>& framePos);
	EngineSettings* m_settings;
	App* m_renderApplication;
	glm::mat4 m_oldView, m_oldProjection, m_currentView, m_currentProjection;
	std::atomic<bool> m_isRunning{ true };
	std::atomic<bool> m_frameReady{ false };
	float m_traceTime{ 0 };
	std::vector<Vec3>m_currentFrameNormal{ std::vector<Vec3>() };
	std::vector<Point3> m_currentFramePosition{ std::vector<Point3>() };
	std::vector<Vec3> m_currentFrameColor{ std::vector<Vec3>() };
	std::mutex m_renderMutex{};
	std::thread* m_renderThread{ nullptr };
	//Final image data
	std::vector<unsigned char> m_frameColorData{ std::vector<unsigned char>() };
	std::vector<unsigned char> m_frameNormalData{ std::vector<unsigned char>() };
	std::vector<Vec3> m_framePosData{ std::vector<Vec3>() };
	std::vector<float> m_frameUpdateTimerData{ std::vector<float>() };
};


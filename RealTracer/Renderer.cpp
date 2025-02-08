#include "precomp.h"
#include "Common.h"
#include "Renderer.h"
#include "App.h"
#include "Camera.h"

Renderer::Renderer()
{
	m_currentFrameColor.resize(IMAGE_WIDTH * IMAGE_HEIGHT);
	m_currentFrameNormal.resize(IMAGE_WIDTH * IMAGE_HEIGHT);
	m_currentFramePosition.resize(IMAGE_WIDTH * IMAGE_HEIGHT);

	m_frameColorData.resize(IMAGE_WIDTH * IMAGE_HEIGHT * 3);
	m_frameNormalData.resize(IMAGE_WIDTH * IMAGE_HEIGHT * 3);
	m_framePosData.resize(IMAGE_WIDTH * IMAGE_HEIGHT);
}

Renderer::~Renderer()
{
	m_isRunning = false;
	m_renderThread->join(); // Make sure renderthread exited
	delete m_renderThread;
}

void Renderer::Init(App& _renderApplication, EngineSettings& _settings)
{
	m_renderApplication = &_renderApplication;
	m_settings = &_settings;

	m_renderThread = new std::thread(&Renderer::RenderThreadMain, this); // Launch the rendering thread
}

bool Renderer::CopyBuffer(std::vector<unsigned char>* _frameColorData, std::vector<unsigned char>* _frameNormalData, std::vector<Vec3>* _framePosData)
{
	if (m_frameReady)
	{
		{
			std::lock_guard<std::mutex> lock(m_renderMutex); // Lock the renderThread

			if (_frameColorData)
			{
				if (_frameColorData->size() == m_frameColorData.size())
				{
					for (int i = 0; i < _frameColorData->size(); i++)
					{
						(*_frameColorData)[i] = m_frameColorData[i];
					}
				}
				else
				{
					Logger::LogWarning("Size of frameColor buffer was different than the one in the renderer", WARNING_SEVERITY::HIGH);

				}
			}
			if (_frameNormalData)
			{
				if (_frameNormalData->size() == m_frameNormalData.size())
				{
					for (int i = 0; i < _frameNormalData->size(); i++)
					{
						(*_frameNormalData)[i] = m_frameNormalData[i];
					}
				}
				else
				{
					Logger::LogWarning("Size of frameNormal buffer was different than the one in the renderer", WARNING_SEVERITY::HIGH);

				}
			}
			if (_framePosData)
			{
				if (_framePosData->size() == m_framePosData.size())
				{
					for (int i = 0; i < _framePosData->size(); i++)
					{
						(*_framePosData)[i] = m_framePosData[i];
					}
				}
				else
				{
					Logger::LogWarning("Size of framePos buffer was different than the one in the renderer", WARNING_SEVERITY::HIGH);

				}
			}


			m_renderApplication->Tick(m_traceTime);

		}//Unlock
		m_frameReady = false;
		return true;
	}
	else
	{
		Logger::LogWarning("Tried to copy data while frame was being processed", WARNING_SEVERITY::LOW);
		return false;
	}
}

void Renderer::RenderThreadMain()
{
	double lastTime = glfwGetTime();

	m_renderApplication->GetCamera().GetProjections(m_oldView, m_oldProjection);
	while (m_isRunning)
	{
		{
			std::lock_guard<std::mutex> lock(m_renderMutex);

			double time = glfwGetTime();
			m_traceTime = time - lastTime;
			lastTime = time;
			// Call theApp render method
			m_renderApplication->Trace(m_currentFrameColor, m_currentFrameNormal, m_currentFramePosition);
			AccumulateFrame(m_traceTime, m_currentFrameColor, m_currentFrameNormal, m_currentFramePosition);
			m_oldView = m_currentView;
			m_oldProjection = m_currentProjection;
			m_renderApplication->GetCamera().GetProjections(m_currentView, m_currentProjection);
			m_frameReady = true;
		} // unlock
		std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Avoid overloading
	}
}

glm::vec2 Renderer::WorldToScreen(Vec3 _worldPos, glm::mat4 _view, glm::mat4 _projection)
{
	// Transform the world-space point into clip space.
	glm::vec4 clipSpacePos = _projection * _view * glm::vec4(_worldPos.x(), _worldPos.y(), _worldPos.z(), 1.0f);
	//If the point is behind the camera, return an invalid coordinate.
	if (clipSpacePos.w <= 0.0001f)
	{
		return glm::vec2(-1000.0f, -1000.0f);
	}

	glm::vec3 ndcPos = (glm::vec3(clipSpacePos) / clipSpacePos.w);

	// Convert NDC to screen coordinates.
	glm::vec2 screenPos;
	screenPos.x = (ndcPos.x * 0.5f + 0.5f) * IMAGE_WIDTH;
	screenPos.y = (1.0f - (ndcPos.y * 0.5f + 0.5f)) * IMAGE_HEIGHT;
	return screenPos;
}

void Renderer::AccumulateFrame(float deltaTime, const std::vector<Vec3>& _frame, const std::vector<Vec3>& _frameNormal, const std::vector<Vec3>& _framePos)
{

	std::vector<unsigned char> frameNormalDataBuffer(IMAGE_WIDTH * IMAGE_HEIGHT * 3);
	std::vector<unsigned char> frameColorDataBuffer(IMAGE_WIDTH * IMAGE_HEIGHT * 3);
	std::vector<Vec3> framePosDataBuffer(IMAGE_WIDTH * IMAGE_HEIGHT);

	for (int i = 0; i < IMAGE_WIDTH * IMAGE_HEIGHT; i++)
	{

		Vec3 currentPos = _framePos[i];
		Vec3 currentNormal = _frameNormal[i];

		//Reproject
		glm::vec2 reprojected = WorldToScreen(currentPos, m_oldView, m_oldProjection);

		bool isReprojected = false;

		// no reprojection if out of bounds
		if (m_settings->accumulatorOn && !m_settings->showNormals && !m_settings->showPositions)
		{
			if (reprojected.x >= 0 && reprojected.x < IMAGE_WIDTH &&
				reprojected.y >= 0 && reprojected.y < IMAGE_HEIGHT)
			{
				int currY = floor(i / IMAGE_WIDTH);
				int currX = i - (currY * IMAGE_WIDTH);

				//Get prev pixel here
				int prevX = static_cast<int>(floor(reprojected.x));
				int prevY = static_cast<int>(floor(reprojected.y));
				prevX = std::max(std::min(prevX, IMAGE_WIDTH - 1), 0);
				prevY = std::max(std::min(prevY, IMAGE_HEIGHT - 1), 0);


				int prevI = prevX + prevY * IMAGE_WIDTH;
				Vec3 prevPos = m_framePosData[prevI];
				Vec3 prevNormal;
				prevNormal.setX(m_frameNormalData[prevI * 3 + 0]);
				prevNormal.setY(m_frameNormalData[prevI * 3 + 1]);
				prevNormal.setZ(m_frameNormalData[prevI * 3 + 2]);

				// Check if the previous pixel's position and normal match the current pixel
				float positionDiff = (currentPos - prevPos).Length();
				float normalDiff = abs(dot(currentNormal, prevNormal));

				if (positionDiff < m_settings->overrideTreshold && normalDiff > m_settings->overrideTreshold)
				{
					float newR;
					float newG;
					float newB;

					newR = _frame[i].x() * 0xff;
					newG = _frame[i].y() * 0xff;
					newB = _frame[i].z() * 0xff;

					//Use previous frame
					float a = 1.f - exp2(-m_traceTime / m_settings->smoothingFactor);
					frameColorDataBuffer[i * 3 + 0] = static_cast<unsigned char>(m_frameColorData[prevI * 3 + 0] * (1.f - a) + newR * a);  // R			
					frameColorDataBuffer[i * 3 + 1] = static_cast<unsigned char>(m_frameColorData[prevI * 3 + 1] * (1.f - a) + newG * a);  // G
					frameColorDataBuffer[i * 3 + 2] = static_cast<unsigned char>(m_frameColorData[prevI * 3 + 2] * (1.f - a) + newB * a);  // B
					//frameTextureData[i * 3 + 0] = static_cast<unsigned char>(0);  // r
					//frameTextureData[i * 3 + 1] = static_cast<unsigned char>(0xff);  // G
					//frameTextureData[i * 3 + 2] = static_cast<unsigned char>(0);  // b
					isReprojected = true;
				}
			}
		}
		if (!isReprojected)
		{
			if (m_settings->showNormals)
			{
				frameColorDataBuffer[i * 3 + 0] = m_frameNormalData[i * 3 + 0];
				frameColorDataBuffer[i * 3 + 1] = m_frameNormalData[i * 3 + 1];
				frameColorDataBuffer[i * 3 + 2] = m_frameNormalData[i * 3 + 2];
			}
			else if (m_settings->showPositions)
			{
				frameColorDataBuffer[i * 3 + 0] = _framePos[i].x() * 0xff;
				frameColorDataBuffer[i * 3 + 1] = _framePos[i].y() * 0xff;
				frameColorDataBuffer[i * 3 + 2] = _framePos[i].z() * 0xff;
			}
			else if (m_settings->showChange)
			{
				frameColorDataBuffer[i * 3 + 0] = 0xff;  // r
				frameColorDataBuffer[i * 3 + 1] = 0x00;  // G
				frameColorDataBuffer[i * 3 + 2] = 0x00;  // b
			}
			else
			{
				frameColorDataBuffer[i * 3 + 0] = _frame[i].x() * 0xff;  // r
				frameColorDataBuffer[i * 3 + 1] = _frame[i].y() * 0xff;  // G
				frameColorDataBuffer[i * 3 + 2] = _frame[i].z() * 0xff;  // b
			}
		}

		frameNormalDataBuffer[i * 3 + 0] = currentNormal.x() * 0xff;
		frameNormalDataBuffer[i * 3 + 1] = currentNormal.y() * 0xff;
		frameNormalDataBuffer[i * 3 + 2] = currentNormal.z() * 0xff;

		framePosDataBuffer[i] = currentPos;
	}

	for (int i = 0; i < IMAGE_WIDTH * IMAGE_HEIGHT; i++)
	{
		m_frameColorData[i * 3 + 0] = frameColorDataBuffer[i * 3 + 0];
		m_frameColorData[i * 3 + 1] = frameColorDataBuffer[i * 3 + 1];
		m_frameColorData[i * 3 + 2] = frameColorDataBuffer[i * 3 + 2];

		m_frameNormalData[i * 3 + 0] = frameNormalDataBuffer[i * 3 + 0];
		m_frameNormalData[i * 3 + 1] = frameNormalDataBuffer[i * 3 + 1];
		m_frameNormalData[i * 3 + 2] = frameNormalDataBuffer[i * 3 + 2];

		m_framePosData[i] = framePosDataBuffer[i];
	}
}

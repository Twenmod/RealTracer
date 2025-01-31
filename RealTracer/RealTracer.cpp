
#include "precomp.h"
#include "Common.h"
#include "Ray.h"
#include "Hittable.h"
#include "Scene.h"
#include "Sphere.h"
#include "Camera.h"

#include "LambertianMat.h"
#include "MetalMat.h"
#include "DielectricMat.h"

void error_callback(int error, const char* description)
{
	Logger::LogWarning(description, WARNING_SEVERITY::HIGH);
}
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}


int main()
{


	std::clog << "\x1B[35m" << R"(
Starting:
__________              ._____________                                 
\______   \ ____ _____  |  \__    ___/___________    ____  ___________ 
 |       _// __ \\__  \ |  | |    |  \_  __ \__  \ _/ ___\/ __ \_  __ \
 |    |   \  ___/ / __ \|  |_|    |   |  | \// __ \\  \__\  ___/|  | \/
 |____|_  /\___  >____  /____/____|   |__|  (____  /\___  >___  >__|   
        \/     \/     \/                         \/     \/    \/       
)";
	uint cores = 0;
	uint logical = 0;
	JobManager::GetProcessorCount(cores, logical);
	JobManager::CreateJobManager(logical * 2);
	std::clog << "\x1B[36mSystem: \n"
		<< "\x1B[36m  Cores: \x1B[96m" << cores << '\n'
		<< "\x1B[36m   Logical: \x1B[96m" << logical << '\n'
		<< "\x1B[36m  Threads: \x1B[96m" << JobManager::GetJobManager()->MaxConcurrent() << '\n'
		<< "\x1B[36m  SIMD size\x1B[96m: " << SIMD_SIZE << '\n'
		<< "\x1B[36mSettings: \n"
		<< "\x1B[36m  Image Size\x1B[96m: " << IMAGE_WIDTH << "x" << IMAGE_HEIGHT << '\n'
		<< "\x1B[36m  Samples: \x1B[96m" << SAMPLES_PER_PIXEL * SIMD_SIZE << '\n';

	Logger::CreateLogger();

	//Set up a window
	if (!glfwInit())
	{
		Logger::LogWarning("GLFW Failed to Init", WARNING_SEVERITY::FATAL);
		return -1;
	}
	glfwSetErrorCallback(error_callback);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "RealTracer", NULL, NULL);
	if (!window)
	{
		Logger::LogWarning("GLFW Window or OpenGL context creation failed", WARNING_SEVERITY::FATAL);
		return -1;
	}
	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);
	if (!gladLoadGL())
	{
		Logger::LogWarning("OpenGL context creation failed", WARNING_SEVERITY::FATAL);
		return -1;
	}
	glfwSwapInterval(0);

	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	const char* glsl_version = "#version 330";
	ImGui::CreateContext();
	ImGui::StyleColorsClassic();
	//ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	ImGuiIO io = ImGui::GetIO();
	io.DisplaySize = ImVec2(WINDOW_WIDTH, WINDOW_HEIGHT);
	io.Fonts->AddFontDefault();  // Ensure the default font is loaded
	io.Fonts->Build();  // Build the font atlas


	unsigned char* tex_pixels;
	int tex_width, tex_height;
	io.Fonts->GetTexDataAsRGBA32(&tex_pixels, &tex_width, &tex_height);

	// Upload texture to OpenGL (for the font atlas)
	GLuint fontTexture;
	glGenTextures(1, &fontTexture);
	glBindTexture(GL_TEXTURE_2D, fontTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_pixels);

	io.Fonts->SetTexID((ImTextureID)(intptr_t)fontTexture);


	const char* vertexShader = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 v_texCoord;

void main()
{
	gl_Position = vec4(aPos,1);
	v_texCoord = aTexCoords;
}
)";
	const char* fragmentShader = R"(
#version 330 core

out vec4 FragColor;

in vec2 v_texCoord;

uniform sampler2D image;
uniform bool denoise;

uniform vec2 texResolution;
vec2 offset = 1.0 / texResolution;

void main() {
if (denoise) {
	vec2 offsets[9] = vec2[](
		vec2(-offset.x,  offset.y), // top-left
		vec2( 0.0,       offset.y), // top-center
		vec2( offset.x,  offset.y), // top-right
		vec2(-offset.x,  0.0),   // center-left
		vec2( 0.0,       0.0),   // center-center
		vec2( offset.x,  0.0),   // center-right
		vec2(-offset.x, -offset.y), // bottom-left
		vec2( 0.0,      -offset.y), // bottom-center
		vec2( offset.x, -offset.y)  // bottom-right    
	);

    float kernel[9] = float[](
        1, 2, 1,
        2,  4, 2,
        1, 2, 1
    );

        vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(image, v_texCoord + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * (kernel[i]);
    
    FragColor = vec4(col*(1.0/16.0), 1.0);
}else {
	FragColor = texture(image, v_texCoord);
}
}
)";

	GLint success;
	GLchar infoLog[1024];

	unsigned int vertex, fragment;
	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertexShader, NULL);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 1024, NULL, infoLog);
		Logger::LogWarning("SHADER COMPILATION ERROR ", WARNING_SEVERITY::HIGH);
		Logger::LogWarning(infoLog, WARNING_SEVERITY::HIGH);
	}
	// fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragmentShader, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 1024, NULL, infoLog);
		Logger::LogWarning("SHADER COMPILATION ERROR ", WARNING_SEVERITY::HIGH);
		Logger::LogWarning(infoLog, WARNING_SEVERITY::HIGH);
	}	// shader Program
	unsigned int screenShader = glCreateProgram();
	glAttachShader(screenShader, vertex);
	glAttachShader(screenShader, fragment);
	glLinkProgram(screenShader);
	glGetShaderiv(screenShader, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(screenShader, 1024, NULL, infoLog);
		Logger::LogWarning("SHADER COMPILATION ERROR ", WARNING_SEVERITY::HIGH);
		Logger::LogWarning(infoLog, WARNING_SEVERITY::HIGH);
	}	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	float screenPlaneVert[] = {
		// positions          // texture coords
		 1.f, -1.f, 0.0f,      1.0f, 1.0f,   // bottom right
		 1.f,  1.f, 0.0f,      1.0f, 0.0f,   // top right
		-1.f, -1.f, 0.0f,      0.0f, 1.0f,   // bottom 
		-1.f,  1.f, 0.0f,      0.0f, 0.0f,   // top left 
		-1.f, -1.f, 0.0f,      0.0f, 1.0f,   // bottom left
		 1.f,  1.f, 0.0f,      1.0f, 0.0f    // top right
	};

	unsigned int screenIndices[] = {
	0, 1, 2,   // First triangle
	1, 3, 2    // Second triangle
	};


	unsigned int screenVAO, screenVBO;
	glGenVertexArrays(1, &screenVAO);
	glGenBuffers(1, &screenVBO);

	glBindVertexArray(screenVAO);

	glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(screenPlaneVert), &screenPlaneVert[0], GL_STATIC_DRAW);

	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	// texCoords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	uint frameTexture;
	glGenTextures(1, &frameTexture);
	glBindTexture(GL_TEXTURE_2D, frameTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, IMAGE_WIDTH, IMAGE_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glGenerateMipmap(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, frameTexture);
	uint frameNormalTexture;
	glGenTextures(1, &frameNormalTexture);
	glBindTexture(GL_TEXTURE_2D, frameNormalTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, IMAGE_WIDTH, IMAGE_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glGenerateMipmap(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, frameNormalTexture);

	//Material* glass = new DielectricMat(1.5f);
	//Material* glassAirBubble = new DielectricMat(1.f/1.5f);
	//Material* metalRight = new MetalMat(Color(0.8f, 0.2f, 0.8f), 1.f);

	//Scene
	Scene scene;
	scene.Add(*new Sphere(diffuse, 0.f, 0.f, 1.f, 0.5f));
	scene.Add(*new Sphere(red, 2.f, 0.f, -1.f, 0.5f));
	scene.Add(*new Sphere(metal, 4.f, 0.3f, 0.f, 0.8f));
	scene.Add(*new Sphere(glass, 1.f, 0.f, 0.f, 0.5f));
	//scene.Add(*new Sphere(*glass,Vec3(-1.f, 0.f, -1.f), 0.5f));
	//scene.Add(*new Sphere(*metalRight,Vec3(1.f, 0.f, -1.f), 0.5f));
	scene.Add(*new Sphere(grass, 0.f, -100.5f, -1.f, 100.f));

	//Camera
	Camera mainCam;
	mainCam.materials.push_back(new LambertianMat(Color(xs::batch<float>(0.5f), xs::batch<float>(0.5f), xs::batch<float>(0.5f))));
	mainCam.materials.push_back(new LambertianMat(Color(xs::batch<float>(0.9f), xs::batch<float>(0.3f), xs::batch<float>(0.3f))));
	mainCam.materials.push_back(new LambertianMat(Color(xs::batch<float>(0.2f), xs::batch<float>(0.8f), xs::batch<float>(0.2f))));
	mainCam.materials.push_back(new DielectricMat(1.1f));
	mainCam.materials.push_back(new MetalMat(Color(xs::batch<float>(0.8f)), 0.3f));
	mainCam.m_verticalFOV = 20;


	mainCam.m_defocusAngle = 0.0f;
	mainCam.m_focusDistance = 10.0f;

	std::vector<float> frameRates(30, 0.f);
	double lastTime = glfwGetTime();
	std::vector<unsigned char> frameTextureData(IMAGE_WIDTH * IMAGE_HEIGHT * 3);
	std::vector<unsigned char> frameNormalData(IMAGE_WIDTH * IMAGE_HEIGHT * 3);
	std::vector<float> frameUpdatesData(IMAGE_WIDTH * IMAGE_HEIGHT);

	bool accumulatorOn = true;
	bool animate = true;
	bool denoise = false;
	bool showNormals = false;
	bool showChange = false;

	int samples = SAMPLES_PER_PIXEL;

	float overrideTreshold = 0xff * 0.1f;
	float smoothingFactor = 0.05f;
	float updateTimer = 0.1f;
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		double time = glfwGetTime();
		float deltaTime = (time - lastTime);
		frameRates.erase(frameRates.begin());
		frameRates.push_back(1.f / deltaTime);
		lastTime = time;

		ImGui_ImplGlfw_NewFrame();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();


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
			ImGui::Text("FPS: %.f (%.2fms)", tot, deltaTime * 1000.f);

			ImGui::TreePop();
		}
		ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
		if (ImGui::TreeNode("Settings"))
		{
			ImGui::Checkbox("Animate", &animate);
			ImGui::SliderInt("Samples", &samples, 1, 64, (std::to_string(samples * SIMD_SIZE).c_str()));
			ImGui::Checkbox("Accumulator", &accumulatorOn);
			if (accumulatorOn)
			{
				float baseTreshold = overrideTreshold / 0xff;
				if (ImGui::SliderFloat("  Treshold", &baseTreshold, 0.0001, 1.0f, "%.4f"))
				{
					overrideTreshold = baseTreshold * 0xff;
				}
				float invSmooth = 1.f - smoothingFactor;
				if (ImGui::SliderFloat("  Smoothing", &invSmooth, 0, 0.9999f, "%.4f"))
				{
					smoothingFactor = 1.f - invSmooth;
				}
			}
			ImGui::Checkbox("Denoise", &denoise);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Debug"))
		{
			ImGui::Checkbox("Show Normals", &showNormals);
			if (accumulatorOn)
			{
				ImGui::Text("Accumulator");
				ImGui::Checkbox("Draw Updates", &showChange);
			}
			ImGui::TreePop();
		}
		ImGui::End();

		if (animate)
		{
			dynamic_cast<Sphere*>(scene.GetObjects()[0])->posY = sin(time) * 0.5 + 0.5;
			dynamic_cast<Sphere*>(scene.GetObjects()[2])->posZ = sin(time * 0.6) * 2;

			mainCam.m_position = Vec3Single(sin(time * 0.5) * 10, 2, cos(time * 0.5) * 10);
			mainCam.m_direction = Normalize(Vec3Single(0, 0, 0) - mainCam.m_position);

		}

		std::vector<Vec3Single> frameNormal(IMAGE_WIDTH * IMAGE_HEIGHT);
		std::vector<Vec3Single> frame = mainCam.Render(scene, samples, &frameNormal);

		for (int i = 0; i < IMAGE_WIDTH * IMAGE_HEIGHT; i++)
		{
			frameUpdatesData[i] -= deltaTime;
			Vec3Single frameColor = frame[i];
			if (showNormals) frameColor = frameNormal[i];
			Vec3Single frameNormalColor = frameNormal[i];
			float r = frameColor.x() * 0xff;
			float g = frameColor.y() * 0xff;
			float b = frameColor.z() * 0xff;
			float nR = frameNormalColor.x() * 0xff;
			float nG = frameNormalColor.y() * 0xff;
			float nB = frameNormalColor.z() * 0xff;
			if (accumulatorOn)
			{
				float oldr = frameTextureData[i * 3 + 0];
				float oldg = frameTextureData[i * 3 + 1];
				float oldb = frameTextureData[i * 3 + 2];
				float oldNormalR = frameNormalData[i * 3 + 0];
				float oldNormalB = frameNormalData[i * 3 + 2];
				float oldNormalG = frameNormalData[i * 3 + 1];
				float normalDistance = std::sqrt(
					(nR - oldNormalR) * (nR - oldNormalR) +
					(nG - oldNormalG) * (nG - oldNormalG) +
					(nB - oldNormalB) * (nB - oldNormalB)
				);
				if (normalDistance > overrideTreshold || (nR == 127.5f && nG == 127.5f && nB == 127.5f))
				{
					frameUpdatesData[i] = updateTimer;
				}
				if (frameUpdatesData[i] > 0) {
					// override
					if (showChange)
					{
						frameTextureData[i * 3 + 0] = 0xff;  // r
						frameTextureData[i * 3 + 1] = 0;  // G
						frameTextureData[i * 3 + 2] = 0;  // b
					}
					else
					{
						frameTextureData[i * 3 + 0] = r;  // r
						frameTextureData[i * 3 + 1] = g;  // G
						frameTextureData[i * 3 + 2] = b;  // b
					}
				}
				else
				{ // accumalate
					frameTextureData[i * 3 + 0] = oldr * (1.f - smoothingFactor) + r * smoothingFactor;  // G			
					frameTextureData[i * 3 + 1] = oldg * (1.f - smoothingFactor) + g * smoothingFactor;  // G
					frameTextureData[i * 3 + 2] = oldb * (1.f - smoothingFactor) + b * smoothingFactor;  // b
				}
			}
			else
			{
				frameTextureData[i * 3 + 0] = r;  // r
				frameTextureData[i * 3 + 1] = g;  // G
				frameTextureData[i * 3 + 2] = b;  // b
			}

			frameNormalData[i * 3 + 0] = nR;  // r
			frameNormalData[i * 3 + 1] = nG;  // G
			frameNormalData[i * 3 + 2] = nB;  // b

		}

		glUseProgram(screenShader);

		//Draw to texture

		glActiveTexture(GL_TEXTURE0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, IMAGE_WIDTH, IMAGE_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, frameTextureData.data());
		glBindTexture(GL_TEXTURE_2D, frameTexture);

		glUniform1i(glGetUniformLocation(screenShader, "image"), 0);
		glUniform2f(glGetUniformLocation(screenShader, "texResolution"), IMAGE_WIDTH, IMAGE_HEIGHT);
		glUniform1i(glGetUniformLocation(screenShader, "denoise"), denoise);
		glBindVertexArray(screenVAO);
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)30);

		ImGui::EndFrame();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


		glfwSwapBuffers(window);



	}

	glDeleteTextures(1, &frameTexture);







	// Render


	auto time = std::chrono::system_clock::now();

	std::clog << "\x1b[0mStarting Render ";
#ifdef MULTITHREAD
	std::clog << "on " << JobManager::GetJobManager()->MaxConcurrent() << " threads";
#endif
	std::clog << '\n';


	std::chrono::duration<float> delta = std::chrono::system_clock::now() - time;
	std::clog << "Rendering took: " << (delta.count()) << "s\n";

	Logger::DeleteLogger();
	JobManager::DeleteJobManager();
	glfwDestroyWindow(window);
	glfwTerminate();
}
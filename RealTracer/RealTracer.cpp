
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

void main() {
    FragColor = texture(image,v_texCoord);
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
	glGetShaderiv(screenShader, GL_COMPILE_STATUS, &success);
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

	glUseProgram(screenShader);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(screenShader, "image"), 0);
	glBindVertexArray(screenVAO);

	//Material* glass = new DielectricMat(1.5f);
	//Material* glassAirBubble = new DielectricMat(1.f/1.5f);
	//Material* metalRight = new MetalMat(Color(0.8f, 0.2f, 0.8f), 1.f);

	//Scene
	Scene scene;
	scene.Add(*new Sphere(diffuse, 0.f, 0.f, 1.f, 0.5f));
	scene.Add(*new Sphere(red, 2.f, 0.f, -1.f, 0.5f));
	scene.Add(*new Sphere(glass, 4.f, 0.3f, 0.f, 0.8f));
	//scene.Add(*new Sphere(*glass,Vec3(-1.f, 0.f, -1.f), 0.5f));
	//scene.Add(*new Sphere(*metalRight,Vec3(1.f, 0.f, -1.f), 0.5f));
	scene.Add(*new Sphere(grass, 0.f, -100.5f, -1.f, 100.f));

	//Camera
	Camera mainCam;
	mainCam.materials.push_back(new LambertianMat(Color(0.5f, 0.5f, 0.5f)));
	mainCam.materials.push_back(new LambertianMat(Color(0.9f, 0.3f, 0.3f)));
	mainCam.materials.push_back(new LambertianMat(Color(0.2f, 0.8f, 0.2f)));
	mainCam.materials.push_back(new DielectricMat(1.1f));
	mainCam.m_verticalFOV = 20;
	mainCam.m_position = Vec3Single(13, 2, 3);
	mainCam.m_direction = Normalize(Vec3Single(0, 0, 0) - mainCam.m_position);

	mainCam.m_defocusAngle = 0.0f;
	mainCam.m_focusDistance = 10.0f;

	std::vector<float> frameRates(30,0);
	double lastTime = glfwGetTime();
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		double time = glfwGetTime();
		float deltaTime = (time - lastTime);
		frameRates.erase(frameRates.begin());
		frameRates.push_back(1.f / deltaTime);
		lastTime = time;

		ImGui::NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui_ImplOpenGL3_NewFrame();

		static bool open = true;
		ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiCond_Appearing);
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
			ImGui::Text("FPS: %.f (%.2fms)",tot,deltaTime*1000.f);

			ImGui::TreePop();
		}
		ImGui::End();

		dynamic_cast<Sphere*>(scene.GetObjects()[0])->posY = sin(time)*0.5+0.5;
		dynamic_cast<Sphere*>(scene.GetObjects()[2])->posZ = sin(time*0.6)*2;

		std::vector<Vec3Single> frame = mainCam.Render(scene);

		std::vector<unsigned char> frameTextureData(IMAGE_WIDTH*IMAGE_HEIGHT * 3);
		for (int i = 0; i < IMAGE_WIDTH * IMAGE_HEIGHT; i++)
		{
			Vec3Single frameColor = frame[i];
			frameTextureData[i * 3 + 0] = frameColor.x()* 0xff;  // R
			frameTextureData[i * 3 + 1] = frameColor.y()* 0xff;  // G
			frameTextureData[i * 3 + 2] = frameColor.z()* 0xff;  // B	
		}


		//Draw to texture
		uint frameTexture;
		glGenTextures(1, &frameTexture);
		glBindTexture(GL_TEXTURE_2D, frameTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, IMAGE_WIDTH, IMAGE_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, frameTextureData.data());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, frameTexture);

		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)30);

		ImGui::EndFrame();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


		glfwSwapBuffers(window);


		glDeleteTextures(1, &frameTexture);

	}








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
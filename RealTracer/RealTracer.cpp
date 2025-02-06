
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

#include "App.h"
#include "DemoApp.h"
#include "Renderer.h"

void error_callback(int, const char* description)
{
	Logger::LogWarning(description, WARNING_SEVERITY::HIGH);
}
static void key_callback(GLFWwindow* window, int key, int, int action, int)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}


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
uniform sampler2D normals;
uniform bool denoise;

uniform vec2 texResolution;
vec2 offset = 1.0 / texResolution;

float normalDifference(vec3 normal1, vec3 normal2) {
    return length(normal1 - normal2);  // Euclidean distance between normals
}

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

    vec3 centerNormal = texture(normals, v_texCoord).xyz;

    vec3 sampleTex[9];
    vec3 sampleNormals[9];
    float weights[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(image, v_texCoord + offsets[i]));
        sampleNormals[i] = texture(normals, v_texCoord + offsets[i]).xyz;

        float normalDiff = normalDifference(centerNormal, sampleNormals[i]);
            
        //edgeweight to avoid blurring edges
		const float sharpness = 10.0;
        float edgeWeight = exp(-normalDiff * sharpness);  //larger value sharpens more
        weights[i] = kernel[i] * edgeWeight;

    }

    float totalWeight = 0.0;
    for(int i = 0; i < 9; i++) {
        totalWeight += weights[i];
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++) {
        col += sampleTex[i] * weights[i];
    }
    
    FragColor = vec4(col/totalWeight, 1.0);
}else {
	FragColor = texture(image, v_texCoord);
}
}
)";
unsigned int vertex, fragment;

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


GLFWwindow* window;
std::vector<unsigned char> frameTextureData(IMAGE_WIDTH* IMAGE_HEIGHT * 3);
std::vector<unsigned char> frameNormalData(IMAGE_WIDTH* IMAGE_HEIGHT * 3);
std::vector<Vec3> framePosData(IMAGE_WIDTH* IMAGE_HEIGHT);
std::vector<float> frameUpdatesData(IMAGE_WIDTH* IMAGE_HEIGHT);
EngineSettings settings;
unsigned int screenVAO, screenVBO;
uint frameTexture;
uint frameNormalTexture;
unsigned int screenShader;

void Init()
{
	Logger::CreateLogger();
	uint cores = 0;
	uint logical = 0;
	JobManager::GetProcessorCount(cores, logical);
	JobManager::CreateJobManager(logical * 2);

	//Set up a window
	if (!glfwInit())
	{
		Logger::LogWarning("GLFW Failed to Init", WARNING_SEVERITY::FATAL);
		abort();
	}
	glfwSetErrorCallback(error_callback);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "RealTracer", NULL, NULL);
	if (!window)
	{
		Logger::LogWarning("GLFW Window or OpenGL context creation failed", WARNING_SEVERITY::FATAL);
		abort();
	}
	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);
	if (!gladLoadGL())
	{
		Logger::LogWarning("OpenGL context creation failed", WARNING_SEVERITY::FATAL);
		abort();
	}
	glfwSwapInterval(VSYNC_ENABLED);

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


	std::clog << "\x1B[36mSystem Info: \n"
		<< "\x1B[36m  Cores: \x1B[96m" << cores << '\n'
		<< "\x1B[36m   Logical: \x1B[96m" << logical << '\n'
		<< "\x1B[36m  Threads: \x1B[96m" << JobManager::GetJobManager()->MaxConcurrent() << '\n'
		<< "\x1B[36m  SIMD Architecture: \x1B[96m"
#if SSE2
		<< "SSE2"
#else
#if AVX2
		<< "AVX2"
#else
#if AVX512
		<< "AVX512"
#else
		<< "\x1B[31\x1B[24ERROR"
#endif
#endif
#endif
		<< "\x1B[0m\n"
		<< "\x1B[36m  SIMD size:\x1B[96m " << SIMD_SIZE << '\n'
		<< "\x1B[36mSettings: \n"
		<< "\x1B[36m  Image Size:\x1B[96m " << IMAGE_WIDTH << "x" << IMAGE_HEIGHT << '\n'
		<< "\x1B[36m  Samples: \x1B[96m" << SAMPLES_PER_PIXEL * SIMD_SIZE << '\n';

}

void RenderScreen()
{
	glUseProgram(screenShader);
	glActiveTexture(GL_TEXTURE0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, IMAGE_WIDTH, IMAGE_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, frameTextureData.data());
	glBindTexture(GL_TEXTURE_2D, frameTexture);
	glActiveTexture(GL_TEXTURE1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, IMAGE_WIDTH, IMAGE_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, frameNormalData.data());
	glBindTexture(GL_TEXTURE_2D, frameNormalTexture);
	glUniform1i(glGetUniformLocation(screenShader, "image"), 0);
	glUniform1i(glGetUniformLocation(screenShader, "normals"), 1);
	glUniform2f(glGetUniformLocation(screenShader, "texResolution"), IMAGE_WIDTH, IMAGE_HEIGHT);
	glUniform1i(glGetUniformLocation(screenShader, "denoise"), settings.denoise);
	glBindVertexArray(screenVAO);
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)30);
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


	Init();

	GLint success;
	GLchar infoLog[1024];

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
	screenShader = glCreateProgram();
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

	glGenTextures(1, &frameTexture);
	glBindTexture(GL_TEXTURE_2D, frameTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, IMAGE_WIDTH, IMAGE_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, frameTexture);
	glGenTextures(1, &frameNormalTexture);
	glBindTexture(GL_TEXTURE_2D, frameNormalTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, IMAGE_WIDTH, IMAGE_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, frameNormalTexture);

	App* theApp = new DemoApp(*window, settings);
	theApp->Init();
	Renderer* renderer = new Renderer();
	renderer->Init(*theApp, settings);

	double lastTime = glfwGetTime();

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		double time = glfwGetTime();
		float mainDeltaTime = static_cast<float>((time - lastTime));
		lastTime = time;

		ImGui_ImplGlfw_NewFrame();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();

		theApp->FastTick(mainDeltaTime);

		if (renderer->GetFrameReady())
		{
			renderer->CopyBuffer(&frameTextureData, &frameNormalData, &framePosData);
		}

		RenderScreen();

		theApp->Render();

		ImGui::EndFrame();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);

		theApp->PostRender();

	}

	std::clog << "\x1B[43m\x1B[30m EXITING APPLICATION \n\x1B[0m";

	glDeleteTextures(1, &frameTexture);
	glDeleteTextures(1, &frameNormalTexture);

	delete renderer;
	delete theApp;
	Logger::DeleteLogger();
	JobManager::DeleteJobManager();
	glfwDestroyWindow(window);
	glfwTerminate();
}
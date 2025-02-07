#pragma once
#include "xsimd/xsimd.hpp"
namespace xs = xsimd;
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <vector>
#include <chrono>
#include <thread>
#include <Windows.h>
#include <mutex>
#include <algorithm>

#include "Math.h"
#include "Logger.h"

#include "stb_image.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/backends/imgui_impl_glfw.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

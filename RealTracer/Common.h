#pragma once

namespace xs = xsimd;

//Window settings
constexpr int WINDOW_WIDTH = 400;
constexpr int WINDOW_HEIGHT = 300;

//Image settings
constexpr int IMAGE_WIDTH = 200u;
constexpr int IMAGE_HEIGHT = 150u;


//Tracing settings

//Actual samples is this times your available simd size (1, 4, 8, 16)
constexpr int SAMPLES_PER_PIXEL = 2;

constexpr int MAX_BOUNCES = 5;
constexpr float MIN_INTERSECTION_DEPTH = 0.001f;


#define MULTITHREAD


//Materials
enum E_MATERIALS
{
	diffuse,
	red,
	grass,
	glass
};







constexpr float ASPECT_RATIO = static_cast<float>(IMAGE_WIDTH) / static_cast<float>(IMAGE_HEIGHT);
constexpr size_t SIMD_SIZE = xs::batch<float>::size;
constexpr float PIXEL_SAMPLES_SCALE = (1.f / (static_cast<float>(SAMPLES_PER_PIXEL)))/SIMD_SIZE;

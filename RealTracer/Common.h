#pragma once

namespace xs = xsimd;

//Window settings
constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 768;

//Image settings
constexpr int IMAGE_WIDTH = 1280u/4u;
constexpr int IMAGE_HEIGHT = 768u/4u;


//Tracing settings

//Actual samples is this times your available simd size (1, 4, 8, 16)
constexpr int SAMPLES_PER_PIXEL = 1;

constexpr int MAX_BOUNCES = 7;
constexpr float MIN_INTERSECTION_DEPTH = 0.0001f;


#define MULTITHREAD


//Materials
enum E_MATERIALS
{
	diffuse,
	red,
	grass,
	glass,
	metal,
	mirror
};







constexpr float ASPECT_RATIO = static_cast<float>(IMAGE_WIDTH) / static_cast<float>(IMAGE_HEIGHT);
constexpr size_t SIMD_SIZE = xs::batch<float>::size;

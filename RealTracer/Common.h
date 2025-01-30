#pragma once

//Image settings
constexpr int IMAGE_WIDTH = 640u;
constexpr int IMAGE_HEIGHT = 400u;


//Tracing settings

//Actual samples is this times your available simd size (1, 4, 8, 16)
constexpr int SAMPLES_PER_PIXEL = 8;

constexpr int MAX_BOUNCES = 10;
constexpr float MIN_INTERSECTION_DEPTH = 0.001f;










constexpr float ASPECT_RATIO = static_cast<float>(IMAGE_WIDTH) / static_cast<float>(IMAGE_HEIGHT);
constexpr float PIXEL_SAMPLES_SCALE = 1.f / static_cast<float>(SAMPLES_PER_PIXEL);
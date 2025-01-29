#pragma once

//Image settings
constexpr int IMAGE_WIDTH = 480u;
constexpr int IMAGE_HEIGHT = 300u;


//Tracing settings

constexpr uint SAMPLES_PER_PIXEL = 24;
constexpr uint MAX_BOUNCES = 10;
constexpr float MIN_INTERSECTION_DEPTH = 0.001f;










constexpr float ASPECT_RATIO = static_cast<float>(IMAGE_WIDTH) / static_cast<float>(IMAGE_HEIGHT);
constexpr float PIXEL_SAMPLES_SCALE = 1.f / static_cast<float>(SAMPLES_PER_PIXEL);
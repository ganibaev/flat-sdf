#pragma once
#include <cstdint>
#include "fsdf_image.h"
#include "fsdf_shapes.h"
#include <memory>
#include <cmath>
#include <string>
#include <sstream>
#include <thread>
#include <chrono>

struct Scene
{
	std::vector<std::unique_ptr<Shape>> _objects;
	uint32_t _width = 256;
	uint32_t _height = 256;
};

class Renderer {
public:
	std::vector<Scene> _scenes;

	void init();
	void run();
	void sample_pixels(size_t start, int num_workers, const Scene& _scene, Image& result, int sceneIndex);
	void run_scene(const Scene& _scene, int index);

	void saveResult(std::string filePath, Image& result);
};
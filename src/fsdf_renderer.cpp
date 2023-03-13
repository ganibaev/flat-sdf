#include "fsdf_renderer.h"

void Renderer::init()
{
	Scene initials;
	initials._width = 512;
	initials._height = 512;
	initials._objects.emplace_back(new Pic(128, 256, Pixel{ 0, 0, 255, 255 }, 482.0f / 2, "../inputs/letterG.png", Pixel{ 0, 0, 255, 255 }));
	initials._objects.emplace_back(new Pic(256, 256, Pixel{ 0, 0, 255, 255 }, 256.0f / 2, "../inputs/letterA.png", Pixel{ 0, 0, 255, 255 }));
	initials._objects.emplace_back(new Pic(384, 256, Pixel{ 0, 0, 255, 255 }, 380.0f / 2, "../inputs/letterN.png", Pixel{ 0, 0, 255, 255 }));

	_scenes.push_back(std::move(initials));

	Scene big;

	big._width = 2048;
	big._height = 2048;
	big._objects.emplace_back(new Line(1024, 1800, Pixel{ 0, 0, 255, 255 }, 1024.0f));
	big._objects.emplace_back(new Rectangle(512, 512, Pixel{ 127, 0, 127, 255 }, 128.0f));
	big._objects.emplace_back(new Circle(512, 512, Pixel{ 255, 0, 0, 255 }, 256.0f));
	big._objects.emplace_back(new Rectangle(1536, 512, Pixel{ 127, 0, 127, 255 }, 128.0f, true));
	big._objects.emplace_back(new Triangle(1024, 1024, Pixel{ 0, 255, 0, 255 }, 256.0f));
	big._objects.emplace_back(new Pic(1024, 512, Pixel{ 0, 0, 255, 255 }, 512.0f, "../inputs/flame.png", Pixel{ 0, 204, 255, 255 }, true));

	_scenes.push_back(std::move(big));

	Scene small;
	small._width = 128;
	small._height = 128;
	small._objects.emplace_back(new Pic(64, 64, Pixel{ 0, 0, 255, 255 }, 64.0f, "../inputs/flame.png", Pixel{ 0, 0, 255, 255 }));

	_scenes.push_back(std::move(small));

}

void Renderer::run()
{
	for (size_t i = 0; i < _scenes.size(); ++i)
	{
		// Benchmark start
		std::chrono::time_point<std::chrono::system_clock> start, end;
		start = std::chrono::system_clock::now();

		run_scene(_scenes[i], i);

		// Benchmark end
		end = std::chrono::system_clock::now();
		std::chrono::duration<double, std::milli> elapsed_time = end - start;
		std::cout << "Rendering scene " << i << " took " << elapsed_time.count() << " milliseconds." << std::endl;
	}
}

void Renderer::sample_pixels(size_t start, int num_workers, const Scene& _scene, Image& result, int sceneIndex)
{
	for (size_t ind = start; ind < result._height * static_cast<size_t>(result._width); ind += num_workers)
	{
		int x = ind % result._width;
		int y = ind / result._width;
		float min_sdf = 1.00f;
		int min_ind = -1;
		for (size_t i = 0; i < _scene._objects.size(); ++i)
		{
			float sdf = 1.0f;
			float x_lookup = (x - _scene._objects[i]->_xPosition) * _scene._objects[i]->_scale + 128;
			float y_lookup = (y - _scene._objects[i]->_yPosition) * _scene._objects[i]->_scale + 128;
			if (x_lookup < 0.0f || y_lookup < 0.0f || x_lookup > 255.0f || y_lookup > 255.0f)
			{
				continue;
			}
			int x1 = std::floor(x_lookup), x2 = std::ceil(x_lookup), y1 = std::floor(y_lookup), y2 = std::ceil(y_lookup);
			Pixel secondColor = _scene._objects[i]->_color;
			float sdf11 = 0, sdf12 = 0, sdf21 = 0, sdf22 = 0;
			switch (_scene._objects[i]->_type)
			{
			case 'c':
			{
				Circle* circ = dynamic_cast<Circle*>(_scene._objects[i].get());
				sdf11 = static_cast<float>(circ->getSDF(x1, y1)) / 255.0f;
				sdf12 = static_cast<float>(circ->getSDF(x1, y2)) / 255.0f;
				sdf21 = static_cast<float>(circ->getSDF(x2, y1)) / 255.0f;
				sdf22 = static_cast<float>(circ->getSDF(x2, y2)) / 255.0f;
				sdf = blerp(sdf11, sdf12, sdf21, sdf22, x1, x2, y1, y2, x_lookup, y_lookup);
				break;
			}
			case 'r':
			{
				Rectangle* rect = dynamic_cast<Rectangle*>(_scene._objects[i].get());
				sdf11 = static_cast<float>(rect->getSDF(x1, y1)) / 255.0f;
				sdf12 = static_cast<float>(rect->getSDF(x1, y2)) / 255.0f;
				sdf21 = static_cast<float>(rect->getSDF(x2, y1)) / 255.0f;
				sdf22 = static_cast<float>(rect->getSDF(x2, y2)) / 255.0f;
				sdf = blerp(sdf11, sdf12, sdf21, sdf22, x1, x2, y1, y2, x_lookup, y_lookup);
				break;
			}
			case 't':
			{
				Triangle* triangle = dynamic_cast<Triangle*>(_scene._objects[i].get());
				sdf11 = static_cast<float>(triangle->getSDF(x1, y1)) / 255.0f;
				sdf12 = static_cast<float>(triangle->getSDF(x1, y2)) / 255.0f;
				sdf21 = static_cast<float>(triangle->getSDF(x2, y1)) / 255.0f;
				sdf22 = static_cast<float>(triangle->getSDF(x2, y2)) / 255.0f;
				sdf = blerp(sdf11, sdf12, sdf21, sdf22, x1, x2, y1, y2, x_lookup, y_lookup);
				break;
			}
			case 'l':
			{
				Line* line = dynamic_cast<Line*>(_scene._objects[i].get());
				sdf11 = static_cast<float>(line->getSDF(x1, y1)) / 255.0f;
				sdf12 = static_cast<float>(line->getSDF(x1, y2)) / 255.0f;
				sdf21 = static_cast<float>(line->getSDF(x2, y1)) / 255.0f;
				sdf22 = static_cast<float>(line->getSDF(x2, y2)) / 255.0f;
				sdf = blerp(sdf11, sdf12, sdf21, sdf22, x1, x2, y1, y2, x_lookup, y_lookup);
				break;
			}
			case 'p':
			{
				Pic* pic = dynamic_cast<Pic*>(_scene._objects[i].get());
				secondColor = pic->_topColor;
				sdf11 = static_cast<float>(pic->getSDF(x1, y1)) / 255.0f;
				sdf12 = static_cast<float>(pic->getSDF(x1, y2)) / 255.0f;
				sdf21 = static_cast<float>(pic->getSDF(x2, y1)) / 255.0f;
				sdf22 = static_cast<float>(pic->getSDF(x2, y2)) / 255.0f;
				sdf = blerp(sdf11, sdf12, sdf21, sdf22, x1, x2, y1, y2, x_lookup, y_lookup);
				break;
			}
			default:
				break;
			}

			if (sdf < min_sdf)
			{
				min_sdf = sdf;
				Pixel endColor;
				if (_scene._objects[i]->_outline && sdf > 0.0f && sdf < 1.0f)
				{
					endColor.r = 0;
					endColor.g = 0;
					endColor.b = 0;
					endColor.a = _scene._objects[i]->_color.a;
					result.setPixel(x, y, endColor);
				}
				else
				{
					Pixel primColor = _scene._objects[i]->_color;
					float y_frac = y_lookup / 255.0f;
					Pixel pointColor;
					pointColor.r = lerp(primColor.r, secondColor.r, 1.0f, 0.0f, y_frac);
					pointColor.g = lerp(primColor.g, secondColor.g, 1.0f, 0.0f, y_frac);
					pointColor.b = lerp(primColor.b, secondColor.b, 1.0f, 0.0f, y_frac);
					pointColor.a = primColor.a;

					endColor.r = (1 - sdf) * pointColor.r + sdf * endColor.r;
					endColor.g = (1 - sdf) * pointColor.g + sdf * endColor.g;
					endColor.b = (1 - sdf) * pointColor.b + sdf * endColor.b;
					endColor.a = pointColor.a;
					result.setPixel(x, y, endColor);
				}
			}
		}
	}
}

void Renderer::run_scene(const Scene& _scene, int index)
{
	Image result;
	result.setExtent(_scene._width, _scene._height);
	const int num_threads = std::thread::hardware_concurrency();
	std::vector<std::thread> workers;
	workers.reserve(num_threads);

	for (size_t id = 0; id < num_threads; ++id) {
		workers.emplace_back(&Renderer::sample_pixels, this, id, num_threads, std::cref(_scene), std::ref(result), index);
	}

	for (auto& thread : workers)
	{
		thread.join();
	}

	std::ostringstream filePathStream;
	filePathStream << "../scene" << index << ".bmp";
	saveResult(filePathStream.str(), result);
}

void Renderer::saveResult(std::string filePath, Image& result)
{
	result.writeToFile(filePath.c_str());
}
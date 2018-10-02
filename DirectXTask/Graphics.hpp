#pragma once

//LIBS
#include <memory>

//SELF
#include "Direct3D.hpp"
#include "Camera.hpp"
#include "Model.hpp"
#include "ColourShader.hpp"

class Graphics
{
public:
	Graphics(int width, int height, HWND window);
	~Graphics();

	bool frame();

	static constexpr bool fullscreen = false;
	static constexpr bool vsync = true;
	static constexpr float screen_depth = 1000.0f;
	static constexpr float screen_near = 0.1f;

private:
	bool render();

	std::unique_ptr<Direct3D> direct3d;
	std::unique_ptr<Camera> camera;
	std::unique_ptr<Model> model;
	std::unique_ptr<ColourShader> colour_shader;
};
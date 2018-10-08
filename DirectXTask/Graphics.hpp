#pragma once

//LIBS
#include <memory>

//SELF
#include "Direct3D.hpp"
#include "Camera.hpp"
#include "Model.hpp"
#include "ColourShader.hpp"
#include "World.hpp"

class Graphics
{
public:
	Graphics(int width, int height, HWND window);

	bool update(Input* input, float dt);
	bool draw();

	static constexpr bool fullscreen = false;
	static constexpr bool vsync = true;
	static constexpr float screen_depth = 1000.0f;
	static constexpr float screen_near = 0.1f;

private:
	float time = 0;
	HWND window;

	std::unique_ptr<Direct3D> direct3d;
	std::unique_ptr<Camera> camera;
	std::unique_ptr<Model> model;
	std::unique_ptr<World> world;
	std::unique_ptr<ColourShader> colour_shader;
	Input* input;
};
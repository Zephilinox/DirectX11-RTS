#pragma once

//STD
#include <memory>

//SELF
#include "Camera.hpp"
#include "Direct3D.hpp"
#include "Input.hpp"
#include "ColourShader.hpp"
#include "Model.hpp"

class World
{
public:
	World(Direct3D* d3d, HWND window, int width, int height, float screen_depth);
	~World();

	bool update(Input* input, float dt);
	bool draw(Direct3D* d3d, ColourShader* shader);

private:
};
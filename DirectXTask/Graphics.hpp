#pragma once

//LIBS
#include <memory>

//SELF
#include "Direct3D.hpp"
#include "Camera.hpp"
#include "Model.hpp"
#include "ColourShader.hpp"
#include "World.hpp"
#include "Pathfinding.hpp"
#include "Entity.hpp"

class Graphics
{
public:
	Graphics(int width, int height, HWND window);

	bool update(Input* input, float dt);
	bool draw();

	static constexpr bool fullscreen = true;
	static constexpr bool vsync = false;
	static constexpr float screen_depth = 1000.0f;
	static constexpr float screen_near = 0.1f;

private:
	float time = 0;
	HWND window;

	std::unique_ptr<Direct3D> direct3d;
	std::unique_ptr<Camera> camera;
	std::unique_ptr<Model> sphere;
	std::unique_ptr<Model> cube;
	std::unique_ptr<World> world;
	std::unique_ptr<Pathfinding> pathfinding;
	std::unique_ptr<ColourShader> colour_shader;
	std::vector<Model::Instance> sphere_instances;
	std::vector<Model::Instance> cube_instances;
	std::vector<std::unique_ptr<Entity>> entities;

	Input* input;
};
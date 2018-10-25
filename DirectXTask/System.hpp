#pragma once

//STD
#include <memory>

//SELF
#include "Input.hpp"
#include "Graphics.hpp"
#include "Timer.hpp"
#include "World.hpp"
#include "Window.hpp"
#include "Group.hpp"

class System
{
public:
	System();

	void run();

private:
	void event();
	void update();
	void draw();

	std::unique_ptr<Input> input;
	std::unique_ptr<Graphics> graphics;
	std::unique_ptr<World> world;
	std::unique_ptr<Window> window;
	std::unique_ptr<Camera> camera;
	std::unique_ptr<Pathfinding> pathfinding;
	std::vector<ColourShader::Instance> sphere_instances;
	std::vector<ColourShader::Instance> cube_instances;
	std::vector<ColourShader::Instance> entity_instances;
	std::vector<ColourShader::Instance> pathfinding_instances;
	std::vector<std::unique_ptr<Entity>> entities;
	std::unique_ptr<Model> sphere;
	std::unique_ptr<Model> cube;
	std::unique_ptr<Group> group_1;
	std::unique_ptr<Group> group_2;
	std::unique_ptr<Group> group_3;
	std::unique_ptr<Group> group_4;
	std::unique_ptr<Group> group_5;
	int active_group = 0;
	Timer frame_timer;
	float dt;
	float time;
	float timeSinceLastFPSPrint = 0;
	bool quit = false;
};
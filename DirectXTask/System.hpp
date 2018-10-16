#pragma once

//STD
#include <memory>

//SELF
#include "Input.hpp"
#include "Graphics.hpp"
#include "Timer.hpp"
#include "World.hpp"
#include "Window.hpp"

class System
{
public:
	System();

	void run();

private:
	bool update();
	bool draw();

	std::unique_ptr<Input> input;
	std::unique_ptr<Graphics> graphics;
	std::unique_ptr<World> world;
	std::unique_ptr<Window> window;
	Timer frame_timer;
	float dt;
	float timeSinceLastFPSPrint = 0;
};
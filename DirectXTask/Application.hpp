#pragma once

//SELF
#include "System.hpp"

class Application
{
public:
	Application();

	void run();

	void update(float dt);
	void draw();

private:
	System system;
};
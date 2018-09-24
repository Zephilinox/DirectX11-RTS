#pragma once

//LIBS
#include <memory>

//SELF
#include "Direct3D.hpp"

class Graphics
{
public:
	bool init(int width, int height, HWND window);
	void stop();
	bool frame();

	static constexpr bool fullscreen = false;
	static constexpr bool vsync = true;
	static constexpr float screen_depth = 1000.0f;
	static constexpr float screen_near = 0.1f;

private:
	bool render();
	std::unique_ptr<Direct3D> direct3d;
};
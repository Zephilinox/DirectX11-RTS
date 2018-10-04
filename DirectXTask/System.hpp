#pragma once

//STD
#include <memory>

//LIBS
#define WINDOWS32_LEAN_AND_MEAN
#include <Windows.h>

//SELF
#include "Input.hpp"
#include "Graphics.hpp"
#include "Timer.hpp"
#include "World.hpp"

class System
{
public:
	System();
	~System();

	void run();

	LRESULT CALLBACK MessageHandler(HWND window, UINT msg, WPARAM wparam, LPARAM lparan);

private:
	bool update();
	bool draw();

	void create_window(int& width, int& height);
	void destroy_window();

	LPCSTR name;
	HINSTANCE instance = nullptr;
	HWND window = nullptr;

	std::unique_ptr<Input> input;
	std::unique_ptr<Graphics> graphics;
	std::unique_ptr<World> world;
	Timer frame_timer;
	float dt;
};

static LRESULT CALLBACK WndProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam);
//todo make system a singleton or provide locator
static System* application = 0;
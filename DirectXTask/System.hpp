#include <memory>
#pragma once

#define WINDOWS32_LEAN_AND_MEAN
#include <Windows.h>

#include "Input.hpp"
#include "Graphics.hpp"

class System
{
public:
	bool init();
	void stop();
	void run();

	LRESULT CALLBACK MessageHandler(HWND window, UINT msg, WPARAM wparam, LPARAM lparan);

private:
	bool frame();
	void create_window(int& width, int& height);
	void destroy_window();

	LPCSTR name;
	HINSTANCE instance = nullptr;
	HWND window = nullptr;

	std::unique_ptr<Input> input = nullptr;
	std::unique_ptr<Graphics> graphics = nullptr;
};

static LRESULT CALLBACK WndProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam);
//todo make system a singleton or provide locator
static System* application = 0;
#pragma once

//STD
#include <string>

//LIBS
#define WINDOWS32_LEAN_AND_MEAN
#include <Windows.h>

struct WindowSettings
{
	std::string title;
	int width;
	int height;
	bool fullscreen;
	bool vsync;
};

class Window;

static LRESULT CALLBACK WndProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam);
static Window* window = 0;

class Window
{
public:
	Window(WindowSettings settings);
	~Window();

	HWND window_handle;

private:
	LRESULT CALLBACK MessageHandler(HWND window, UINT msg, WPARAM wparam, LPARAM lparan);
	HINSTANCE instance_handle;
	
	WindowSettings settings;

	friend static LRESULT CALLBACK WndProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam);
};




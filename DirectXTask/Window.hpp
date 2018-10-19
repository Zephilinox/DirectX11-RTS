#pragma once

//STD
#include <string>
#include <variant>
#include <type_traits>

//LIBS
#define WINDOWS32_LEAN_AND_MEAN
#include <Windows.h>
#include <Windowsx.h>

struct EventWindowClosed
{
	
};

struct EventMouseMoved
{
	float x = 0;
	float y = 0;
	int modifier_key = 0;
};

struct EventKeyPress
{
	unsigned int key_code = 0;
	bool key_down;
};

struct EventMouseKeyPress
{
	unsigned int mouse_key_code = 0;
	bool key_down;
};

using Event = std::variant<EventWindowClosed, EventMouseMoved, EventKeyPress, EventMouseKeyPress>;

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

	bool poll(Event& e);

	HWND window_handle;
	bool open = false;

	float width;
	float height;
	bool fullscreen;
	bool vsync;

private:
	LRESULT CALLBACK MessageHandler(HWND window, UINT msg, WPARAM wparam, LPARAM lparan);
	HINSTANCE instance_handle;
	
	WindowSettings settings;

	friend static LRESULT CALLBACK WndProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam);
};




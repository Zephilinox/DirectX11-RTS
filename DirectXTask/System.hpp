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

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool frame();
	void init_windows(int&, int&);
	void stop_windows();

	LPCSTR name;
	HINSTANCE instance = nullptr;
	HWND window = nullptr;

	std::unique_ptr<Input> input = nullptr;
	std::unique_ptr<Graphics> graphics = nullptr;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static System* application = 0;
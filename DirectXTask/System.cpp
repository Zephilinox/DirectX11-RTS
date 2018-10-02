#include "System.hpp"

bool System::init()
{
	int width = 0;
	int height = 0;

	create_window(width, height);

	input = std::make_unique<Input>();
	if (!input)
	{
		return false;
	}

	graphics = std::make_unique<Graphics>();
	if (!graphics)
	{
		return false;
	}
	
	return graphics->init(width, height, window);
}

void System::stop()
{
	if (graphics)
	{
		graphics->stop();
		graphics = nullptr;
	}

	if (input)
	{
		input = nullptr;
	}

	destroy_window();
}

void System::run()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	bool quit = false;
	while (!quit)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
		{
			quit = true;
		}	
		else
		{
			quit = frame();
		}
	}
}

bool System::frame()
{
	if (input->is_key_down(VK_ESCAPE))
	{
		return true;
	}

	return graphics->frame();
}

LRESULT CALLBACK System::MessageHandler(HWND window, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
		case WM_KEYDOWN:
		{
			input->key_down(static_cast<unsigned int>(wparam));
			return 0;
		} break;

		case WM_KEYUP:
		{
			input->key_up(static_cast<unsigned int>(wparam));
			return 0;
		} break;
	}

	return DefWindowProc(window, msg, wparam, lparam);
}

void System::create_window(int& width, int& height)
{
	application = this;
	instance = GetModuleHandle(0);
	name = "DirectX11";

	WNDCLASSEX window_settings;
	window_settings.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	window_settings.lpfnWndProc = WndProc;
	window_settings.cbClsExtra = 0;
	window_settings.cbWndExtra = 0;
	window_settings.hInstance = instance;
	window_settings.hIcon = LoadIcon(0, IDI_WINLOGO);
	window_settings.hIconSm = window_settings.hIcon;
	window_settings.hCursor = LoadCursor(0, IDC_HAND);
	window_settings.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	window_settings.lpszMenuName = 0;
	window_settings.lpszClassName = name;
	window_settings.cbSize = sizeof(WNDCLASSEX);

	RegisterClassEx(&window_settings);

	width = GetSystemMetrics(SM_CXSCREEN);
	height = GetSystemMetrics(SM_CYSCREEN);

	DEVMODE screen_settings;
	int x = 0;
	int y = 0;

	if (Graphics::fullscreen)
	{
		memset(&screen_settings, 0, sizeof(screen_settings));
		screen_settings.dmSize = sizeof(screen_settings);
		screen_settings.dmPelsWidth = static_cast<unsigned long>(width);
		screen_settings.dmPelsHeight = static_cast<unsigned long>(height);
		screen_settings.dmBitsPerPel = 32;
		screen_settings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&screen_settings, CDS_FULLSCREEN);
	}
	else
	{
		int old_width = width;
		int old_height = height;

		width = 800;
		height = 600;

		x = (old_width - width) / 2;
		y = (old_height - height) / 2;
	}


	//https://stackoverflow.com/questions/3970066/creating-a-transparent-window-in-c-win32
	window = CreateWindowEx(0,
		name, name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU,
		x, y, width, height,
		0, 0, instance, 0);

	ShowWindow(window, SW_SHOW);
	SetWindowLong(window, GWL_EXSTYLE, GetWindowLong(window, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(window, RGB(204, 51, 127), 0, LWA_COLORKEY);
	SetForegroundWindow(window);
	SetFocus(window);
	ShowCursor(true);
}

void System::destroy_window()
{
	ShowCursor(true);

	if (Graphics::fullscreen)
	{
		ChangeDisplaySettings(0, 0);
	}

	DestroyWindow(window);
	window = 0;

	UnregisterClass(name, instance);
	instance = 0;

	application = 0;
}

LRESULT CALLBACK WndProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		} break;

		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		} break;

		default:
		{
			return application->MessageHandler(window, msg, wparam, lparam);
		}
	}
}
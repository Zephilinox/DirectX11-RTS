#include "Window.hpp"

Window::Window(WindowSettings settings)
{
	window = this;
	instance_handle = GetModuleHandle(0);
	this->settings = settings;

	WNDCLASSEX window_settings;
	window_settings.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	window_settings.lpfnWndProc = WndProc;
	window_settings.cbClsExtra = 0;
	window_settings.cbWndExtra = 0;
	window_settings.hInstance = instance_handle;
	window_settings.hIcon = LoadIcon(0, IDI_WINLOGO);
	window_settings.hIconSm = window_settings.hIcon;
	window_settings.hCursor = LoadCursor(0, IDC_ARROW);
	window_settings.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	window_settings.lpszMenuName = 0;
	window_settings.lpszClassName = settings.title.c_str();
	window_settings.cbSize = sizeof(WNDCLASSEX);

	RegisterClassEx(&window_settings);

	int system_width = GetSystemMetrics(SM_CXSCREEN);
	int system_height = GetSystemMetrics(SM_CYSCREEN);
	int width = settings.width;;
	int height = settings.height;

	DEVMODE screen_settings;
	int window_x = 0;
	int window_y = 0;

	if (settings.fullscreen)
	{
		memset(&screen_settings, 0, sizeof(screen_settings));
		screen_settings.dmSize = sizeof(screen_settings);
		screen_settings.dmPelsWidth = static_cast<unsigned int>(system_width);
		screen_settings.dmPelsHeight = static_cast<unsigned int>(system_height);
		screen_settings.dmBitsPerPel = 32;
		screen_settings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&screen_settings, CDS_FULLSCREEN);
	}
	else
	{
		RECT desired_window_size;
		desired_window_size.left = 0;
		desired_window_size.top = 0;
		desired_window_size.right = width;
		desired_window_size.bottom = height;

		AdjustWindowRectEx(&desired_window_size, WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU, false, 0);

		width = desired_window_size.right - desired_window_size.left;
		height = desired_window_size.bottom - desired_window_size.top;
		window_x = (system_width - width) / 2;
		window_y = (system_height - height) / 2;
	}

	window_handle = CreateWindowExA(0,
		settings.title.c_str(), settings.title.c_str(),
		WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU,
		window_x, window_y, width, height,
		0, 0, instance_handle, nullptr);

	ShowWindow(window_handle, SW_SHOW);
	SetWindowLongA(window_handle, GWL_EXSTYLE, GetWindowLong(window_handle, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetForegroundWindow(window_handle);
	SetFocus(window_handle);
	ShowCursor(true);
}

Window::~Window()
{
	ShowCursor(true);

	if (settings.fullscreen)
	{
		ChangeDisplaySettings(0, 0);
	}

	DestroyWindow(window_handle);
	window = 0;

	UnregisterClass(settings.title.c_str(), instance_handle);
	instance_handle = 0;

	window = nullptr;
}

LRESULT Window::MessageHandler(HWND window, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
	{
		//input->key_down(static_cast<unsigned int>(wparam));
		return 0;
	} break;

	case WM_KEYUP:
	{
		//input->key_up(static_cast<unsigned int>(wparam));
		return 0;
	} break;
	}

	return DefWindowProc(window, msg, wparam, lparam);
}

LRESULT CALLBACK WndProc(HWND window_handle, UINT msg, WPARAM wparam, LPARAM lparam)
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
			return window->MessageHandler(window_handle, msg, wparam, lparam);
		}
	}
}

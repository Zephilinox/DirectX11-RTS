#include "Graphics.hpp"

bool Graphics::init(int width, int height, HWND window)
{
	direct3d = std::make_unique<Direct3D>();
	if (!direct3d)
	{
		return false;
	}

	bool result = direct3d->init(width, height, vsync, window, fullscreen, screen_depth, screen_near);
	if (!result)
	{
		MessageBox(window, "Could not init Direct3D", "ERROR", MB_OK);
		return false;
	}

	return true;
}

void Graphics::stop()
{
	if (direct3d)
	{
		direct3d->stop();
		direct3d = nullptr;
	}
}

bool Graphics::frame()
{
	return render();
}

bool Graphics::render()
{
	direct3d->begin(0.8f, 0.2f, 0.5f, 1.0f);
	direct3d->end();

	return false;
}

#include "Graphics.hpp"

bool Graphics::init(int width, int height, HWND window)
{
	direct3d = std::make_unique<Direct3D>();
	bool result = direct3d->init(width, height, vsync, window, fullscreen, screen_depth, screen_near);
	if (!result)
	{
		MessageBox(window, "Could not init Direct3D", "ERROR", MB_OK);
		return false;
	}

	camera = std::make_unique<Camera>();
	if (!camera)
	{
		return false;
	}

	camera->set_pos(0.0f, 0.0f, -5.0f);

	model = std::make_unique<Model>();
	if (!model)
	{
		return false;
	}

	result = model->init(direct3d->get_device());
	if (!result)
	{
		MessageBoxW(window, L"Could not init model", L"ERROR", MB_OK);
		return false;
	}

	colour_shader = std::make_unique<ColourShader>();
	if (!colour_shader)
	{
		return false;
	}

	result = colour_shader->init(direct3d->get_device(), window);
	if (!result)
	{
		MessageBoxW(window, L"Could not init shader", L"ERROR", MB_OK);
		return false;
	}

	return true;
}

void Graphics::stop()
{
	if (colour_shader)
	{
		colour_shader->stop();
		colour_shader = nullptr;
	}

	if (model)
	{
		model->stop();
		model = nullptr;
	}

	if (camera)
	{
		camera = nullptr;
	}

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
	direct3d->begin(0.8f, 0.2f, 0.5f, 0.5f);

	camera->render();

	dx::XMMATRIX world;
	direct3d->get_world_matrix(world);
	dx::XMMATRIX view;
	camera->get_view_matrix(view);
	dx::XMMATRIX projection;
	direct3d->get_projection_matrix(projection);

	model->render(direct3d->get_device_context());

	bool result = colour_shader->render(direct3d->get_device_context(), model->get_index_count(), world, view, projection);
	if (!result)
	{
		return false;
	}

	direct3d->end();

	return false;
}

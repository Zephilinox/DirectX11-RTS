#include "Graphics.hpp"

Graphics::Graphics(int width, int height, HWND window)
{
	direct3d = std::make_unique<Direct3D>();
	bool result = direct3d->init(width, height, vsync, window, fullscreen, screen_depth, screen_near);
	if (!result)
	{
		MessageBox(window, "Could not init Direct3D", "ERROR", MB_OK);
		throw;
	}

	camera = std::make_unique<Camera>();
	camera->set_pos(0.0f, 0.0f, -5.0f);

	try
	{
		model = std::make_unique<Model>(direct3d->get_device());
	}
	catch (...)
	{
		MessageBoxW(window, L"Could not init model", L"ERROR", MB_OK);
	}

	colour_shader = std::make_unique<ColourShader>(direct3d->get_device(), window);
}

Graphics::~Graphics()
{
	colour_shader = nullptr;
	model = nullptr;
	camera = nullptr;

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

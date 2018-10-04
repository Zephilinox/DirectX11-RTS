#include "Graphics.hpp"

Graphics::Graphics(int width, int height, HWND window)
{
	try
	{
		direct3d = std::make_unique<Direct3D>(width, height, vsync, window, fullscreen, screen_depth, screen_near);
	}
	catch (...)
	{
		MessageBox(window, "Could not init Direct3D", "ERROR", MB_OK);
		throw;
	}

	camera = std::make_unique<Camera>();
	camera->set_pos(32.0f, 10.0f, -10.0f);
	camera->set_rot(20, 0, 0);

	try
	{
		model = std::make_unique<Model>(direct3d->get_device());
	}
	catch (...)
	{
		MessageBoxW(window, L"Could not init Model", L"ERROR", MB_OK);
	}

	try
	{
		world = std::make_unique<World>(direct3d->get_device());
	}
	catch (...)
	{
		MessageBoxW(window, L"Could not init World", L"ERROR", MB_OK);
	}

	colour_shader = std::make_unique<ColourShader>(direct3d->get_device(), window);
}

bool Graphics::update(Input* input, float dt)
{
	camera->update(input, dt);
	return false;
}

bool Graphics::draw()
{
	direct3d->begin(0.0f, 0.0f, 0.0f, 1.0f);

	camera->draw();
	dx::XMMATRIX world_matrix = direct3d->get_world_matrix();
	dx::XMMATRIX view_matrix =	camera->get_view_matrix();
	dx::XMMATRIX projection_matrix = direct3d->get_projection_matrix();

	model->render(direct3d->get_device_context());

	bool result = colour_shader->render(direct3d->get_device_context(), model->get_index_count(), model->get_vertex_count(), model->get_instance_count(), world_matrix, view_matrix, projection_matrix);
	if (!result)
	{
		return false;
	}

	camera->draw();
	
	world->draw(direct3d->get_device_context());
	result = colour_shader->render(direct3d->get_device_context(), world->get_index_count(), world_matrix, view_matrix, projection_matrix);
	if (!result)
	{
		return false;
	}

	direct3d->end();

	return false;
}

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
	camera->set_pos(0.0f, 0.0f, -5.0f);

	try
	{
		model = std::make_unique<Model>(direct3d->get_device());
		model2 = std::make_unique<Model>(direct3d->get_device(), dx::XMFLOAT3(-2.0f, -2.0f, 0.0f), dx::XMFLOAT3(-1.0F, 0.0f, 0.0f), dx::XMFLOAT3(0.0F, -2.0f, 0.0f));
	}
	catch (...)
	{
		MessageBoxW(window, L"Could not init Model", L"ERROR", MB_OK);
	}

	colour_shader = std::make_unique<ColourShader>(direct3d->get_device(), window);
}

Graphics::~Graphics()
{
}

bool Graphics::frame()
{
	return render();
}

bool Graphics::render()
{
	direct3d->begin(0.8f, 0.2f, 0.5f, 0.5f);

	if (GetAsyncKeyState('A'))
	{
		camera->set_pos(
			camera->get_pos().x - 0.4f,
			camera->get_pos().y,
			camera->get_pos().z);
	}

	if (GetAsyncKeyState('D'))
	{
		camera->set_pos(
			camera->get_pos().x + 0.4f,
			camera->get_pos().y,
			camera->get_pos().z);
	}

	if (GetAsyncKeyState('W'))
	{
		camera->set_pos(
			camera->get_pos().x,
			camera->get_pos().y + 0.4f,
			camera->get_pos().z);
	}

	if (GetAsyncKeyState('S'))
	{
		camera->set_pos(
			camera->get_pos().x,
			camera->get_pos().y - 0.4f,
			camera->get_pos().z);
	}

	if (GetAsyncKeyState('Q'))
	{
		camera->set_pos(
			camera->get_pos().x,
			camera->get_pos().y,
			camera->get_pos().z + 0.4f);
	}

	if (GetAsyncKeyState('E'))
	{
		camera->set_pos(
			camera->get_pos().x,
			camera->get_pos().y,
			camera->get_pos().z - 0.4f);
	}

	if (GetAsyncKeyState('Z'))
	{
		camera->set_rot(
			camera->get_rot().x + 4.0f,
			camera->get_rot().y,
			camera->get_rot().z);
	}

	if (GetAsyncKeyState('X'))
	{
		camera->set_rot(
			camera->get_rot().x - 4.0f,
			camera->get_rot().y,
			camera->get_rot().z);
	}

	camera->render();

	dx::XMMATRIX world = direct3d->get_world_matrix();
	dx::XMMATRIX view =	camera->get_view_matrix();
	dx::XMMATRIX projection = direct3d->get_projection_matrix();

	if (rand() % 2)
	{
		model->render(direct3d->get_device_context());
	}
	else
	{
		model2->render(direct3d->get_device_context());
	}

	bool result = colour_shader->render(direct3d->get_device_context(), model->get_index_count(), model->get_vertex_count(), model->get_instance_count(), world, view, projection);
	if (!result)
	{
		return false;
	}

	direct3d->end();

	return false;
}

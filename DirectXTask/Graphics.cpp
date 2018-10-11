#include "Graphics.hpp"

//STD
#include <iostream>

Graphics::Graphics(int width, int height, HWND window)
{
	this->window = window;

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
	camera->set_pos(32.0f, 5.0f, -5.0f);
	camera->set_rot(40, 0, 0);

	try
	{
		model = std::make_unique<Model>(direct3d->get_device());
	}
	catch (...)
	{
		MessageBoxW(window, L"Could not init Model", L"ERROR", MB_OK);
	}

	float deg2rad = 0.0174533f;

	instances.push_back({
		{ 30.0f - (std::cosf(time) * 4 + 4), 1.0f, 30.0f - (std::sinf(time) * 4 + 4) },
		{ 0.0f, 0.0f, 0.0f },
		{ 2.0f, std::sin(time) + 2.0f, 2.0f },
		{ (std::sin(time) + 1.0f) / 2.0f, 0.5f, 0.5f, 1.0f }
	});

	instances.push_back({
		{ 32.0f, 1.0f, 32.0f },
		{ std::sinf(time) * 180.0f * deg2rad, std::sinf(time) * 180.0f * deg2rad, std::sinf(time) * 180.0f * deg2rad },
		{ 1.0f, 1.0f, 1.0f},
		{ 1.0f, (std::sin(time) + 2.0f) / 2.0f, 0.0f, 1.0f }
	});

	instances.push_back({
		{ 40.0f, 1.0f + std::cosf(time) * 3 + 3, 40.0f },
		{ 0.0f, std::sinf(time) * 180.0f * deg2rad, 0.0f },
		{ 1.0f, std::sin(time) + 2.0f, 1.0f },
		{ 0.0f, 1.0f, (std::sin(time) + 1.0f) / 2.0f, 1.0f }
	});

	instances.push_back({
		{ 44.0f, 1.0f, 44.0f },
		{ 0.0f, 0.0f, std::cosf(time) * 180.0f * deg2rad },
		{ std::sin(time) + 2.0f, 1.0f, 1.0f },
		{ (std::sin(time) + 1.0f) / 2.0f, 0.2f, 0.5f, 1.0f }
	});

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
	time += dt;
	camera->update(input, dt);
	this->input = input;

	float deg2rad = 0.0174533f;

	if (input->is_key_down('C'))
	{
		float rand_x = (rand() % 6400) / 100.0f;
		float rand_y = (rand() % 2000) / 100.0f;
		float rand_z = (rand() % 6400) / 100.0f;

		instances.push_back({
			{ rand_x, rand_y, rand_z},
			{ 0.0f, 0.0f, 0.0f },
			{ 1.0f, 1.0f, 1.0f },
			{ (std::sin(time) + 1.0f) / 2.0f, 0.2f, 0.5f, 1.0f }
			});
	}

	for (auto& instance : instances)
	{
		instance.rotation.x = std::cosf(time) * 180.0f * deg2rad;
		instance.rotation.y = std::cosf(time) * 180.0f * deg2rad;
		instance.rotation.z = std::cosf(time) * 180.0f * deg2rad;
	}

	POINT pos;
	if (GetCursorPos(&pos))
	{
		if (ScreenToClient(window, &pos))
		{
			if ((GetKeyState(VK_LBUTTON) & 0x100) != 0)
			{
				dx::XMMATRIX world_matrix = direct3d->get_world_matrix();
				dx::XMMATRIX view_matrix = camera->get_view_matrix();
				dx::XMMATRIX projection_matrix = direct3d->get_projection_matrix();

				std::cout << pos.x << "," << pos.y << "\n";
				dx::XMFLOAT3 near_pos{ static_cast<float>(pos.x), static_cast<float>(pos.y), 0.0f };
				dx::XMFLOAT3 far_pos{ static_cast<float>(pos.x), static_cast<float>(pos.y), 1000.0f };
				dx::XMVECTOR near_pos_vec = dx::XMLoadFloat3(&near_pos);
				dx::XMVECTOR far_pos_vec = dx::XMLoadFloat3(&far_pos);

				near_pos_vec = dx::XMVector3Unproject(
					near_pos_vec,
					0, 0,
					1280, 720,
					0.0f, 100.0f,
					projection_matrix,
					view_matrix,
					world_matrix);

				far_pos_vec = dx::XMVector3Unproject(
					far_pos_vec,
					0, 0,
					1280, 720,
					0.0f, 100.0f,
					projection_matrix,
					view_matrix,
					world_matrix);

				auto direction = dx::XMVectorSubtract(near_pos_vec, far_pos_vec);

				std::cout << dx::XMVectorGetX(near_pos_vec) << ", " << dx::XMVectorGetY(near_pos_vec) << ", " << dx::XMVectorGetZ(near_pos_vec) << "\n";
			}
		}
	}
	return false;
}

bool Graphics::draw()
{
	direct3d->begin(1.0f, 1.0f, 0.95f, 1.0f);

	camera->draw();
	dx::XMMATRIX world_matrix = direct3d->get_world_matrix();
	dx::XMMATRIX view_matrix =	camera->get_view_matrix();
	dx::XMMATRIX projection_matrix = direct3d->get_projection_matrix();
	
	direct3d->set_wireframe(false);
	model->update_instance_buffer(direct3d->get_device(), direct3d->get_device_context(), instances);
	model->render(direct3d->get_device_context());
	
	bool result = colour_shader->render(direct3d->get_device_context(), model->get_index_count(), model->get_vertex_count(), model->get_instance_count(), world_matrix, view_matrix, projection_matrix);
	if (!result)
	{
		return false;
	}

	world_matrix = dx::XMMatrixIdentity();
	
	direct3d->set_wireframe(true);
	world->draw(direct3d->get_device_context());
	result = colour_shader->render(direct3d->get_device_context(), world->get_index_count(), world->get_index_count(), 1, world_matrix, view_matrix, projection_matrix);
	if (!result)
	{
		return false;
	}

	direct3d->end();

	return false;
}

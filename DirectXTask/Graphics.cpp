#include "Graphics.hpp"

//STD
#include <iostream>
#include <iomanip>

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
	camera->set_pos(32.0f, 20.0f, 0.0f);
	camera->set_rot(30, 0, 0);

	try
	{
		sphere = std::make_unique<Model>(direct3d->get_device(), "Sphere.txt");
		cube = std::make_unique<Model>(direct3d->get_device(), "Cube2.txt");
	}
	catch (...)
	{
		MessageBoxW(window, L"Could not init Model", L"ERROR", MB_OK);
	}

	float deg2rad = 0.0174533f;

	sphere_instances.push_back({
		{ 30.0f - (std::cosf(time) * 4 + 4), 1.0f, 30.0f - (std::sinf(time) * 4 + 4) },
		{ 0.0f, 0.0f, 0.0f },
		{ 1.00f, 1.0f, 1.0f},
		{ 0, 1.0f, 0.8f, 1.0f }
	});

	sphere_instances.push_back({
		{ 32.0f, 1.0f, 32.0f },
		{ std::sinf(time) * 180.0f * deg2rad, std::sinf(time) * 180.0f * deg2rad, std::sinf(time) * 180.0f * deg2rad },
		{ 1.00f, 1.0f, 1.0f},
		{ 1.0f, 0.8f, 0.0f, 1.0f }
	});

	sphere_instances.push_back({
		{ 40.0f, 1.0f + std::cosf(time) * 3 + 3, 40.0f },
		{ 0.0f, std::sinf(time) * 180.0f * deg2rad, 0.0f },
		{ 1.00f, 1.0f, 1.0f},
		{ 1.0f, 0.4f, 0.4f, 1.0f }
	});

	sphere_instances.push_back({
		{ 44.0f, 1.0f, 44.0f },
		{ 0.0f, 0.0f, std::cosf(time) * 180.0f * deg2rad },
		{ 1.00f, 1.0f, 1.0f},
		{ 0.0f, 0.8f, 1.0f, 1.0f }
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
		for (int i = 0; i < 100; ++i)
		{
			float rand_x = (rand() % 51200) / 100.0f;
			rand_x -= 256 - 64;
			float rand_y = (rand() % 51200) / 100.0f + 32.0f;
			float rand_z = (rand() % 51200) / 100.0f;
			rand_z -= 256 - 64;

			sphere_instances.push_back({
				{ rand_x, rand_y, rand_z},
				{ 0.0f, 0.0f, 0.0f },
				{ 1.0f, 1.0f, 1.0f },
				{ (std::sin(time * 10.0f) + 1.0f) / 2.0f, 0.2f, 0.5f, 1.0f }
			});
		}

		std::cout.imbue(std::locale(""));
		std::cout << "\nInstances: " << sphere_instances.size() << "\n";
		int instances_mb = static_cast<int>((sphere_instances.size() * sizeof(Model::Instance)) / 1024.0f / 1024.0f);
		std::cout << "Instances Memory: " << instances_mb << "MB\n";
		std::cout << "Vertices: " << sphere_instances.size() * sphere->get_vertex_count() << "\n";
	}

	static float rot = std::cosf(time) * 180.0f * deg2rad;
	rot += 1.0f * dt;
	for (auto& instance : sphere_instances)
	{
		instance.rotation.x = rot;
		instance.rotation.y = rot;
		instance.rotation.z = rot;
	}

	for (auto& instance : cube_instances)
	{
		instance.rotation.x = rot;
		instance.rotation.y = rot;
		instance.rotation.z = rot;
	}

	POINT pos;
	if (GetCursorPos(&pos))
	{
		if (ScreenToClient(window, &pos))
		{
			if ((GetKeyState(VK_LBUTTON) & 0x100) != 0)
			{
				camera->draw();
				dx::XMMATRIX world_matrix = direct3d->get_world_matrix();
				dx::XMMATRIX view_matrix = camera->get_view_matrix();
				dx::XMMATRIX projection_matrix = direct3d->get_projection_matrix();

				float rel_pos_x = pos.x / 1280.0f;
				float rel_pos_y = pos.y / 720.0f;
				rel_pos_x *= 2;
				rel_pos_x -= 1.0f;
				rel_pos_y *= 2;
				rel_pos_y -= 1.0f;
				//std::cout << pos.x << "," << pos.y << "\n";
				//std::cout << rel_pos_x << "," << rel_pos_y << "\n";
				dx::XMFLOAT3 near_pos{ static_cast<float>(pos.x), static_cast<float>(pos.y), 0.0f };
				dx::XMFLOAT3 far_pos{ static_cast<float>(pos.x), static_cast<float>(pos.y), 0.995f };
				dx::XMVECTOR near_pos_vec = dx::XMLoadFloat3(&near_pos);
				dx::XMVECTOR far_pos_vec = dx::XMLoadFloat3(&far_pos);

				near_pos_vec = dx::XMVector3Unproject(
					near_pos_vec,
					0, 0,
					1280.0f, 720.0f,
					0.0f, 1.0f,
					projection_matrix,
					view_matrix,
					world_matrix);

				far_pos_vec = dx::XMVector3Unproject(
					far_pos_vec,
					0, 0,
					1280.0f, 720.0f,
					0.0f, 1.0f,
					projection_matrix,
					view_matrix,
					world_matrix);
				
				auto direction = dx::XMVectorSubtract(near_pos_vec, far_pos_vec);

				//std::cout << dx::XMVectorGetX(near_pos_vec) << ", " << dx::XMVectorGetY(near_pos_vec) << ", " << dx::XMVectorGetZ(near_pos_vec) << "\n";
				
				dx::XMFLOAT3 pos;
				dx::XMStoreFloat3(&pos, far_pos_vec);
				cube_instances.push_back({
					{pos.x, pos.y, pos.z},
					{0, 0, 0},
					{1.0f, 1.0f, 1.0f},
					{0.2f, 0.2f, 0.2f, 1.0f},
				});
			}
		}
	}
	return false;
}

bool Graphics::draw()
{
	float deg2rad = 0.0174533f;
	float rot = std::cosf(time) * 18.0f * deg2rad;
	direct3d->begin(0.9f, 0.9f, 0.85f, 1.0f);
	dx::XMFLOAT3 light_direction = { -0.3f, rot, rot };
	dx::XMFLOAT4 diffuse_colour = { 1.0f, 1.0f, 1.0f, 1.0f };

	camera->draw();
	dx::XMMATRIX world_matrix = direct3d->get_world_matrix();
	dx::XMMATRIX view_matrix =	camera->get_view_matrix();
	dx::XMMATRIX projection_matrix = direct3d->get_projection_matrix();
	
	direct3d->set_wireframe(false);
	sphere->update_instance_buffer(direct3d->get_device(), direct3d->get_device_context(), sphere_instances);
	sphere->render(direct3d->get_device_context());
	
	bool result = colour_shader->render(direct3d->get_device_context(), sphere->get_index_count(), sphere->get_vertex_count(), sphere->get_instance_count(), world_matrix, view_matrix, projection_matrix, light_direction, diffuse_colour);
	if (!result)
	{
		return false;
	}

	direct3d->set_wireframe(false);
	cube->update_instance_buffer(direct3d->get_device(), direct3d->get_device_context(), cube_instances);
	cube->render(direct3d->get_device_context());

	result = colour_shader->render(direct3d->get_device_context(), cube->get_index_count(), cube->get_vertex_count(), cube->get_instance_count(), world_matrix, view_matrix, projection_matrix, light_direction, diffuse_colour);
	if (!result)
	{
		return false;
	}

	world_matrix = dx::XMMatrixIdentity();
	
	direct3d->set_wireframe(false);
	world->draw(direct3d->get_device_context());
	result = colour_shader->render(direct3d->get_device_context(), world->get_index_count(), world->get_index_count(), 1, world_matrix, view_matrix, projection_matrix, light_direction, diffuse_colour);
	if (!result)
	{
		return false;
	}

	direct3d->end();

	return false;
}

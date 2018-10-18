#include "System.hpp"

#include <iostream>
#include <thread>

System::System()
{
	WindowSettings ws{ "RTS", 1280, 720 };
	window = std::make_unique<Window>(ws);
	input = std::make_unique<Input>();
	graphics = std::make_unique<Graphics>(window.get());
	world = std::make_unique<World>(graphics->direct3d->get_device());

	camera = std::make_unique<Camera>();
	camera->set_pos(32.0f, 20.0f, 0.0f);
	camera->set_rot(30, 0, 0);

	try
	{
		sphere = std::make_unique<Model>(graphics->direct3d->get_device(), "Sphere.txt");
		cube = std::make_unique<Model>(graphics->direct3d->get_device(), "Cube2.txt");
	}
	catch (...)
	{
		MessageBoxW(window->window_handle, L"Could not init Model", L"ERROR", MB_OK);
	}

	sphere_instances.push_back({
		{ 30.0f, 1.0f, 30.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 1.00f, 1.0f, 1.0f},
		{ 0, 1.0f, 0.8f, 1.0f }
		});

	sphere_instances.push_back({
		{ 32.0f, 1.0f, 32.0f },
		{ 0, 0, 0 },
		{ 1.00f, 1.0f, 1.0f},
		{ 1.0f, 0.8f, 0.0f, 1.0f }
		});

	entities.push_back(std::make_unique<Entity>());
	entities[0]->instance = {
		{ 40.0f, 0, 40.0f },
		{ 0.0f, 0, 0.0f },
		{ 0.5f, 0.5f, 0.5f},
		{ 1.0f, 0.4f, 0.4f, 1.0f }
	};

	entities.push_back(std::make_unique<Entity>());
	entities[1]->instance = {
		{ 44.0f, 1.0f, 44.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 0.5f, 0.5f, 0.5f},
		{ 0.0f, 0.8f, 1.0f, 1.0f }
	};

	for (auto& ent : entities)
	{
		ent->world = world.get();
	}

	pathfinding = std::make_unique<Pathfinding>(world.get());

	for (auto& cell : pathfinding->all_cells)
	{
		cube_instances.push_back({
			{ cell.x, cell.y + 0.1f, cell.z },
			{ 0.0f, 0.0f, 0.0f },
			{ 0.1f, 0.1f, 0.1f},
			{0.5f, 1.0f, 0.5f, 1.0f }
			});

		if (cell.y > 9)
		{
			cube_instances.back().colour.x = 1.0f;
			cube_instances.back().colour.y = 0.5f;
			cell.walkable = false;
		}

		if (cell.y < 2)
		{
			cube_instances.back().colour.y = 0.5f;
			cube_instances.back().colour.z = 1.0f;
			cell.walkable = false;
		}
	}
}

void System::run()
{
	while (window->open && !quit)
	{
		dt = frame_timer.getElapsedTime<Timer::seconds_float>();
		timeSinceLastFPSPrint += dt;
		if (timeSinceLastFPSPrint > 1.0f)
		{
			std::cout << "FPS: " << static_cast<int>(1.0f / dt) << "\n";
			timeSinceLastFPSPrint = 0;
		}
		frame_timer.restart();

		event();
		update();
		draw();
	}
}

void System::event()
{
	Event e;
	while (window->poll(e))
	{
		std::visit([&](auto&& arg)
		{
			using T = std::decay_t<decltype(arg)>;
			if constexpr(std::is_same_v<T, EventMouseMoved>)
			{
				//std::cout << "moved mouse to " << arg.x << ", " << arg.y << "\n";
			}
			else if constexpr (std::is_same_v<T, EventWindowClosed>)
			{
				window->open = false;
			}
			else if constexpr (std::is_same_v<T, EventKeyPress>)
			{
				if (arg.key_down)
				{
					input->key_down(arg.key_code);
				}
				else
				{
					input->key_up(arg.key_code);
				}
			}
		}, e);
	}
}

void System::update()
{
	if (input->is_key_down(VK_ESCAPE))
	{
		quit = true;
	}

	time += dt;
	camera->update(input.get(), dt);

	float deg2rad = 0.0174533f;

	if (input->is_key_down('C'))
	{
		for (int i = 0; i < 1000; ++i)
		{
			float rand_x = (rand() % 51200) / 100.0f;
			float rand_y = (rand() % 5120) / 100.0f + 64.0f;
			float rand_z = (rand() % 51200) / 100.0f;

			cube_instances.push_back({
				{ rand_x, rand_y, rand_z},
				{ 0.0f, 0.0f, 0.0f },
				{ 1.0f, 1.0f, 1.0f },
				{ (std::sin(time * 10.0f) + 1.0f) / 2.0f, 0.2f, 0.5f, 1.0f }
				});
		}
	}

	POINT pos;
	if (GetCursorPos(&pos) && ScreenToClient(window->window_handle, &pos))
	{
		dx::XMMATRIX world_matrix = graphics->direct3d->get_world_matrix();
		dx::XMMATRIX view_matrix = camera->get_view_matrix();
		dx::XMMATRIX projection_matrix = graphics->direct3d->get_projection_matrix();

		dx::XMFLOAT3 near_pos{ static_cast<float>(pos.x), static_cast<float>(pos.y), 0.0f };
		dx::XMFLOAT3 far_pos{ static_cast<float>(pos.x), static_cast<float>(pos.y), 1.0f };
		dx::XMVECTOR near_pos_vec = dx::XMLoadFloat3(&near_pos);
		dx::XMVECTOR far_pos_vec = dx::XMLoadFloat3(&far_pos);

		float width = window->width;
		float height = window->height;

		near_pos_vec = dx::XMVector3Unproject(
			near_pos_vec,
			0, 0,
			width, height,
			0.0f, 1.0f,
			projection_matrix,
			view_matrix,
			world_matrix);

		far_pos_vec = dx::XMVector3Unproject(
			far_pos_vec,
			0, 0,
			width, height,
			0.0f, 1.0f,
			projection_matrix,
			view_matrix,
			world_matrix);

		if ((GetKeyState(VK_LBUTTON) & 0x100) != 0)
		{
			auto intersection_pos = world->triangle_intersection(near_pos_vec, far_pos_vec);

			if (!isnan(intersection_pos.x))
			{
				//entities[0]->goal_pos.push_back(intersection_pos);

				std::cout << intersection_pos.x << ", " << intersection_pos.y << ", " << intersection_pos.z << "\n";
				auto cell = pathfinding->find_closest_cell(intersection_pos.x, intersection_pos.y, intersection_pos.z);
				std::cout << std::boolalpha << cell.walkable << " " << cell.grid_x << ", " << cell.grid_y << "\n";
				std::cout << cell.x << ", " << cell.y << ", " << cell.z << "\n\n";

				if (cell.valid && cell.walkable)
				{
					entities[0]->goal_pos.push_back({cell.x, cell.y, cell.z});
				}
			}
		}

		if ((GetKeyState(VK_RBUTTON) & 0x100) != 0)
		{
			auto intersection_pos = world->triangle_intersection(near_pos_vec, far_pos_vec);

			if (!isnan(intersection_pos.x))
			{
				//entities[1]->goal_pos.push_back(intersection_pos);

				std::cout << intersection_pos.x << ", " << intersection_pos.y << ", " << intersection_pos.z << "\n";
				auto cell = pathfinding->find_closest_cell(intersection_pos.x, intersection_pos.y, intersection_pos.z);
				std::cout << std::boolalpha << cell.walkable << " " << cell.grid_x << ", " << cell.grid_y << "\n";
				std::cout << cell.x << ", " << cell.y << ", " << cell.z << "\n\n";

				if (cell.valid && cell.walkable)
				{
					entities[1]->goal_pos.push_back({ cell.x, cell.y, cell.z });
				}
			}
		}
	}

	entity_instances.clear();

	for (auto& ent : entities)
	{
		ent->update(dt);
		entity_instances.push_back(ent->instance);
	}
}

void System::draw()
{
	graphics->begin(0.2f, 0.2f, 0.2f, camera.get());

	sphere->update_instance_buffer(graphics->direct3d->get_device(), graphics->direct3d->get_device_context(), sphere_instances);
	graphics->draw(sphere->vertex_buffer.val, sphere->index_buffer.val, sphere->instance_buffer.val, sphere->vertex_count, sphere->index_count, sphere->instance_count);

	sphere->update_instance_buffer(graphics->direct3d->get_device(), graphics->direct3d->get_device_context(), entity_instances);
	graphics->draw(sphere->vertex_buffer.val, sphere->index_buffer.val, sphere->instance_buffer.val, sphere->vertex_count, sphere->index_count, sphere->instance_count);

	cube->update_instance_buffer(graphics->direct3d->get_device(), graphics->direct3d->get_device_context(), cube_instances);
	graphics->draw(cube->vertex_buffer.val, cube->index_buffer.val, cube->instance_buffer.val, cube->vertex_count, cube->index_count, cube->instance_count);

	graphics->draw(world->vertex_buffer, world->index_buffer, world->instance_buffer, world->vertex_count, world->index_count, world->instance_count);

	graphics->end();
}
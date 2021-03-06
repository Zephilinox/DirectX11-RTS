#include "System.hpp"

#include <iostream>
#include <thread>

System::System()
{
	WindowSettings ws{ "RTS", 1280, 720, false, false };
	window = std::make_unique<Window>(ws);
	input = std::make_unique<Input>();
	graphics = std::make_unique<Graphics>(window.get());
	world = std::make_unique<World>(graphics->direct3d->get_device());

	camera = std::make_unique<Camera>();
	camera->set_pos(32.0f, 20.0f, 0.0f);
	camera->set_rot(30, 0, 0);

	try
	{
		sphere = std::make_unique<Model>(graphics->direct3d->get_device(), graphics->direct3d->get_device_context(), "Sphere.txt");
		cube = std::make_unique<Model>(graphics->direct3d->get_device(), graphics->direct3d->get_device_context(), "Cube2.txt");
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

	entities.push_back(std::make_unique<Entity>());
	entities[2]->instance = {
		{ 44.0f, 1.0f, 44.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 0.5f, 0.5f, 0.5f},
		{ 0.0f, 1.0f, 0.2f, 1.0f }
	};

	entities.push_back(std::make_unique<Entity>());
	entities[3]->instance = {
		{ 44.0f, 1.0f, 44.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 0.5f, 0.5f, 0.5f},
		{ 1.0f, 0.7f, 0.0f, 1.0f }
	};

	entities.push_back(std::make_unique<Entity>());
	entities[4]->instance = {
		{ 44.0f, 1.0f, 44.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 0.5f, 0.5f, 0.5f},
		{ 1.0f, 0.6f, 0.6f, 1.0f }
	};

	for (auto& ent : entities)
	{
		ent->world = world.get();
	}

	pathfinding = std::make_unique<Pathfinding>(world.get());

	for (auto& cell : pathfinding->all_cells)
	{
		if (cell.y >= 2 && cell.y <= 9)
		{
			continue;
		}

		cube_instances.push_back({
			{ cell.x, cell.y + 0.1f, cell.z },
			{ 0.0f, 0.0f, 0.0f },
			{ 0.3f, 0.2f, 0.3f},
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

	group_1 = std::make_unique<Group>(input.get(), pathfinding.get());
	group_2 = std::make_unique<Group>(input.get(), pathfinding.get());
	group_3 = std::make_unique<Group>(input.get(), pathfinding.get());
	group_4 = std::make_unique<Group>(input.get(), pathfinding.get());
	group_5 = std::make_unique<Group>(input.get(), pathfinding.get());
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
			else if constexpr (std::is_same_v<T, EventMouseKeyPress>)
			{
				if (arg.key_down)
				{
					input->mouse_key_down(arg.mouse_key_code);
				}
				else
				{
					input->mouse_key_up(arg.mouse_key_code);
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

	for (int i = 0; i < 5; ++i)
	{
		if (input->is_key_down('0' + i))
		{
			active_group = i;
			std::cout << "Activated " << i << "\n";
		}
	}

	time += dt;
	camera->update(input.get(), dt);
	
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

		dx::XMFLOAT3 far_pos2;
		dx::XMStoreFloat3(&far_pos2, far_pos_vec);

		dx::XMFLOAT3 near_pos2;
		dx::XMStoreFloat3(&near_pos2, near_pos_vec);


		if (input->is_mouse_key_down(0))
		{
			/*cube_instances.push_back({
				{ far_pos2.x, far_pos2.y, far_pos2.z},
				{ 0.0f, 0.0f, 0.0f },
				{ 100.0f, 100.0f, 100.0f },
				{ 0.2f, 0.2f, 0.5f, 1.0f }
				});

			cube_instances.push_back({
				{ near_pos2.x, near_pos2.y, near_pos2.z},
				{ 0.0f, 0.0f, 0.0f },
				{ 1.0f, 1.0f, 1.0f },
				{ 1.0f, 0.2f, 0.5f, 1.0f }
				});*/

			auto intersection_pos = world->triangle_intersection(near_pos_vec, far_pos_vec);

			if (!isnan(intersection_pos.x))
			{
				std::cout << intersection_pos.x << ", " << intersection_pos.y << ", " << intersection_pos.z << "\n";
				Cell& cell = *pathfinding->find_closest_cell(intersection_pos.x, intersection_pos.y, intersection_pos.z);
				std::cout << std::boolalpha << cell.walkable << " " << cell.grid_x << ", " << cell.grid_y << "\n";
				std::cout << cell.x << ", " << cell.y << ", " << cell.z << "\n\n";

				if (cell.valid && cell.walkable)
				{
					entities[active_group]->goal_pos.clear();
					entities[active_group]->goal_pos_index = 0;
					entities[active_group]->goal_pos.push_back({ entities[active_group]->instance.position});
					entities[active_group]->goal_pos.push_back({cell.x, cell.y, cell.z});
				}
			}
		}

		static bool rmb_down = false;
		//A*
		if (!rmb_down && input->is_mouse_key_down(1))
		{
			rmb_down = true;
			auto intersection_pos = world->triangle_intersection(near_pos_vec, far_pos_vec);

			if (!isnan(intersection_pos.x))
			{
				std::cout << intersection_pos.x << ", " << intersection_pos.y << ", " << intersection_pos.z << "\n";
				Cell& cell = *pathfinding->find_closest_cell(intersection_pos.x, intersection_pos.y, intersection_pos.z);
				std::cout << std::boolalpha << cell.walkable << " " << cell.grid_x << ", " << cell.grid_y << "\n";
				std::cout << cell.x << ", " << cell.y << ", " << cell.z << "\n\n";

				if (cell.valid && cell.walkable)
				{
					auto cells = pathfinding->find_path(entities[active_group]->instance.position, intersection_pos);
					std::cout << "Path of " << cells.size() << " found\n";
					if (cells.size())
					{
						entities[active_group]->goal_pos.clear();
						entities[active_group]->goal_pos_index = 0;
						entities[active_group]->goal_pos.push_back(entities[active_group]->instance.position);
						for (auto& cell : cells)
						{
							entities[active_group]->goal_pos.push_back({ cell.x, cell.y, cell.z });
						}
						entities[active_group]->goal_pos.push_back(intersection_pos);
					}
				}
			}
		}
		else
		{
			rmb_down = input->is_mouse_key_down(1);
		}
	}
	else
	{
		throw;
	}

	entity_instances.clear();
	pathfinding_instances.clear();
	for (auto& ent : entities)
	{
		ent->update(dt);
		entity_instances.push_back(ent->instance);

		for (auto& pos : ent->goal_pos)
		{
			pathfinding_instances.push_back(
			{
				pos,
				{0, 0, 0},
				{0.15f, 2.0f, 0.15f},
				ent->instance.colour
			});
		}
	}
}

void System::draw()
{
	dx::XMFLOAT3 light_direction = { -0.3f, 0, 0 };
	dx::XMFLOAT4 diffuse_colour = { 5.0f, 5.0f, 5.0f, 1.0f };
	dx::XMFLOAT4 diffuse_colour_world = { 1.0f, 1.0f, 1.0f, 1.0f };

	graphics->begin(0.2f, 0.2f, 0.2f, camera.get());

	sphere->update_instance_buffer(graphics->direct3d->get_device(), graphics->direct3d->get_device_context(), sphere_instances);
	graphics->draw(sphere->vertex_buffer.val, sphere->index_buffer.val, sphere->instance_buffer.val, sphere->vertex_count, sphere->index_count, sphere->instance_count, sphere->get_texture(), light_direction, diffuse_colour);

	sphere->update_instance_buffer(graphics->direct3d->get_device(), graphics->direct3d->get_device_context(), entity_instances);
	graphics->draw(sphere->vertex_buffer.val, sphere->index_buffer.val, sphere->instance_buffer.val, sphere->vertex_count, sphere->index_count, sphere->instance_count, sphere->get_texture(), light_direction, diffuse_colour);

	cube->update_instance_buffer(graphics->direct3d->get_device(), graphics->direct3d->get_device_context(), cube_instances);
	graphics->draw(cube->vertex_buffer.val, cube->index_buffer.val, cube->instance_buffer.val, cube->vertex_count, cube->index_count, cube->instance_count, cube->get_texture(), light_direction, diffuse_colour);

	cube->update_instance_buffer(graphics->direct3d->get_device(), graphics->direct3d->get_device_context(), pathfinding_instances);
	graphics->draw(cube->vertex_buffer.val, cube->index_buffer.val, cube->instance_buffer.val, cube->vertex_count, cube->index_count, cube->instance_count, cube->get_texture(), light_direction, diffuse_colour);

	graphics->draw(world->vertex_buffer, world->index_buffer, world->instance_buffer, world->vertex_count, world->index_count, world->instance_count, sphere->get_texture(), light_direction, diffuse_colour_world);

	graphics->end();
}
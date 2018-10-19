#include "Pathfinding.hpp"

//STD
#include <iostream>
#include <algorithm>
#include <experimental/vector>

//SELF
#include "World.hpp"

Pathfinding::Pathfinding(World* world)
	: world(world)
{
	int fails = 0;

	for (int h = 0; h < grid_height; ++h)
	{
		for (int w = 0; w < grid_width; ++w)
		{
			dx::XMFLOAT3 from{static_cast<float>(w) * cell_size, 1000.0f, static_cast<float>(h) * cell_size };
			dx::XMFLOAT3 to{ static_cast<float>(w) * cell_size, -1000.0f, static_cast<float>(h) * cell_size };
			dx::XMVECTOR from_vec = dx::XMLoadFloat3(&from);
			dx::XMVECTOR to_vec = dx::XMLoadFloat3(&to);
			auto intersection_pos = world->triangle_intersection(from_vec, to_vec);

			if (!isnan(intersection_pos.x))
			{
				int id = all_cells.size();
				all_cells.push_back({ id, w, h, intersection_pos.x, intersection_pos.y, intersection_pos.z, true, true });
			}
			else
			{
				from = { static_cast<float>(w) * cell_size, 50.0f, static_cast<float>(h) * cell_size };
				to = { static_cast<float>(w) * cell_size, -50.0f, static_cast<float>(h) * cell_size };
				from_vec = dx::XMLoadFloat3(&from);
				to_vec = dx::XMLoadFloat3(&to);
				intersection_pos = world->triangle_intersection(from_vec, to_vec);

				if (!isnan(intersection_pos.x))
				{
					int id = all_cells.size();
					all_cells.push_back({ id, w, h, intersection_pos.x, intersection_pos.y, intersection_pos.z, true, true });
				}
				else
				{
					int id = all_cells.size();
					all_cells.push_back({ id, w, h, -1, -1, -1, false, false });
					fails++;
				}
			}
		}

		std::cout << (static_cast<float>(h) / static_cast<float>(grid_height)) * 100.0f << "%\n";
	}

	std::cout << "A* Cells: " << fails << " failed.\n";
}

Cell* Pathfinding::find_closest_cell(float x, float y, float z)
{
	static Cell c;

	int grid_x = std::round(x / cell_size);
	int grid_z = std::round(z / cell_size);
	if (grid_x >= 0 && grid_x < grid_width)
	{
		if (grid_z >= 0 && grid_z < grid_height)
		{
			return &all_cells[grid_width * grid_z + grid_x];
		}
	}

	return &c;
}

std::vector<Cell> Pathfinding::find_path(dx::XMFLOAT3 start_pos, dx::XMFLOAT3 goal_pos)
{
	Cell* start_cell = find_closest_cell(start_pos.x, start_pos.y, start_pos.z);
	Cell* goal_cell = find_closest_cell(goal_pos.x, goal_pos.y, goal_pos.z);

	for (auto& cell : all_cells)
	{
		cell.in_open_list = false;
		cell.in_closed_list = false;
		cell.parent_id = -1;
	}

	open_cells.clear();
	closed_cells.clear();
	
	if (start_cell->valid && start_cell->walkable && goal_cell->valid && goal_cell->walkable)
	{
		start_cell->in_closed_list = false;
		start_cell->in_open_list = true;
		open_cells.push_back(start_cell);

		while (!open_cells.empty())
		{
			Cell* current_cell = open_cells.front();

			for (int i = 1; i < open_cells.size(); ++i)
			{
				int f = open_cells[i]->g_cost + open_cells[i]->h_cost;
				int cur_f = current_cell->g_cost + current_cell->h_cost;

				if (f < cur_f || (f == cur_f && open_cells[i]->h_cost < current_cell->h_cost))
				{
					current_cell = open_cells[i];
				}
			}
			current_cell->in_closed_list = true;
			current_cell->in_open_list = false;
			std::experimental::erase_if(open_cells, [&](auto cell)
			{
				return cell->grid_id == current_cell->grid_id;
			});

			closed_cells.push_back(current_cell);

			if (current_cell->grid_id == goal_cell->grid_id)
			{
				return get_final_path(start_cell, goal_cell);
			}

			for (auto& cell : get_neighbouring_cells(current_cell))
			{
				if (!cell->walkable || cell->in_closed_list)
				{
					continue;
				}

				int move_cost = current_cell->g_cost + heuristic_distance(*current_cell, *cell);

				if (move_cost < cell->g_cost || !cell->in_open_list)
				{
					cell->g_cost = move_cost;
					cell->h_cost = heuristic_distance(*cell, *goal_cell);
					cell->parent_id = current_cell->grid_id;

					if (!cell->in_open_list)
					{
						cell->in_open_list = true;
						open_cells.push_back(cell);
					}
				}
			}
		}
	}

	for (auto& cell : all_cells)
	{
		cell.in_open_list = false;
		cell.in_closed_list = false;
		cell.parent_id = -1;
	}

	open_cells.clear();
	closed_cells.clear();

	return {};
}

std::vector<Cell*> Pathfinding::get_neighbouring_cells(Cell* cell)
{
	std::vector<Cell*> neighbouring_cells;

	int x = cell->grid_x + 1;
	int y = cell->grid_y;
	if (valid_grid_pos(x, y))
	{
		neighbouring_cells.push_back(&all_cells[grid_width * y + x]);
	}

	x = cell->grid_x - 1;
	y = cell->grid_y;
	if (valid_grid_pos(x, y))
	{
		neighbouring_cells.push_back(&all_cells[grid_width * y + x]);
	}

	x = cell->grid_x;
	y = cell->grid_y + 1;
	if (valid_grid_pos(x, y))
	{
		neighbouring_cells.push_back(&all_cells[grid_width * y + x]);
	}

	x = cell->grid_x;
	y = cell->grid_y - 1;
	if (valid_grid_pos(x, y))
	{
		neighbouring_cells.push_back(&all_cells[grid_width * y + x]);
	}

	x = cell->grid_x + 1;
	y = cell->grid_y + 1;
	if (valid_grid_pos(x, y))
	{
		neighbouring_cells.push_back(&all_cells[grid_width * y + x]);
	}

	x = cell->grid_x + 1;
	y = cell->grid_y - 1;
	if (valid_grid_pos(x, y))
	{
		neighbouring_cells.push_back(&all_cells[grid_width * y + x]);
	}

	x = cell->grid_x - 1;
	y = cell->grid_y - 1;
	if (valid_grid_pos(x, y))
	{
		neighbouring_cells.push_back(&all_cells[grid_width * y + x]);
	}

	x = cell->grid_x - 1;
	y = cell->grid_y + 1;
	if (valid_grid_pos(x, y))
	{
		neighbouring_cells.push_back(&all_cells[grid_width * y + x]);
	}
	
	return neighbouring_cells;
}

bool Pathfinding::valid_grid_pos(int x, int y)
{
	return x >= 0 && x < grid_width && y >= 0 && y < grid_height;
}

int Pathfinding::heuristic_distance(Cell start, Cell end)
{
	//octile distance
	int x = std::abs(start.grid_x - end.grid_x);
	int z = std::abs(start.grid_y - end.grid_y);
	float standard_cost = 100.0f;
	float diagonal_cost = 140.0f;
	float total_standard_cost = standard_cost * (x + z);
	float diagonal_savings = diagonal_cost - (2.0f * standard_cost);
	float total_diagonal_cost = diagonal_savings * min(x, z);

	int y = std::abs(start.y - end.y);
	float height_penalty = standard_cost * 2 * y * (1.0f / cell_size);

	return std::round(total_standard_cost + total_diagonal_cost + height_penalty);
}

std::vector<Cell> Pathfinding::get_final_path(Cell* start, Cell* goal)
{
	std::vector<Cell> path;
	Cell* current_cell = goal;

	while (current_cell->grid_id != start->grid_id)
	{
		path.push_back(*current_cell);
		current_cell = &all_cells[current_cell->parent_id];
	}

	std::reverse(path.begin(), path.end());

	for (auto& cell : all_cells)
	{
		cell.in_open_list = false;
		cell.in_closed_list = false;
		cell.parent_id = -1;
	}

	open_cells.clear();
	closed_cells.clear();

	return path;
}

std::vector<Cell> Pathfinding::simplify_path(std::vector<Cell> path)
{
	//lazy mans jump point search haha (not really)
	std::vector<Cell> waypoints;

	dx::XMFLOAT2 direction_old{ 0, 0};

	for (int i = 0; i < path.size() - 1; ++i)
	{
		dx::XMFLOAT2 direction{
			static_cast<float>(path[i + 1].grid_x - path[i].grid_x),
			static_cast<float>(path[i + 1].grid_y - path[i].grid_y)
		};

		if (direction.x != direction_old.x ||
			direction.y != direction_old.y)
		{
			waypoints.push_back(path[i]);
		}
		direction_old = direction;
	}

	waypoints.push_back(path[path.size() - 1]);

	return waypoints;
}

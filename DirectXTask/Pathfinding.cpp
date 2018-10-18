#include "Pathfinding.hpp"

//STD
#include <iostream>
#include <algorithm>

//SELF
#include "World.hpp"

Pathfinding::Pathfinding(World* world)
	: world(world)
{
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
				all_cells.push_back({ w, h, intersection_pos.x, intersection_pos.y, intersection_pos.z, true, true });
			}
			else
			{
				all_cells.push_back({ w, h, -1, -1, -1, true, false });
			}
		}

		std::cout << (static_cast<float>(h) / static_cast<float>(grid_height)) * 100.0f << "%\n";
	}

	std::cout << "A* Cells: " << all_cells.size() << " out of " << grid_width*grid_height << "\n";
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
			open_cells.erase(open_cells.begin());
			closed_cells.push_back(current_cell);

			if (current_cell->grid_x == goal_cell->grid_x &&
				current_cell->grid_y == goal_cell->grid_y)
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
					cell->parent_id = grid_width * current_cell->grid_y + current_cell->grid_x;

					if (!cell->in_open_list)
					{
						open_cells.push_back(cell);
					}
				}
			}
		}

	}
	else
	{
		for (auto& cell : all_cells)
		{
			cell.in_open_list = false;
			cell.in_closed_list = false;
			cell.parent_id = -1;
		}

		open_cells.clear();
		closed_cells.clear();
	}

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
	int x = std::abs(start.x - end.x);
	int y = std::abs(start.y - end.y);

	return x + y;
}

std::vector<Cell> Pathfinding::get_final_path(Cell* start, Cell* goal)
{
	std::vector<Cell> path;
	Cell* current_cell = goal;

	while (current_cell->grid_x != start->grid_x &&
		current_cell->grid_y != start->grid_y)
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

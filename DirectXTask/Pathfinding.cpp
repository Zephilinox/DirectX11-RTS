#include "Pathfinding.hpp"

//STD
#include <iostream>

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

Cell Pathfinding::find_closest_cell(float x, float y, float z)
{
	int grid_x = std::round(x / cell_size);
	int grid_z = std::round(z / cell_size);
	if (grid_x > 0 && grid_x < grid_width)
	{
		if (grid_z > 0 && grid_z < grid_height)
		{
			return all_cells[grid_width * grid_z + grid_x];
		}
	}

	return Cell();
}

std::vector<Cell> Pathfinding::find_path(Cell start, Cell end)
{
	return std::vector<Cell>();
}

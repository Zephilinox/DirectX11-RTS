#include "Pathfinding.hpp"

//STD
#include <iostream>

//SELF
#include "World.hpp"

Pathfinding::Pathfinding(World* world)
	: world(world)
{
	float resolution = 0.5f;
	int height = 32 * (1.0 / resolution) + 1;
	int width = 32 * (1.0f / resolution) + 1;

	for (int h = 0; h < height; ++h)
	{
		for (int w = 0; w < width; ++w)
		{
			dx::XMFLOAT3 from{static_cast<float>(w) * resolution, 1000.0f, static_cast<float>(h) * resolution };
			dx::XMFLOAT3 to{ static_cast<float>(w) * resolution, -1000.0f, static_cast<float>(h) * resolution };
			dx::XMVECTOR from_vec = dx::XMLoadFloat3(&from);
			dx::XMVECTOR to_vec = dx::XMLoadFloat3(&to);
			auto intersection_pos = world->triangle_intersection(from_vec, to_vec);
			if (!isnan(intersection_pos.x))
			{
				all_cells.push_back({ intersection_pos.x, intersection_pos.y, intersection_pos.z, true });
			}
		}

		std::cout << (static_cast<float>(h) / static_cast<float>(height)) * 100.0f << "%\n";
	}

	std::cout << "A* Cells: " << all_cells.size() << " out of " << width*height << "\n";
}

#include "Pathfinding.hpp"

//STD
#include <iostream>

//SELF
#include "World.hpp"

Pathfinding::Pathfinding(World* world)
	: world(world)
{
	int height = 64;
	int width = 64;

	for (int h = 0; h < height; ++h)
	{
		for (int w = 0; w < width; ++w)
		{
			dx::XMFLOAT3 from{static_cast<float>(w), 1000.0f, static_cast<float>(h)};
			dx::XMFLOAT3 to{ static_cast<float>(w), -1000.0f, static_cast<float>(h)};
			dx::XMVECTOR from_vec = dx::XMLoadFloat3(&from);
			dx::XMVECTOR to_vec = dx::XMLoadFloat3(&to);
			auto intersection_pos = world->triangle_intersection(from_vec, to_vec);
			if (!isnan(intersection_pos.x))
			{
				all_cells.push_back({ intersection_pos.x, intersection_pos.y, intersection_pos.z, true });
			}
		}
	}

	std::cout << "A* Cells: " << all_cells.size() << " out of " << width*height << "\n";
}

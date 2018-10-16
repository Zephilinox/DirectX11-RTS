#pragma once

//STD
#include <vector>

class World;

struct Cell
{
	float x;
	float y;
	float z;
	bool walkable;
};

class Pathfinding
{
public:
	Pathfinding(World* world);

	std::vector<Cell> all_cells;

private:
	std::vector<Cell> open_cells;
	std::vector<Cell> visited_cells;
	World* world;
};
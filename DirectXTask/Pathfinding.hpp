#pragma once

//STD
#include <vector>

class World;

struct Cell
{
	int grid_x = 0;
	int grid_y = 0;

	float x = 0;
	float y = 0;
	float z = 0;

	bool walkable = true;
	bool valid = false;

	int g_cost = 0;
	int h_cost = 0;

	Cell* parent = nullptr;
};

class Pathfinding
{
public:
	Pathfinding(World* world);

	Cell find_closest_cell(float x, float y, float z);
	
	std::vector<Cell> find_path(Cell start, Cell end);

	std::vector<Cell> all_cells;

	static constexpr float worldgrid_width = 160;
	static constexpr float worldgrid_height = 160;
	static constexpr float cell_size = 1.0f;
	static constexpr float grid_width = worldgrid_width * (1.0f / cell_size) + 1;
	static constexpr float grid_height = worldgrid_height * (1.0f / cell_size) + 1;

private:
	std::vector<Cell> open_cells;
	std::vector<Cell> visited_cells;
	World* world;
};
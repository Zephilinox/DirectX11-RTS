#pragma once

//STD
#include <vector>

//LIBS
#include <directxmath.h>
namespace dx = DirectX;

class World;

struct Cell
{
	int grid_id = 0;
	int grid_x = 0;
	int grid_y = 0;

	float x = 0;
	float y = 0;
	float z = 0;

	bool walkable = false;
	bool valid = false;

	int g_cost = 0;
	int h_cost = 0;

	int parent_id = -1;
	bool in_closed_list = false;
	bool in_open_list = false;
};

class Pathfinding
{
public:
	Pathfinding(World* world);

	Cell* find_closest_cell(float x, float y, float z);
	
	std::vector<Cell> find_path(dx::XMFLOAT3 start_pos, dx::XMFLOAT3 goal_pos);
	std::vector<Cell*> get_neighbouring_cells(Cell* cell);
	bool valid_grid_pos(int x, int y);
	int heuristic_distance(Cell start, Cell end);

	std::vector<Cell> all_cells;

	static constexpr float worldgrid_width = 160;
	static constexpr float worldgrid_height = 160;
	static constexpr float cell_size = 1.0f;
	static constexpr float grid_width = worldgrid_width * (1.0f / cell_size) + 1;
	static constexpr float grid_height = worldgrid_height * (1.0f / cell_size) + 1;

private:
	std::vector<Cell> get_final_path(Cell* start, Cell* goal);
	std::vector<Cell> simplify_path(std::vector<Cell> cells);

	std::vector<Cell*> open_cells;
	std::vector<Cell*> closed_cells;
	World* world;
};
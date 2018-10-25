#pragma once

//STD
#include <vector>

//SELF
#include "Entity.hpp"

class Input;
class Pathfinding;
class World;

class Group
{
public:
	Group(Input* input, Pathfinding* pathfinding);
	void update(float dt);

	std::vector<Entity> entities;

private:
	Input* input;
	Pathfinding* pathfinding;
	World* world;
};
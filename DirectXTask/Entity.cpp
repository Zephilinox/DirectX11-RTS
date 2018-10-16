#include "Entity.hpp"

//STD
#include <iostream>

void Entity::update(float dt)
{
	if (goal_pos.empty())
	{
		return;
	}

	bool at_pos = true;
	
	if (instance.position.x + 0.1f < goal_pos.front().x)
	{
		at_pos = false;
		instance.position.x += 100.0f * dt;
	}

	if (instance.position.x - 0.1f > goal_pos.front().x)
	{
		at_pos = false;
		instance.position.x -= 100.0f * dt;
	}
	
	if (instance.position.y + 0.1f < goal_pos.front().y + 1)
	{
		at_pos = false;
		instance.position.y += 100.0f * dt;
	}

	if (instance.position.y - 0.1f > goal_pos.front().y + 1)
	{
		at_pos = false;
		instance.position.y -= 100.0f * dt;
	}

	if (instance.position.z + 0.1f < goal_pos.front().z)
	{
		at_pos = false;
		instance.position.z += 100.0f * dt;
	}

	if (instance.position.z - 0.1f > goal_pos.front().z)
	{
		at_pos = false;
		instance.position.z -= 100.0f * dt;
	}

	if (at_pos)
	{
		goal_pos.pop_front();
	}
}

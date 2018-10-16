#include "Entity.hpp"

//STD
#include <iostream>

void Entity::update(float dt)
{
	if (goal_pos.empty())
	{
		return;
	}

	dx::XMFLOAT3 dir = {0, 0, 0};

	if (instance.position.x + 0.1f < goal_pos.front().x)
	{
		dir.x += 1;
	}

	if (instance.position.x - 0.1f > goal_pos.front().x)
	{
		dir.x -= 1;
	}
	
	if (instance.position.y + 0.1f < goal_pos.front().y + 1)
	{
		dir.y += 1;
	}

	if (instance.position.y - 0.1f > goal_pos.front().y + 1)
	{
		dir.y -= 1;
	}

	if (instance.position.z + 0.1f < goal_pos.front().z)
	{
		dir.z += 1;
	}

	if (instance.position.z - 0.1f > goal_pos.front().z)
	{
		dir.z -= 1;
	}

	float magnitude = sqrt((dir.x * dir.x) + (dir.y * dir.y) + (dir.z * dir.z));
	if (fabsf(magnitude) > 0.01f)
	{
		dir.x /= magnitude;
		dir.y /= magnitude;
		dir.z /= magnitude;

		float speed = 50.0f;

		instance.position.x += dir.x * speed * dt;
		instance.position.y += dir.y * speed * dt;
		instance.position.z += dir.z * speed * dt;
	}
	else
	{
		//not moving, so we are at our goal
		goal_pos.pop_front();
	}
}

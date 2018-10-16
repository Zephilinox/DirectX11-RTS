#include "Entity.hpp"

//STD
#include <iostream>

//SELF
#include "World.hpp"

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
	
	if (instance.position.y + 1.0f < goal_pos.front().y + 1)
	{
		dir.y += 1;
	}

	if (instance.position.y - 1.0f > goal_pos.front().y + 1)
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
		instance.position.z += dir.z * speed * dt;
		if (world)
		{
			dx::XMFLOAT3 from{ instance.position.x, 1000, instance.position.z };
			dx::XMFLOAT3 to{ instance.position.x, -1000, instance.position.z };
			auto from_vec = dx::XMLoadFloat3(&from);
			auto to_vec = dx::XMLoadFloat3(&to);
			auto pos = world->triangle_intersection(from_vec, to_vec);
			if (!isnan(pos.y))
			{
				instance.position.y = pos.y + 1;
			}
			else
			{
				instance.position.y += dir.y * speed * dt;
			}
		}
		else
		{
			instance.position.y += dir.y * speed * dt;
		}
	}
	else
	{
		//not moving, so we are at our goal
		goal_pos.pop_front();
	}
}

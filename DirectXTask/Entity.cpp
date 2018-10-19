#include "Entity.hpp"

//STD
#include <iostream>

//SELF
#include "World.hpp"

void Entity::update(float dt)
{
	if (goal_pos.empty())
	{
		goal_pos_index = 0;
		return;
	}

	if (goal_pos_index + 1 < goal_pos.size())
	{
		dx::XMVECTOR from_vec = dx::XMLoadFloat3(&goal_pos[goal_pos_index]);
		dx::XMVECTOR to_vec = dx::XMLoadFloat3(&goal_pos[goal_pos_index + 1]);
		dx::XMVECTOR direction = dx::XMVectorSubtract(from_vec, to_vec);
		
		float distance = dx::XMVectorGetX(dx::XMVector3Length(direction));
		if (percent_along < 0.99)
		{
			percent_along += (10.0f * dt) / distance;
			instance.position = lerp(goal_pos[goal_pos_index], goal_pos[goal_pos_index + 1], percent_along);

			if (world)
			{
				dx::XMFLOAT3 from{ instance.position.x, 1000, instance.position.z };
				dx::XMFLOAT3 to{ instance.position.x, -1000, instance.position.z };
				auto from_vec = dx::XMLoadFloat3(&from);
				auto to_vec = dx::XMLoadFloat3(&to);
				auto pos = world->triangle_intersection(from_vec, to_vec);
				if (!isnan(pos.y))
				{
					instance.position.y = pos.y;
				}
			}
		}
		else
		{
			percent_along = 0;
			goal_pos_index++;
		}
	}

	/*dx::XMFLOAT3 dir = {0, 0, 0};

	if (instance.position.x + 0.1f < goal_pos[goal_pos_index].x)
	{
		dir.x += 1;
	}

	if (instance.position.x - 0.1f > goal_pos[goal_pos_index].x)
	{
		dir.x -= 1;
	}
	
	if (instance.position.y + 1.0f < goal_pos[goal_pos_index].y)
	{
		dir.y += 1;
	}

	if (instance.position.y - 1.0f > goal_pos[goal_pos_index].y)
	{
		dir.y -= 1;
	}

	if (instance.position.z + 0.1f < goal_pos[goal_pos_index].z)
	{
		dir.z += 1;
	}

	if (instance.position.z - 0.1f > goal_pos[goal_pos_index].z)
	{
		dir.z -= 1;
	}

	float magnitude = sqrt((dir.x * dir.x) + (dir.y * dir.y) + (dir.z * dir.z));
	if (fabsf(magnitude) > 0.01f)
	{
		dir.x /= magnitude;
		dir.y /= magnitude;
		dir.z /= magnitude;

		float speed = 10.0f;

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
				instance.position.y = pos.y;
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
		//goal_pos.pop_front();
		goal_pos_index++;

		if (goal_pos_index >= goal_pos.size())
		{
			goal_pos_index = 0;
			goal_pos.clear();
		}
	}*/
}

dx::XMFLOAT3 Entity::lerp(dx::XMFLOAT3 from, dx::XMFLOAT3 to, float t)
{
	dx::XMFLOAT3 pos;
	pos.x = (from.x * (1.0f - t)) + (to.x * t);
	pos.y = (from.y * (1.0f - t)) + (to.y * t);
	pos.z = (from.z * (1.0f - t)) + (to.z * t);
	return pos;
}

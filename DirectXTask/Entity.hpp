#pragma once

//STD
#include <deque>

//SELF
#include "Model.hpp"

class World;

class Entity
{
public:

	void update(float dt);

	ColourShader::Instance instance;

	std::deque<dx::XMFLOAT3> goal_pos;
	int goal_pos_index = 0;

	World* world = nullptr;

private:

};
#pragma once

//STD
#include <deque>

//SELF
#include "Model.hpp"

class Entity
{
public:

	void update(float dt);

	Model::Instance instance;

	std::deque<dx::XMFLOAT3> goal_pos;

private:

};
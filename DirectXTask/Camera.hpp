#pragma once

//LIBS
#include <directxmath.h>
namespace dx = DirectX;

//SELF
#include "Input.hpp"

class Camera
{
public:
	void set_pos(float x, float y, float z);
	void set_rot(float x, float y, float z);

	dx::XMFLOAT3 get_pos();
	dx::XMFLOAT3 get_rot();

	void update(Input* input, float dt);
	void draw();
	dx::XMMATRIX get_view_matrix();

private:
	float pos_x = 0.0f;
	float pos_y = 0.0f;
	float pos_z = 0.0f;

	float rot_x = 0.0f;
	float rot_y = 0.0f;
	float rot_z = 0.0f;

	float pos_speed = 20.0f;
	float rot_speed = 50.0f;

	dx::XMMATRIX view_matrix;
};
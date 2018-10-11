#include "Camera.hpp"

//STD
#include <cmath>
#include <iostream>

//LIBS
#include <d3d11.h>

void Camera::set_pos(float x, float y, float z)
{
	pos_x = x;
	pos_y = y;
	pos_z = z;
}

void Camera::set_rot(float x, float y, float z)
{
	rot_x = x;
	rot_y = y;
	rot_z = z;
}

dx::XMFLOAT3 Camera::get_pos()
{
	return dx::XMFLOAT3(pos_x, pos_y, pos_z);
}

dx::XMFLOAT3 Camera::get_rot()
{
	return dx::XMFLOAT3(rot_x, rot_y, rot_z);
}

void Camera::update(Input* input, float dt)
{
	constexpr float deg2rad = 0.0174532925f;
	if (input->is_key_down(VK_SHIFT))
	{
		pos_speed = 100.0f;
	}
	else
	{
		pos_speed = 20.0f;
	}

	if (input->is_key_down(VK_LEFT))
	{
		rot_y -= rot_speed * dt;

		while (rot_y < 0.0f)
		{
			rot_y += 360.0f;
		}
	}
	else if (input->is_key_down(VK_RIGHT))
	{
		rot_y += rot_speed * dt;

		while (rot_y > 360.0f)
		{
			rot_y -= 360.0f;
		}
	}

	if (input->is_key_down('W'))
	{
		pos_x += std::sinf(rot_y * deg2rad) * pos_speed * dt;
		pos_z += std::cosf(rot_y * deg2rad) * pos_speed * dt;
	}
	else if (input->is_key_down('S'))
	{
		pos_x -= std::sinf(rot_y * deg2rad) * pos_speed * dt;
		pos_z -= std::cosf(rot_y * deg2rad) * pos_speed * dt;
	}

	if (input->is_key_down(VK_UP))
	{
		rot_x -= rot_speed * dt;

		if (rot_x > 90.0f)
		{
			rot_x = 90.0f;
		}
	}
	else if (input->is_key_down(VK_DOWN))
	{
		rot_x += rot_speed * dt;

		if (rot_x < -90.0f)
		{
			rot_x = -90.0f;
		}
	}

	if (input->is_key_down('Q'))
	{
		pos_y += pos_speed * dt;
	}
	else if (input->is_key_down('E'))
	{
		pos_y -= pos_speed * dt;
	}

	if (input->is_key_down('A'))
	{
		pos_x -= std::cosf(rot_y * deg2rad) * pos_speed * dt;
		pos_z += std::sinf(rot_y * deg2rad) * pos_speed * dt;
	}
	else if (input->is_key_down('D'))
	{
		pos_x += std::cosf(rot_y * deg2rad) * pos_speed * dt;
		pos_z -= std::sinf(rot_y * deg2rad) * pos_speed * dt;
	}

	//std::cout << "Pos = {" << pos_x << ", " << pos_y << ", " << pos_z << "\n";
	//std::cout << "Rot = {" << rot_x << ", " << rot_y << ", " << rot_z << "\n";
}

void Camera::draw()
{
	dx::XMFLOAT3 up{0.0f, 1.0f, 0.0f};
	dx::XMVECTOR up_vector = XMLoadFloat3(&up);
	dx::XMFLOAT3 position{pos_x, pos_y, pos_z};
	dx::XMVECTOR position_vector = XMLoadFloat3(&position);
	
	constexpr float deg2rad = 0.0174532925f;
	dx::XMFLOAT3 lookat = {0.0f, 0.0f, 1.0f};
	dx::XMVECTOR lookat_vector = XMLoadFloat3(&lookat);

	float pitch = rot_x * deg2rad;
	float yaw = rot_y * deg2rad;
	float roll = rot_z * deg2rad;

	dx::XMMATRIX rotation_matrix = dx::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
	lookat_vector = dx::XMVector3TransformCoord(lookat_vector, rotation_matrix);
	up_vector = dx::XMVector3TransformCoord(up_vector, rotation_matrix);

	lookat_vector = dx::XMVectorAdd(position_vector, lookat_vector);

	view_matrix = dx::XMMatrixLookAtLH(position_vector, lookat_vector, up_vector);
}

dx::XMMATRIX Camera::get_view_matrix()
{
	return view_matrix;
}

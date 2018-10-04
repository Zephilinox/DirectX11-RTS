#include "Camera.hpp"

//STD
#include <cmath>

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

void Camera::render()
{
	dx::XMFLOAT3 up{0.0f, 1.0f, 0.0f};
	dx::XMVECTOR up_vector = XMLoadFloat3(&up);
	dx::XMFLOAT3 position{pos_x, pos_y, pos_z};
	dx::XMVECTOR position_vector = XMLoadFloat3(&position);

	if (GetAsyncKeyState(VK_NUMPAD4))
	{
		rot_y -= 0.5f;
	}

	if (GetAsyncKeyState(VK_NUMPAD6))
	{
		rot_y += 0.5f;
	}

	constexpr float deg2rad = 0.0174532925f;
	dx::XMFLOAT3 lookat
	{
		std::sinf(rot_y * deg2rad) + pos_x,
		pos_y,
		std::cosf(rot_y * deg2rad) + pos_z
	};

	dx::XMVECTOR lookat_vector = XMLoadFloat3(&lookat);

	view_matrix = dx::XMMatrixLookAtLH(position_vector, lookat_vector, up_vector);
}

dx::XMMATRIX Camera::get_view_matrix()
{
	return view_matrix;
}

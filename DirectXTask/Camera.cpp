#include "Camera.hpp"

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
	dx::XMFLOAT3 lookat{0.0f, 0.0f, 1.0f};
	dx::XMVECTOR lookat_vector = XMLoadFloat3(&lookat);

	//todo deg2rad
	float pitch = pos_x * 0.0174532925f;
	float yaw = pos_y * 0.0174532925f;
	float roll = pos_z * 0.0174532925f;

	dx::XMMATRIX rotationMatrix = dx::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	lookat_vector = XMVector3TransformCoord(lookat_vector, rotationMatrix);
	up_vector = XMVector3TransformCoord(up_vector, rotationMatrix);

	lookat_vector = dx::XMVectorAdd(position_vector, lookat_vector);
	view_matrix = dx::XMMatrixLookAtLH(position_vector, lookat_vector, up_vector);
}

void Camera::get_view_matrix(dx::XMMATRIX& matrix)
{
	matrix = view_matrix;
}

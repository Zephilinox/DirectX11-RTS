#include "Graphics.hpp"

//STD
#include <iostream>

Graphics::Graphics(int width, int height, HWND window)
{
	this->window = window;

	try
	{
		direct3d = std::make_unique<Direct3D>(width, height, vsync, window, fullscreen, screen_depth, screen_near);
	}
	catch (...)
	{
		MessageBox(window, "Could not init Direct3D", "ERROR", MB_OK);
		throw;
	}

	camera = std::make_unique<Camera>();
	camera->set_pos(0.0f, 5.0f, -5.0f);
	camera->set_rot(40, 0, 0);

	try
	{
		model = std::make_unique<Model>(direct3d->get_device());
	}
	catch (...)
	{
		MessageBoxW(window, L"Could not init Model", L"ERROR", MB_OK);
	}

	try
	{
		world = std::make_unique<World>(direct3d->get_device());
	}
	catch (...)
	{
		MessageBoxW(window, L"Could not init World", L"ERROR", MB_OK);
	}

	colour_shader = std::make_unique<ColourShader>(direct3d->get_device(), window);
}

bool Graphics::update(Input* input, float dt)
{
	time += dt;
	camera->update(input, dt);

	POINT pos;
	if (GetCursorPos(&pos))
	{
		if (ScreenToClient(window, &pos))
		{
			if ((GetKeyState(VK_LBUTTON) & 0x100) != 0)
			{
				dx::XMMATRIX world_matrix = direct3d->get_world_matrix();
				dx::XMMATRIX view_matrix = camera->get_view_matrix();
				dx::XMMATRIX projection_matrix = direct3d->get_projection_matrix();

				std::cout << pos.x << "," << pos.y << "\n";
				dx::XMFLOAT3 near_pos{ static_cast<float>(pos.x), static_cast<float>(pos.y), 0.0f };
				dx::XMFLOAT3 far_pos{ static_cast<float>(pos.x), static_cast<float>(pos.y), 1000.0f };
				dx::XMVECTOR near_pos_vec = dx::XMLoadFloat3(&near_pos);
				dx::XMVECTOR far_pos_vec = dx::XMLoadFloat3(&far_pos);

				near_pos_vec = dx::XMVector3Unproject(
					near_pos_vec,
					0, 0,
					1280, 720,
					0.0f, 100.0f,
					projection_matrix,
					view_matrix,
					world_matrix);

				far_pos_vec = dx::XMVector3Unproject(
					far_pos_vec,
					0, 0,
					1280, 720,
					0.0f, 100.0f,
					projection_matrix,
					view_matrix,
					world_matrix);

				auto direction = dx::XMVectorSubtract(near_pos_vec, far_pos_vec);

				std::cout << dx::XMVectorGetX(near_pos_vec) << ", " << dx::XMVectorGetY(near_pos_vec) << ", " << dx::XMVectorGetZ(near_pos_vec) << "\n";
			}
		}
	}
	return false;
}

bool Graphics::draw()
{
	direct3d->begin(0.0f, 0.0f, 0.0f, 1.0f);

	camera->draw();
	dx::XMMATRIX world_matrix = direct3d->get_world_matrix();
	dx::XMMATRIX view_matrix =	camera->get_view_matrix();
	dx::XMMATRIX projection_matrix = direct3d->get_projection_matrix();

	std::vector<Model::Instance> instances = 
	{
		{
			{ 30.0f - (std::cosf(time) * 4 + 4), 1.0f, 30.0f - (std::sinf(time) * 4 + 4) },
			{ (std::sinf(time) + 1) / 2, 1.0f, (std::cosf(time) + 1) / 2, 1.0f},
			{ 0.0f, 0.0f, 0.0f}
		},
		{
			{ 32.0f, 1.0f, 32.0f },
			{ std::cosf(time), std::cosf(time), 1.0f, 1.0f },
			{ 0.0f, 0.0f, 0.0f }
		},
		{
			{ 40.0f, 1.0f + std::cosf(time) * 3 + 3, 40.0f },
			{ 1.0f, 0.0f, std::sin(time), 1.0f },
			{ 0.0f, 0.0f, 0.0f }
		},
		{
			{ 44.0f + std::sinf(time) * 12, 1.0f, 44.0f },
			{ 1.0f, std::cosf(time), 0.0f, 1.0f },
			{ 0.0f, 0.0f, 0.0f }
		}
	};

	ID3D11RasterizerState* Fill;
	D3D11_RASTERIZER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));
	desc.FillMode = D3D11_FILL_SOLID;
	desc.CullMode = D3D11_CULL_BACK;
	direct3d->get_device()->CreateRasterizerState(&desc, &Fill);
	direct3d->get_device_context()->RSSetState(Fill);
	model->update_instances(direct3d->get_device_context(), instances);

	model->render(direct3d->get_device_context());

	Fill->Release();

	bool result = colour_shader->render(direct3d->get_device_context(), model->get_index_count(), model->get_vertex_count(), model->get_instance_count(), world_matrix, view_matrix, projection_matrix);
	if (!result)
	{
		return false;
	}

	world_matrix = dx::XMMatrixIdentity();

	ID3D11RasterizerState* Wireframe;
	ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));
	desc.FillMode = D3D11_FILL_WIREFRAME;
	desc.CullMode = D3D11_CULL_NONE;
	direct3d->get_device()->CreateRasterizerState(&desc, &Wireframe);
	direct3d->get_device_context()->RSSetState(Wireframe);

	world->draw(direct3d->get_device_context());
	result = colour_shader->render(direct3d->get_device_context(), world->get_index_count(), world->get_index_count(), 1, world_matrix, view_matrix, projection_matrix);
	if (!result)
	{
		return false;
	}
	Wireframe->Release();

	direct3d->end();

	return false;
}

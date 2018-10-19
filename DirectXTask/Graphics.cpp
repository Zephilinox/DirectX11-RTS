#include "Graphics.hpp"

//STD
#include <iostream>
#include <iomanip>

//SELF
#include "Window.hpp"

Graphics::Graphics(Window* window)
{
	this->window_handle = window->window_handle;

	try
	{
		direct3d = std::make_unique<Direct3D>(window->width, window->height, window->vsync, window_handle, window->fullscreen, screen_depth, screen_near);
	}
	catch (...)
	{
		MessageBox(window_handle, "Could not init Direct3D", "ERROR", MB_OK);
		throw;
	}

	colour_shader = std::make_unique<ColourShader>(direct3d->get_device(), window_handle);
}

void Graphics::begin(float r, float g, float b, Camera* camera)
{
	direct3d->begin(r, g, b, 1.0f);
	camera->draw();
	view_matrix = camera->get_view_matrix();
}

void Graphics::draw(ID3D11Buffer* vertex_buffer, ID3D11Buffer* index_buffer, ID3D11Buffer* instance_buffer, int vertex_count, int index_count, int instance_count, ID3D11ShaderResourceView* texture)
{
	if (texture == nullptr)
	{
		throw;
	}

	dx::XMFLOAT3 light_direction = { -0.3f, 0, 0 };
	dx::XMFLOAT4 diffuse_colour = { 1.0f, 1.0f, 1.0f, 1.0f };
	dx::XMMATRIX world_matrix = direct3d->get_world_matrix();
	dx::XMMATRIX projection_matrix = direct3d->get_projection_matrix();

	direct3d->set_wireframe(false);

	unsigned int strides[2] = { sizeof(ColourShader::Vertex), sizeof(ColourShader::Instance) };
	unsigned int offsets[2] = { 0, 0 };

	ID3D11Buffer* buffer_pointers[2] = { vertex_buffer, instance_buffer };

	auto device_context = direct3d->get_device_context();
	device_context->IASetVertexBuffers(0, 2, buffer_pointers, strides, offsets);
	device_context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);
	device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	HRESULT result = colour_shader->render(direct3d->get_device_context(), vertex_count, index_count, instance_count, world_matrix, view_matrix, projection_matrix, light_direction, diffuse_colour, texture);
}

void Graphics::end()
{
	direct3d->end();
}

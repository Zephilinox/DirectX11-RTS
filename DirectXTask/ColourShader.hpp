#pragma once

//STD
#include <fstream>

//LIBS
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
namespace dx = DirectX;

//SELF
#include "Direct3D.hpp"

class ColourShader
{
public:
	ColourShader(ID3D11Device* device, HWND window);

	bool render(ID3D11DeviceContext* device_context, int index_count, int vertex_count, int instance_count, dx::XMMATRIX world, dx::XMMATRIX view, dx::XMMATRIX projection, dx::XMFLOAT3 light_direction, dx::XMFLOAT4 diffuse_colour);

private:
	struct MatrixBuffer
	{
		dx::XMMATRIX world;
		dx::XMMATRIX view;
		dx::XMMATRIX projection;
	};

	struct LightBuffer
	{
		dx::XMFLOAT4 diffuse_colour;
		dx::XMFLOAT3 light_direction;
		float padding;
	};

	bool init_shader(ID3D11Device* device, HWND window, LPCWSTR vs_filename, LPCWSTR ps_filename);
	void print_shader_error(ID3D10Blob* error_msg, HWND window, LPCWSTR filename);

	bool set_shader_params(ID3D11DeviceContext* device_context, dx::XMMATRIX world, dx::XMMATRIX view, dx::XMMATRIX projection, dx::XMFLOAT3 light_direction, dx::XMFLOAT4 diffuse_colour);
	void render_shader(ID3D11DeviceContext* device_context, int index_count, int vertex_count, int instance_count);

	D3DRAII<ID3D11VertexShader> vertex_shader;
	D3DRAII<ID3D11PixelShader> pixel_shader;
	D3DRAII<ID3D11InputLayout> layout;
	D3DRAII<ID3D11Buffer> matrix_buffer;
	D3DRAII<ID3D11Buffer> light_buffer;
};
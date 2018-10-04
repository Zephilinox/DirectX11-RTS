#pragma once

//STD
#include <fstream>

//LIBS
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>

namespace dx = DirectX;

class ColourShader
{
public:
	ColourShader(ID3D11Device* device, HWND window);
	~ColourShader();

	bool render(ID3D11DeviceContext* device_context, int index_count, int vertex_count, int instance_count, dx::XMMATRIX world, dx::XMMATRIX view, dx::XMMATRIX projection);
	bool render(ID3D11DeviceContext* device_context, int index_count, dx::XMMATRIX world, dx::XMMATRIX view, dx::XMMATRIX projection);

private:
	struct MatrixBuffer
	{
		dx::XMMATRIX world;
		dx::XMMATRIX view;
		dx::XMMATRIX projection;
	};

	bool init_shader(ID3D11Device* device, HWND window, LPCWSTR vs_filename, LPCWSTR ps_filename);
	void stop_shader();
	void print_shader_error(ID3D10Blob* error_msg, HWND window, LPCWSTR filename);

	bool set_shader_params(ID3D11DeviceContext* device_context, dx::XMMATRIX world, dx::XMMATRIX view, dx::XMMATRIX projection);
	void render_shader(ID3D11DeviceContext* device_context, int index_count, int vertex_count, int instance_count);
	void render_shader(ID3D11DeviceContext* device_context, int index_count);

	ID3D11VertexShader* vertex_shader = nullptr;
	ID3D11PixelShader* pixel_shader = nullptr;
	ID3D11InputLayout* layout = nullptr;
	ID3D11Buffer* matrix_buffer = nullptr;
};
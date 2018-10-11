#pragma once

//TODO: Add to project settings
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

//SELF
#include <memory>
#include <array>

//LIBS
#include <d3d11.h>
#include <DirectXMath.h>

namespace dx = DirectX;

template <typename T>
struct D3DRAII
{
	~D3DRAII()
	{
		if (val)
		{
			val->Release();
		}
	}

	T* val = nullptr;
};

class Direct3D
{
public:
	Direct3D(int width, int height, bool vsync, HWND window, bool fullscreen, float screen_depth, float screen_near);
	~Direct3D();

	void begin(float r, float g, float b, float a);
	void end();

	void set_wireframe(bool enabled);

	ID3D11Device* get_device();
	ID3D11DeviceContext* get_device_context();

	dx::XMMATRIX get_projection_matrix();
	dx::XMMATRIX get_world_matrix();
	dx::XMMATRIX get_ortho_matrix();

	void get_gpu_info(char* name, int& memory);

private:
	DXGI_SWAP_CHAIN_DESC make_swapchain_desc(HWND window, int width, int height, unsigned int numerator, unsigned int denominator);
	D3D11_TEXTURE2D_DESC make_depth_buffer_desc(int width, int height);
	std::tuple<unsigned int, unsigned int> get_display_mode_data(int width, int height, IDXGIAdapter* adapte);
	bool create_device_and_swapchain(HWND window, int width, int height, int numerator, int denominator);
	bool create_render_target_view();
	bool create_texture2d(int width, int height);
	bool create_depth_stencil_view();
	bool create_depth_stencil_state();
	bool create_rasterizer_state();
	void create_viewport(float width, float height);
	void create_matrices(float width, float height, float screen_near, float screen_depth);

	bool vsync;
	bool fullscreen;
	bool wireframe;

	int gpu_memory;
	std::array<char, 128> gpu_description;
	D3DRAII<IDXGISwapChain> swapchain;
	D3DRAII<ID3D11Device> device;
	D3DRAII<ID3D11DeviceContext> device_context;
	D3DRAII<ID3D11RenderTargetView> render_target_view;
	D3DRAII<ID3D11Texture2D> depth_stencil_buffer;
	D3DRAII<ID3D11DepthStencilState> depth_stencil_state;
	D3DRAII<ID3D11DepthStencilView> depth_stencil_view;
	D3DRAII<ID3D11RasterizerState> raster_state_fill;
	D3DRAII<ID3D11RasterizerState> raster_state_wireframe;
	dx::XMMATRIX projection_matrix;
	dx::XMMATRIX world_matrix;
	dx::XMMATRIX ortho_matrix;
};
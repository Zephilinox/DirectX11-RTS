#pragma once

//TODO: Add to project settings
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

//SELF
#include <memory>

//LIBS
#include <d3d11.h>
#include <DirectXMath.h>

namespace dx = DirectX;

class Direct3D
{
public:
	bool init(int width, int height, bool vsync, HWND window, bool fullscreen, float screen_depth, float screen_near);
	void stop();

	void begin(float, float, float, float);
	void end();

	ID3D11Device* get_device();
	ID3D11DeviceContext* get_device_context();

	void get_projection_matrix(dx::XMMATRIX& matrix);
	void get_world_matrix(dx::XMMATRIX& matrix);
	void get_ortho_matrix(dx::XMMATRIX& matrix);

	void get_gpu_info(char* name, int& memory);

private:
	DXGI_SWAP_CHAIN_DESC make_swapchain_desc(HWND window, int width, int height, unsigned int numerator, unsigned int denominator);
	D3D11_TEXTURE2D_DESC make_depth_buffer_desc(int width, int height);

	bool vsync;
	bool fullscreen;

	int gpu_memory;
	char gpu_description[128];
	IDXGISwapChain* swapchain = 0;
	ID3D11Device* device = 0;
	ID3D11DeviceContext* device_context = 0;
	ID3D11RenderTargetView* render_target_view = 0;
	ID3D11Texture2D* depth_stencil_buffer = 0;
	ID3D11DepthStencilState* depth_stencil_state = 0;
	ID3D11DepthStencilView* depth_stencil_view = 0;
	ID3D11RasterizerState* raster_state = 0;
	dx::XMMATRIX projection_matrix;
	dx::XMMATRIX world_matrix;
	dx::XMMATRIX ortho_matrix;
};
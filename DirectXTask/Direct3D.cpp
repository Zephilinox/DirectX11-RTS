#include "Direct3D.hpp"

//STD
#include <iostream>
#include <array>
#include <vector>

//SELF
#include "Model.hpp"

Direct3D::Direct3D(int width, int height, bool vsync, HWND window, bool fullscreen, float screen_depth, float screen_near)
{
	this->vsync = vsync;
	this->fullscreen = fullscreen;

	HRESULT result;

	D3DRAII<IDXGIAdapter> adapter;

	{
		D3DRAII<IDXGIFactory> factory;
		result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory.val);
		if (FAILED(result))
		{
			throw;
		}

		result = factory.val->EnumAdapters(0, &adapter.val);
		if (FAILED(result))
		{
			throw;
		}
	}

	auto [numerator, denominator] = get_display_mode_data(width, height, adapter.val);

	DXGI_ADAPTER_DESC adapter_desc;
	result = adapter.val->GetDesc(&adapter_desc);
	if (FAILED(result))
	{
		throw;
	}

	gpu_memory = static_cast<int>(adapter_desc.DedicatedVideoMemory / 1024 / 1024);

	size_t string_length;
	int error = wcstombs_s(&string_length, gpu_description.data(), gpu_description.size(), adapter_desc.Description, gpu_description.size());
	if (error != 0)
	{
		throw;
	}

	create_device_and_swapchain(window, width, height, numerator, denominator);
	create_render_target_view();
	create_texture2d(width, height);

	create_depth_stencil_state();
	device_context.val->OMSetDepthStencilState(depth_stencil_state.val, 1);

	create_depth_stencil_view();
	device_context.val->OMSetRenderTargets(1, &render_target_view.val, depth_stencil_view.val);

	create_rasterizer_state();
	device_context.val->RSSetState(raster_state.val);

	create_viewport(static_cast<float>(width), static_cast<float>(height));
	create_matrices(static_cast<float>(width), static_cast<float>(height), screen_near, screen_depth);
}

Direct3D::~Direct3D()
{
	if (swapchain.val)
	{
		swapchain.val->SetFullscreenState(false, 0);
	}
}

void Direct3D::begin(float r, float g, float b, float a)
{
	float colour[4] = { r, g, b, a };
	device_context.val->ClearRenderTargetView(render_target_view.val, colour);
	device_context.val->ClearDepthStencilView(depth_stencil_view.val, D3D11_CLEAR_DEPTH, 1.0f, 0);

	static dx::XMMATRIX origin_proj_matrix = projection_matrix;

	if (GetAsyncKeyState(VK_SPACE))
	{
		projection_matrix = ortho_matrix;
	}
	else
	{
		projection_matrix = origin_proj_matrix;
	}	
}

void Direct3D::end()
{
	if (vsync)
	{
		swapchain.val->Present(1, 0);
	}
	else
	{
		swapchain.val->Present(0, 0);
	}

	world_matrix = dx::XMMatrixIdentity();
}

ID3D11Device* Direct3D::get_device()
{
	return device.val;
}

ID3D11DeviceContext* Direct3D::get_device_context()
{
	return device_context.val;
}

dx::XMMATRIX Direct3D::get_projection_matrix()
{
	return projection_matrix;
}

dx::XMMATRIX Direct3D::get_world_matrix()
{
	return world_matrix;
}

dx::XMMATRIX Direct3D::get_ortho_matrix()
{
	return ortho_matrix;
}

void Direct3D::get_gpu_info(char* name, int& memory)
{
	strcpy_s(name, gpu_description.size(), gpu_description.data());
	memory = gpu_memory;
}

DXGI_SWAP_CHAIN_DESC Direct3D::make_swapchain_desc(HWND window, int width, int height, unsigned int numerator, unsigned int denominator)
{
	DXGI_SWAP_CHAIN_DESC swapchain_desc;
	ZeroMemory(&swapchain_desc, sizeof(swapchain_desc));
	swapchain_desc.BufferCount = 1;
	swapchain_desc.BufferDesc.Width = width;
	swapchain_desc.BufferDesc.Height = height;
	swapchain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	if (vsync)
	{
		swapchain_desc.BufferDesc.RefreshRate.Numerator = numerator;
		swapchain_desc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapchain_desc.BufferDesc.RefreshRate.Numerator = 0;
		swapchain_desc.BufferDesc.RefreshRate.Denominator = 1;
	}

	swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchain_desc.OutputWindow = window;

	swapchain_desc.SampleDesc.Count = 1;
	swapchain_desc.SampleDesc.Quality = 0;

	swapchain_desc.Windowed = !fullscreen;

	swapchain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapchain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapchain_desc.Flags = 0;

	return swapchain_desc;
}

D3D11_TEXTURE2D_DESC Direct3D::make_depth_buffer_desc(int width, int height)
{
	D3D11_TEXTURE2D_DESC depth_buffer_desc;
	ZeroMemory(&depth_buffer_desc, sizeof(depth_buffer_desc));

	depth_buffer_desc.Width = width;
	depth_buffer_desc.Height = height;
	depth_buffer_desc.MipLevels = 1;
	depth_buffer_desc.ArraySize = 1;
	depth_buffer_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_buffer_desc.SampleDesc.Count = 1;
	depth_buffer_desc.SampleDesc.Quality = 0;
	depth_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	depth_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depth_buffer_desc.CPUAccessFlags = 0;
	depth_buffer_desc.MiscFlags = 0;

	return depth_buffer_desc;
}

std::tuple<unsigned int, unsigned int> Direct3D::get_display_mode_data(int width, int height, IDXGIAdapter* adapter)
{
	D3DRAII<IDXGIOutput> adapter_out;
	HRESULT result = adapter->EnumOutputs(0, &adapter_out.val);
	if (FAILED(result))
	{
		throw;
	}

	unsigned int mode_count;
	result = adapter_out.val->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_ENUM_MODES_INTERLACED,
		&mode_count,
		0);

	std::vector<DXGI_MODE_DESC> display_modes(mode_count);
	{
		result = adapter_out.val->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,
			DXGI_ENUM_MODES_INTERLACED,
			&mode_count,
			display_modes.data());

		if (FAILED(result))
		{
			throw;
		}
	}
	
	for (unsigned int i = 0; i < mode_count; ++i)
	{
		if (display_modes[i].Width == static_cast<unsigned int>(width))
		{
			if (display_modes[i].Height == static_cast<unsigned int>(height))
			{
				return { display_modes[i].RefreshRate.Numerator, display_modes[i].RefreshRate.Denominator };
			}
		}
	}

	return { 0, 0 };
}

bool Direct3D::create_device_and_swapchain(HWND window, int width, int height, int numerator, int denominator)
{
	auto swapchain_desc = make_swapchain_desc(window, width, height, numerator, denominator);

	D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_11_0;

	HRESULT result = D3D11CreateDeviceAndSwapChain(0, D3D_DRIVER_TYPE_HARDWARE, 0, 0, &feature_level, 1,
		D3D11_SDK_VERSION, &swapchain_desc, &swapchain.val, &device.val, 0, &device_context.val);

	return !FAILED(result);
}

bool Direct3D::create_render_target_view()
{
	D3DRAII<ID3D11Texture2D> back_buffer;
	HRESULT result = swapchain.val->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer.val);
	if (FAILED(result))
	{
		return false;
	}

	result = device.val->CreateRenderTargetView(back_buffer.val, 0, &render_target_view.val);

	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool Direct3D::create_texture2d(int width, int height)
{
	auto depth_buffer_desc = make_depth_buffer_desc(width, height);

	HRESULT result = device.val->CreateTexture2D(&depth_buffer_desc, 0, &depth_stencil_buffer.val);
	return !FAILED(result);
}

bool Direct3D::create_depth_stencil_view()
{
	D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc;
	ZeroMemory(&depth_stencil_view_desc, sizeof(depth_stencil_view_desc));

	depth_stencil_view_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depth_stencil_view_desc.Texture2D.MipSlice = 0;

	HRESULT result = device.val->CreateDepthStencilView(depth_stencil_buffer.val, &depth_stencil_view_desc, &depth_stencil_view.val);

	return !FAILED(result);
}

bool Direct3D::create_depth_stencil_state()
{	
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));

	depth_stencil_desc.DepthEnable = true;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;

	depth_stencil_desc.StencilEnable = true;
	depth_stencil_desc.StencilReadMask = 0xFF;
	depth_stencil_desc.StencilWriteMask = 0xFF;

	depth_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depth_stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depth_stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	HRESULT result = device.val->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_state.val);
	return !FAILED(result);
}

bool Direct3D::create_rasterizer_state()
{
	D3D11_RASTERIZER_DESC raster_desc;
	raster_desc.AntialiasedLineEnable = false;
	raster_desc.CullMode = D3D11_CULL_BACK;
	raster_desc.DepthBias = 0;
	raster_desc.DepthBiasClamp = 0.0f;
	raster_desc.DepthClipEnable = true;
	raster_desc.FillMode = D3D11_FILL_WIREFRAME;
	raster_desc.FrontCounterClockwise = false;
	raster_desc.MultisampleEnable = false;
	raster_desc.ScissorEnable = false;
	raster_desc.SlopeScaledDepthBias = 0.0f;

	HRESULT result = device.val->CreateRasterizerState(&raster_desc, &raster_state.val);
	return !FAILED(result);
}

void Direct3D::create_viewport(float width, float height)
{
	D3D11_VIEWPORT viewport;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	device_context.val->RSSetViewports(1, &viewport);
}

void Direct3D::create_matrices(float width, float height, float screen_near, float screen_depth)
{
	static constexpr float PI = 3.14159f;
	float fov = PI / 4.0f;
	float aspect_ratio = width / height;

	projection_matrix = dx::XMMatrixPerspectiveFovLH(fov, aspect_ratio, screen_near, screen_depth);
	world_matrix = dx::XMMatrixIdentity();
	ortho_matrix = dx::XMMatrixOrthographicOffCenterLH(
		-10 * aspect_ratio,
		10 * aspect_ratio ,
		-10,
		10,
		screen_near,
		screen_depth);
}

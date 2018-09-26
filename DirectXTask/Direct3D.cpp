#include "Direct3D.hpp"

#include <iostream>

bool Direct3D::init(int width, int height, bool vsync, HWND window, bool fullscreen, float screen_depth, float screen_near)
{
	this->vsync = vsync;
	this->fullscreen = fullscreen;

	HRESULT result;
	IDXGIFactory* factory;
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		return false;
	}

	IDXGIAdapter* adapter;
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		return false;
	}

	IDXGIOutput* adapter_out;
	result = adapter->EnumOutputs(0, &adapter_out);
	if (FAILED(result))
	{
		return false;
	}

	unsigned int mode_count;
	result = adapter_out->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_ENUM_MODES_INTERLACED,
		&mode_count,
		0);

	DXGI_MODE_DESC* display_modes = new DXGI_MODE_DESC[mode_count];
	if (!display_modes)
	{
		return false;
	}

	result = adapter_out->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_ENUM_MODES_INTERLACED,
		&mode_count,
		display_modes);
	
	if (FAILED(result))
	{
		return false;
	}

	unsigned int numerator = 0;
	unsigned int denominator = 0;

	for (unsigned int i = 0; i < mode_count; ++i)
	{
		if (display_modes[i].Width == static_cast<unsigned int>(width))
		{
			if (display_modes[i].Height == static_cast<unsigned int>(height))
			{
				numerator = display_modes[i].RefreshRate.Numerator;
				denominator = display_modes[i].RefreshRate.Denominator;
			}
		}
	}

	DXGI_ADAPTER_DESC adapter_desc;
	result = adapter->GetDesc(&adapter_desc);
	if (FAILED(result))
	{
		return false;
	}

	gpu_memory = static_cast<int>(adapter_desc.DedicatedVideoMemory / 1024 / 1024);

	size_t string_length;
	int error = wcstombs_s(&string_length, gpu_description, 128, adapter_desc.Description, 128);
	if (error != 0)
	{
		return false;
	}

	//todo: RAII
	delete[] display_modes;
	display_modes = 0;

	adapter_out->Release();
	adapter_out = 0;
	adapter->Release();
	adapter = 0;
	//factory->Release();
	//factory = 0;

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

	//bloody hell this is a long function, refactor. todo

	D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_11_0;

	result = D3D11CreateDeviceAndSwapChain(0, D3D_DRIVER_TYPE_HARDWARE, 0, D3D11_CREATE_DEVICE_DEBUG, &feature_level, 1,
		D3D11_SDK_VERSION, &swapchain_desc, &swapchain, &device, 0, &device_context);
	if (FAILED(result))
	{
		return false;
	}

	ID3D11Texture2D* back_buffer;
	result = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer);
	if (FAILED(result))
	{
		return false;
	}

	result = device->CreateRenderTargetView(back_buffer, 0, &render_target_view);
	if (FAILED(result))
	{
		return false;
	}

	//RAII todo
	back_buffer->Release();
	back_buffer = 0;

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

	result = device->CreateTexture2D(&depth_buffer_desc, 0, &depth_stencil_buffer);
	if (FAILED(result))
	{
		return false;
	}

	//So very long, todo refactor

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

	result = device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_state);
	if (FAILED(result))
	{
		return false;
	}

	device_context->OMSetDepthStencilState(depth_stencil_state, 1);

	//so very very long, refactor
	D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc;
	ZeroMemory(&depth_stencil_view_desc, sizeof(depth_stencil_view_desc));

	depth_stencil_view_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depth_stencil_view_desc.Texture2D.MipSlice = 0;

	result = device->CreateDepthStencilView(depth_stencil_buffer, &depth_stencil_view_desc, &depth_stencil_view);
	if (FAILED(result))
	{
		return false;
	}

	device_context->OMSetRenderTargets(1, &render_target_view, depth_stencil_view);

	D3D11_RASTERIZER_DESC raster_desc;
	raster_desc.AntialiasedLineEnable = false;
	raster_desc.CullMode = D3D11_CULL_BACK;
	raster_desc.DepthBias = 0;
	raster_desc.DepthBiasClamp = 0.0f;
	raster_desc.DepthClipEnable = true;
	raster_desc.FillMode = D3D11_FILL_SOLID;
	raster_desc.FrontCounterClockwise = false;
	raster_desc.MultisampleEnable = false;
	raster_desc.ScissorEnable = false;
	raster_desc.SlopeScaledDepthBias = 0.0f;

	result = device->CreateRasterizerState(&raster_desc, &raster_state);
	if (FAILED(result))
	{
		return false;
	}

	device_context->RSSetState(raster_state);

	D3D11_VIEWPORT viewport;
	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	device_context->RSSetViewports(1, &viewport);

	//sigh, so very very very very very very very long, refactor
	static constexpr float PI = 3.14159f;
	float fov = PI / 4.0f;
	float aspect_ratio = static_cast<float>(width) / static_cast<float>(height);

	projection_matrix = dx::XMMatrixPerspectiveFovLH(fov, aspect_ratio, screen_near, screen_depth);
	world_matrix = dx::XMMatrixIdentity();
	ortho_matrix = dx::XMMatrixOrthographicLH(static_cast<float>(width), static_cast<float>(height), screen_near, screen_depth);

	return true;
}

void Direct3D::stop()
{
	if (swapchain)
	{
		swapchain->SetFullscreenState(false, 0);
	}

	if (raster_state)
	{
		raster_state->Release();
		raster_state = 0;
	}

	if (depth_stencil_view)
	{
		depth_stencil_view->Release();
		depth_stencil_view = 0;
	}

	if (depth_stencil_state)
	{
		depth_stencil_state->Release();
		depth_stencil_state = 0;
	}

	if (depth_stencil_buffer)
	{
		depth_stencil_buffer->Release();
		depth_stencil_buffer = 0;
	}

	if (render_target_view)
	{
		render_target_view->Release();
		render_target_view = 0;
	}

	if (device_context)
	{
		device_context->Release();
		device_context = 0;
	}

	if (device)
	{
		device->Release();
		device = 0;
	}

	if (swapchain)
	{
		swapchain->Release();
		swapchain = 0;
	}
}

void Direct3D::begin(float r, float g, float b, float a)
{
	float colour[4] = { r, g, b, a };
	device_context->ClearRenderTargetView(render_target_view, colour);
	device_context->ClearDepthStencilView(depth_stencil_view, D3D11_CLEAR_DEPTH, 1.0f, 0);

	static float xpos = 0;
	static float ypos = 0;
	static float zpos = 0;

	if (GetAsyncKeyState('A'))
	{
		xpos += -1.0f;
	}

	if (GetAsyncKeyState('D'))
	{
		xpos += 1.0f;
	}

	if (GetAsyncKeyState('W'))
	{
		ypos += 1.0f;
	}

	if (GetAsyncKeyState('S'))
	{
		ypos -= 1.0f;
	}

	if (GetAsyncKeyState('Q'))
	{
		zpos += 1.0f;
	}

	if (GetAsyncKeyState('E'))
	{
		zpos -= 1.0f;
	}

	world_matrix += dx::XMMatrixTranslation(xpos, ypos, zpos);
}

void Direct3D::end()
{
	if (vsync)
	{
		swapchain->Present(1, 0);
	}
	else
	{
		swapchain->Present(0, 0);
	}

	world_matrix = dx::XMMatrixIdentity();
}

ID3D11Device* Direct3D::get_device()
{
	return device;
}

ID3D11DeviceContext* Direct3D::get_device_context()
{
	return device_context;
}

void Direct3D::get_projection_matrix(dx::XMMATRIX& matrix)
{
	matrix = projection_matrix;
}

void Direct3D::get_world_matrix(dx::XMMATRIX& matrix)
{
	matrix = world_matrix;
}

void Direct3D::get_ortho_matrix(dx::XMMATRIX& matrix)
{
	matrix = ortho_matrix;
}

void Direct3D::get_gpu_info(char* name, int& memory)
{
	strcpy_s(name, 128, gpu_description);
	memory = gpu_memory;
}

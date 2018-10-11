#include "ColourShader.hpp"

//STD
#include <iostream>

ColourShader::ColourShader(ID3D11Device* device, HWND window)
{
	if (!init_shader(device, window, L"colour.vs", L"colour.ps"))
	{
		MessageBoxW(window, L"Could not init shader", L"ERROR", MB_OK);
		throw;
	}
}

ColourShader::~ColourShader()
{
	stop_shader();
}

bool ColourShader::render(ID3D11DeviceContext* device_context, int index_count, int vertex_count, int instance_count, dx::XMMATRIX world, dx::XMMATRIX view, dx::XMMATRIX projection, dx::XMFLOAT3 light_direction, dx::XMFLOAT4 diffuse_colour)
{
	bool successful = set_shader_params(device_context, world, view, projection, light_direction, diffuse_colour);
	if (successful)
	{
		render_shader(device_context, index_count, vertex_count, instance_count);
	}

	return successful;
}

bool ColourShader::init_shader(ID3D11Device* device, HWND window, LPCWSTR vs_filename, LPCWSTR ps_filename)
{
	ID3D10Blob* error_msg = nullptr;
	ID3D10Blob* vertex_shader_buffer = nullptr;

	HRESULT result = D3DCompileFromFile(vs_filename, 0, 0, "ColourVertexShader", "vs_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertex_shader_buffer, &error_msg);

	if (FAILED(result))
	{
		if (error_msg)
		{
			print_shader_error(error_msg, window, vs_filename);
		}
		else
		{
			MessageBoxW(window, vs_filename, L"Missing VS Shader File", MB_OK);
		}

		throw;
		return false;
	}

	ID3D10Blob* pixel_shader_buffer = nullptr;
	result = D3DCompileFromFile(ps_filename, 0, 0, "ColourPixelShader", "ps_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixel_shader_buffer, &error_msg);

	if (FAILED(result))
	{
		if (error_msg)
		{
			print_shader_error(error_msg, window, ps_filename);
		}
		else
		{
			MessageBoxW(window, ps_filename, L"Missing PS Shader File", MB_OK);
		}

		throw;
		return false;
	}

	result = device->CreateVertexShader(vertex_shader_buffer->GetBufferPointer(), vertex_shader_buffer->GetBufferSize(), NULL, &vertex_shader);
	if (FAILED(result))
	{
		throw;
		return false;
	}

	result = device->CreatePixelShader(pixel_shader_buffer->GetBufferPointer(), pixel_shader_buffer->GetBufferSize(), NULL, &pixel_shader);
	if (FAILED(result))
	{
		throw;
		return false;
	}

	constexpr unsigned int element_count = 7;
	D3D11_INPUT_ELEMENT_DESC polygon[element_count];

	polygon[0].SemanticName = "POSITION";
	polygon[0].SemanticIndex = 0;
	polygon[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygon[0].InputSlot = 0;
	polygon[0].AlignedByteOffset = 0;
	polygon[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygon[0].InstanceDataStepRate = 0;

	polygon[1].SemanticName = "COLOR";
	polygon[1].SemanticIndex = 0;
	polygon[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygon[1].InputSlot = 0;
	polygon[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygon[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygon[1].InstanceDataStepRate = 0;

	polygon[2].SemanticName = "NORMAL";
	polygon[2].SemanticIndex = 0;
	polygon[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygon[2].InputSlot = 0;
	polygon[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygon[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygon[2].InstanceDataStepRate = 0;

	polygon[3].SemanticName = "POSITION";
	polygon[3].SemanticIndex = 1;
	polygon[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygon[3].InputSlot = 1;
	polygon[3].AlignedByteOffset = 0;
	polygon[3].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	polygon[3].InstanceDataStepRate = 1;

	polygon[4].SemanticName = "POSITION";
	polygon[4].SemanticIndex = 2;
	polygon[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygon[4].InputSlot = 1;
	polygon[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygon[4].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	polygon[4].InstanceDataStepRate = 1;

	polygon[5].SemanticName = "POSITION";
	polygon[5].SemanticIndex = 3;
	polygon[5].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygon[5].InputSlot = 1;
	polygon[5].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygon[5].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	polygon[5].InstanceDataStepRate = 1;

	polygon[6].SemanticName = "COLOR";
	polygon[6].SemanticIndex = 1;
	polygon[6].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygon[6].InputSlot = 1;
	polygon[6].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygon[6].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	polygon[6].InstanceDataStepRate = 1;

	result = device->CreateInputLayout(polygon, element_count, vertex_shader_buffer->GetBufferPointer(), 
		vertex_shader_buffer->GetBufferSize(), &layout);
	if (FAILED(result))
	{
		throw;
		return false;
	}

	vertex_shader_buffer->Release();
	vertex_shader_buffer = 0;

	pixel_shader_buffer->Release();
	pixel_shader_buffer = 0;

	D3D11_BUFFER_DESC matrix_buffer_desc;
	matrix_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	matrix_buffer_desc.ByteWidth = sizeof(MatrixBuffer);
	matrix_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrix_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrix_buffer_desc.MiscFlags = 0;
	matrix_buffer_desc.StructureByteStride = 0;

	result = device->CreateBuffer(&matrix_buffer_desc, 0, &matrix_buffer);
	if (FAILED(result))
	{
		return false;
	}

	//buffers need to be multiples of 16 bytes
	assert(sizeof(LightBuffer) % 16 == 0);

	D3D11_BUFFER_DESC light_buffer_desc;
	light_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	light_buffer_desc.ByteWidth = sizeof(LightBuffer);
	light_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	light_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	light_buffer_desc.MiscFlags = 0;
	light_buffer_desc.StructureByteStride = 0;

	result = device->CreateBuffer(&light_buffer_desc, 0, &light_buffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void ColourShader::stop_shader()
{
	if (light_buffer)
	{
		light_buffer->Release();
		light_buffer = 0;
	}

	if (matrix_buffer)
	{
		matrix_buffer->Release();
		matrix_buffer = 0;
	}

	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	if (pixel_shader)
	{
		pixel_shader->Release();
		pixel_shader = 0;
	}

	if (vertex_shader)
	{
		vertex_shader->Release();
		vertex_shader = 0;
	}
}

void ColourShader::print_shader_error(ID3D10Blob* error_msg, HWND window, LPCWSTR filename)
{
	char* compile_errors = static_cast<char*>(error_msg->GetBufferPointer());
	std::cout << compile_errors << "\n";
	unsigned long long buffer_size = error_msg->GetBufferSize();

	std::ofstream fout;
	fout.open("shader-error.txt");

	for (unsigned long long i = 0; i < buffer_size; i++)
	{
		fout << compile_errors[i];
	}

	fout.close();

	error_msg->Release();
	error_msg = 0;

	MessageBoxW(window, L"Error compiling shader.  Check shader-error.txt for message.", filename, MB_OK);
}

bool ColourShader::set_shader_params(ID3D11DeviceContext* device_context, dx::XMMATRIX world, dx::XMMATRIX view, dx::XMMATRIX projection, dx::XMFLOAT3 light_direction, dx::XMFLOAT4 diffuse_colour)
{
	world = dx::XMMatrixTranspose(world);
	view = dx::XMMatrixTranspose(view);
	projection = dx::XMMatrixTranspose(projection);


	{
		D3D11_MAPPED_SUBRESOURCE mapped_resource;

		HRESULT result = device_context->Map(matrix_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
		if (FAILED(result))
		{
			return false;
		}

		MatrixBuffer* data = static_cast<MatrixBuffer*>(mapped_resource.pData);

		data->world = world;
		data->view = view;
		data->projection = projection;

		device_context->Unmap(matrix_buffer, 0);

		device_context->VSSetConstantBuffers(0, 1, &matrix_buffer);
	}

	{
		D3D11_MAPPED_SUBRESOURCE mapped_resource;
		HRESULT result = device_context->Map(light_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
		if (FAILED(result))
		{
			return false;
		}

		LightBuffer* data = static_cast<LightBuffer*>(mapped_resource.pData);
		data->diffuse_colour = diffuse_colour;
		data->light_direction = light_direction;

		device_context->Unmap(light_buffer, 0);
		device_context->PSSetConstantBuffers(0, 1, &light_buffer);
	}

	return true;
}

void ColourShader::render_shader(ID3D11DeviceContext* device_context, int index_count, int vertex_count, int instance_count)
{
	device_context->IASetInputLayout(layout);

	device_context->VSSetShader(vertex_shader, 0, 0);
	device_context->PSSetShader(pixel_shader, 0, 0);

	device_context->DrawIndexedInstanced(index_count, instance_count, 0, 0, 0);
}
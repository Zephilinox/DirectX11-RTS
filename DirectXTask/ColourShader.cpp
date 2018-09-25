#include "ColourShader.hpp"

//STD
#include <iostream>

bool ColourShader::init(ID3D11Device* device, HWND window)
{
	//https://stackoverflow.com/questions/7016206/trying-to-createdirectory-getting-char-to-lpcwstr-error-willing-to-try-anothe
	bool successful = init_shader(device, window, L"colour.vs", L"colour.ps");
	return successful;
}

void ColourShader::stop()
{
	stop_shader();
}

bool ColourShader::render(ID3D11DeviceContext* device_context, int index_count, dx::XMMATRIX world, dx::XMMATRIX view, dx::XMMATRIX projection)
{
	bool successful = set_shader_params(device_context, world, view, projection);
	if (successful)
	{
		render_shader(device_context, index_count);
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

		return false;
	}

	result = device->CreateVertexShader(vertex_shader_buffer->GetBufferPointer(), vertex_shader_buffer->GetBufferSize(), NULL, &vertex_shader);
	if (FAILED(result))
	{
		return false;
	}

	result = device->CreatePixelShader(pixel_shader_buffer->GetBufferPointer(), pixel_shader_buffer->GetBufferSize(), NULL, &pixel_shader);
	if (FAILED(result))
	{
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC polygon[2];
	unsigned int element_count;

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

	element_count = sizeof(polygon) / sizeof(polygon[0]);

	//fails here
	result = device->CreateInputLayout(polygon, element_count, vertex_shader_buffer->GetBufferPointer(), 
		vertex_shader_buffer->GetBufferSize(), &layout);
	if (FAILED(result))
	{
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
	if(FAILED(result))
	{
		return false;
	}

	return true;
}

void ColourShader::stop_shader()
{
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

bool ColourShader::set_shader_params(ID3D11DeviceContext* device_context, dx::XMMATRIX world, dx::XMMATRIX view, dx::XMMATRIX projection)
{
	world = dx::XMMatrixTranspose(world);
	view = dx::XMMatrixTranspose(view);
	projection = dx::XMMatrixTranspose(projection);

	D3D11_MAPPED_SUBRESOURCE mapped_resource;
	unsigned int buffer_count;

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

	buffer_count = 0;

	device_context->VSSetConstantBuffers(buffer_count, 1, &matrix_buffer);

	return true;
}

void ColourShader::render_shader(ID3D11DeviceContext* device_context, int index_count)
{
	device_context->IASetInputLayout(layout);

	device_context->VSSetShader(vertex_shader, 0, 0);
	device_context->PSSetShader(pixel_shader, 0, 0);

	device_context->DrawIndexed(index_count, 0, 0);
}

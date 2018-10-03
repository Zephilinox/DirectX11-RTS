#include "Model.hpp"

Model::Model(ID3D11Device* device,
	dx::XMFLOAT3 vertex1,
	dx::XMFLOAT3 vertex2,
	dx::XMFLOAT3 vertex3)
{
	vertex_count = 3;
	index_count = 3;

	VertexType* vertices = new VertexType[vertex_count];
	unsigned long* indices = new unsigned long[index_count];

	//Bottom left
	vertices[0].position = vertex1;  
	vertices[0].colour = dx::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	//Top middle
	vertices[1].position = vertex2;  
	vertices[1].colour = dx::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	//Bottom right
	vertices[2].position = vertex3;  
	vertices[2].colour = dx::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

	D3D11_BUFFER_DESC vertex_buffer_desc;
	vertex_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	vertex_buffer_desc.ByteWidth = sizeof(VertexType) * vertex_count;
	vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertex_buffer_desc.CPUAccessFlags = 0;
	vertex_buffer_desc.MiscFlags = 0;
	vertex_buffer_desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertex_data;
	vertex_data.pSysMem = vertices;
	vertex_data.SysMemPitch = 0;
	vertex_data.SysMemSlicePitch = 0;

	HRESULT result = device->CreateBuffer(&vertex_buffer_desc, &vertex_data, &vertex_buffer);
	if (FAILED(result))
	{
		throw;
	}

	D3D11_BUFFER_DESC index_buffer_desc;
	index_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	index_buffer_desc.ByteWidth = sizeof(unsigned long) * index_count;
	index_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	index_buffer_desc.CPUAccessFlags = 0;
	index_buffer_desc.MiscFlags = 0;
	index_buffer_desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA index_data;
	index_data.pSysMem = indices;
	index_data.SysMemPitch = 0;
	index_data.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&index_buffer_desc, &index_data, &index_buffer);
	if (FAILED(result))
	{
		throw;
	}

	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;
}

Model::~Model()
{
	stop_buffers();
}

void Model::render(ID3D11DeviceContext* device_context)
{
	render_buffers(device_context);
}

int Model::get_index_count()
{
	return index_count;
}

void Model::stop_buffers()
{
	if (index_buffer)
	{
		index_buffer->Release();
		index_buffer = nullptr;
	}

	if (vertex_buffer)
	{
		vertex_buffer->Release();
		vertex_buffer = nullptr;
	}
}

void Model::render_buffers(ID3D11DeviceContext* device_context)
{
	unsigned int stride = sizeof(VertexType); 
	unsigned int offset = 0;

	device_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
	device_context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);
	device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

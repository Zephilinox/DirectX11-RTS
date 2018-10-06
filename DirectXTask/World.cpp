#include "World.hpp"

World::World(ID3D11Device* device)
{
	constexpr int terrainHeight = 64;
	constexpr int terrainWidth = 64;

	// Set the color of the terrain grid.
	dx::XMFLOAT4 color  = dx::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.5f);
	dx::XMFLOAT4 color2 = dx::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.5f);
	dx::XMFLOAT4 color3 = dx::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.5f);
	dx::XMFLOAT4 color4 = dx::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.5f);
	// Calculate the number of vertices in the terrain.
	vertex_count = (terrainWidth - 1) * (terrainHeight - 1) * 6;

	// Set the index count to the same as the vertex count.
	index_count = vertex_count;
	// Create the vertex array.
	Vertex* vertices = new Vertex[vertex_count];

	// Create the index array.
	unsigned long* indices = new unsigned long[index_count];

	int index = 0;
	for (int h = 0; h < terrainHeight; ++h)
	{
		for (int w = 0; w < terrainWidth; ++w)
		{
			vertices[index].position = { static_cast<float>(w), 0, static_cast<float>(h) };
			vertices[index].colour = { 1.0f, 1.0f, 1.0f, 0.0f };
			index++;
		}
	}

	int k = 0;
	int l = 0;
	for (int i = 0; i < index_count; i += 6)
	{
		indices[i] = k;
		indices[i + 1] = k + terrainWidth;
		indices[i + 2] = k + terrainWidth + 1;
		indices[i + 3] = k;
		indices[i + 4] = k + terrainWidth + 1;
		indices[i + 5] = k + 1;
		k++;
		l++;
		if (l == terrainWidth - 1)
		{
			l = 0;
			k++;
		}
	}

	// Set up the description of the static vertex buffer.
	D3D11_BUFFER_DESC vertex_buffer_desc;
	vertex_buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
	vertex_buffer_desc.ByteWidth = sizeof(Vertex) * vertex_count;
	vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertex_buffer_desc.CPUAccessFlags = 0;
	vertex_buffer_desc.MiscFlags = 0;
	vertex_buffer_desc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	D3D11_SUBRESOURCE_DATA vertex_data;
	vertex_data.pSysMem = vertices;
	vertex_data.SysMemPitch = 0;
	vertex_data.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	HRESULT result = device->CreateBuffer(&vertex_buffer_desc, &vertex_data, &vertex_buffer);
	if (FAILED(result))
	{
		throw;
	}

	// Set up the description of the static index buffer.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * index_count;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &index_buffer);
	if (FAILED(result))
	{
		throw;
	}
	
	instance_count = 1;

	Instance* instances = new Instance[instance_count];

	instances[0].position = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);
	instances[0].colour = dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	D3D11_BUFFER_DESC instance_buffer_desc;
	instance_buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
	instance_buffer_desc.ByteWidth = sizeof(Instance) * instance_count;
	instance_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instance_buffer_desc.CPUAccessFlags = 0;
	instance_buffer_desc.MiscFlags = 0;
	instance_buffer_desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA instance_data;
	instance_data.pSysMem = instances;
	instance_data.SysMemPitch = 0;
	instance_data.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&instance_buffer_desc, &instance_data, &instance_buffer);
	if (FAILED(result))
	{
		throw;
	}

	// Release the arrays now that the buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	delete[] instances;
	instances = 0;
}

World::~World()
{
	// Release the index buffer.
	if (instance_buffer)
	{
		instance_buffer->Release();
		instance_buffer = 0;
	}

	// Release the index buffer.
	if (index_buffer)
	{
		index_buffer->Release();
		index_buffer = 0;
	}

	// Release the vertex buffer.
	if (vertex_buffer)
	{
		vertex_buffer->Release();
		vertex_buffer = 0;
	}
}

bool World::update(Input* input, float dt)
{
	return false;
}

int World::get_index_count()
{
	return index_count;
}

bool World::draw(ID3D11DeviceContext* device_context)
{

	unsigned int strides[2] = { sizeof(Vertex), sizeof(Instance) };
	unsigned int offsets[2] = { 0, 0 };

	ID3D11Buffer* buffer_pointers[2] = { vertex_buffer, instance_buffer };

	device_context->IASetVertexBuffers(0, 2, buffer_pointers, strides, offsets);
	device_context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);
	device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return false;
}
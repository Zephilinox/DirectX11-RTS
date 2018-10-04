#include "World.hpp"

World::World(ID3D11Device* device)
{
	/*VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i, j, terrainWidth, terrainHeight, index;
	XMFLOAT4 color;
	float positionX, positionZ;*/
	// Set the height and width of the terrain grid.
	constexpr int terrainHeight = 64;
	constexpr int terrainWidth = 64;

	// Set the color of the terrain grid.
	dx::XMFLOAT4 color = dx::XMFLOAT4(0.0f, 1.0f, 0.2f, 0.5f);
	// Calculate the number of vertices in the terrain.
	vertex_count = (terrainWidth - 1) * (terrainHeight - 1) * 8;

	// Set the index count to the same as the vertex count.
	index_count = vertex_count;
	// Create the vertex array.
	VertexType* vertices = new VertexType[vertex_count];

	// Create the index array.
	unsigned long* indices = new unsigned long[index_count];

	// Initialize the index into the vertex and index arrays.
	int index = 0;

	// Load the vertex array and index array with data.
	for (int j = 0; j<(terrainHeight - 1); j++)
	{
		for (int i = 0; i<(terrainWidth - 1); i++)
		{
			// Line 1 - Upper left.
			float positionX = (float)i;
			float positionZ = (float)(j + 1);

			vertices[index].position = dx::XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			// Line 1 - Upper right.
			positionX = (float)(i + 1);
			positionZ = (float)(j + 1);

			vertices[index].position = dx::XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			// Line 2 - Upper right
			positionX = (float)(i + 1);
			positionZ = (float)(j + 1);

			vertices[index].position = dx::XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			// Line 2 - Bottom right.
			positionX = (float)(i + 1);
			positionZ = (float)j;

			vertices[index].position = dx::XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			// Line 3 - Bottom right.
			positionX = (float)(i + 1);
			positionZ = (float)j;

			vertices[index].position = dx::XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			// Line 3 - Bottom left.
			positionX = (float)i;
			positionZ = (float)j;

			vertices[index].position = dx::XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			// Line 4 - Bottom left.
			positionX = (float)i;
			positionZ = (float)j;

			vertices[index].position = dx::XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			// Line 4 - Upper left.
			positionX = (float)i;
			positionZ = (float)(j + 1);

			vertices[index].position = dx::XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;
		}
	}

	// Set up the description of the static vertex buffer.
	D3D11_BUFFER_DESC vertex_buffer_desc;
	vertex_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	vertex_buffer_desc.ByteWidth = sizeof(VertexType) * vertex_count;
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
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
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

	// Release the arrays now that the buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;
}

World::~World()
{

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
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	device_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	device_context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case lines.
	device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	return false;
}
#include "Model.hpp"

//STD
#include <iostream>

Model::Model(ID3D11Device* device)
{
	vertex_count = 8;
	index_count = 36;

	std::vector<Vertex> vertices(vertex_count);
	std::vector<unsigned long> indices(index_count);

	vertices[0].position = { -1.0f, -1.0f, -1.0f };
	vertices[0].colour = {0.9f, 0.9f, 0.9f, 1.0f};

	vertices[1].position = { -1.0f, 1.0f, -1.0f };
	vertices[1].colour = { 0.8f, 0.8f, 0.8f, 1.0f };

	vertices[2].position = { 1.0f, 1.0f, -1.0f };
	vertices[2].colour = { 0.7f, 0.7f, 0.7f, 1.0f };

	vertices[3].position = { 1.0f, -1.0f, -1.0f };
	vertices[3].colour = { 0.6f, 0.6f, 0.6f, 1.0f };

	vertices[4].position = { -1.0f, -1.0f, 1.0f };
	vertices[4].colour = { 0.9f, 0.9f, 0.9f, 1.0f };

	vertices[5].position = { -1.0f, 1.0f, 1.0f };
	vertices[5].colour = { 0.8f, 0.8f, 0.8f, 1.0f };

	vertices[6].position = { 1.0f, 1.0f, 1.0f };
	vertices[6].colour = { 0.7f, 0.7f, 0.7f, 1.0f };

	vertices[7].position = { 1.0f, -1.0f, 1.0f };
	vertices[7].colour = { 0.6f, 0.6f, 0.6f, 1.0f };

	//front
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 0;
	indices[4] = 2;
	indices[5] = 3;

	//back
	indices[6] = 4;
	indices[7] = 6;
	indices[8] = 5;
	indices[9] = 4;
	indices[10] = 7;
	indices[11] = 6;

	//left
	indices[12] = 4;
	indices[13] = 5;
	indices[14] = 1;
	indices[15] = 4;
	indices[16] = 1;
	indices[17] = 0;

	//right
	indices[18] = 3;
	indices[19] = 2;
	indices[20] = 6;
	indices[21] = 3;
	indices[22] = 6;
	indices[23] = 7;

	//top
	indices[24] = 1;
	indices[25] = 5;
	indices[26] = 6;
	indices[27] = 1;
	indices[28] = 6;
	indices[29] = 2;

	//bottom
	indices[30] = 4;
	indices[31] = 0;
	indices[32] = 3;
	indices[33] = 4;
	indices[34] = 3;
	indices[35] = 7;

	D3D11_BUFFER_DESC vertex_buffer_desc;
	vertex_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	vertex_buffer_desc.ByteWidth = sizeof(Vertex) * vertex_count;
	vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertex_buffer_desc.CPUAccessFlags = 0;
	vertex_buffer_desc.MiscFlags = 0;
	vertex_buffer_desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertex_data;
	vertex_data.pSysMem = vertices.data();
	vertex_data.SysMemPitch = 0;
	vertex_data.SysMemSlicePitch = 0;

	HRESULT result = device->CreateBuffer(&vertex_buffer_desc, &vertex_data, &vertex_buffer.val);
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
	index_data.pSysMem = indices.data();
	index_data.SysMemPitch = 0;
	index_data.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&index_buffer_desc, &index_data, &index_buffer.val);
	if (FAILED(result))
	{
		throw;
	}

	create_instance_buffer(device, 5);
}

Model::~Model()
{
	//todo: see if I need this
}

void Model::render(ID3D11DeviceContext* device_context)
{
	unsigned int strides[2] = { sizeof(Vertex), sizeof(Instance) };
	unsigned int offsets[2] = { 0, 0 };

	ID3D11Buffer* buffer_pointers[2] = { vertex_buffer.val, instance_buffer.val };

	device_context->IASetVertexBuffers(0, 2, buffer_pointers, strides, offsets);
	device_context->IASetIndexBuffer(index_buffer.val, DXGI_FORMAT_R32_UINT, 0);
	device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

int Model::get_vertex_count()
{
	return vertex_count;
}

int Model::get_instance_count()
{
	return instance_count;
}

int Model::get_index_count()
{
	return index_count;
}

void Model::create_instance_buffer(ID3D11Device* device, int max_instances)
{
	max_instance_count = max_instances;
	instance_count = max_instances;

	std::vector<Instance> instances(instance_count);

	D3D11_BUFFER_DESC instance_buffer_desc;
	instance_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	instance_buffer_desc.ByteWidth = sizeof(Instance) * instance_count;
	instance_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instance_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	instance_buffer_desc.MiscFlags = 0;
	instance_buffer_desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA instance_data;
	instance_data.pSysMem = instances.data();
	instance_data.SysMemPitch = 0;
	instance_data.SysMemSlicePitch = 0;

	HRESULT result = device->CreateBuffer(&instance_buffer_desc, &instance_data, &instance_buffer.val);
	if (FAILED(result))
	{
		throw;
	}
}

void Model::update_instance_buffer(ID3D11Device* device, ID3D11DeviceContext* device_context, std::vector<Instance>& instances)
{
	assert(instances.size() > 0);
	if (instances.size() > max_instance_count)
	{
		std::cout << "Grew instance buffer from " << max_instance_count << " to ";
		if (instances.size() > max_instance_count * 2)
		{
			create_instance_buffer(device, instances.size());
		}
		else
		{
			create_instance_buffer(device, max_instance_count * 2);
		}
		std::cout << max_instance_count << "\n";
	}
	assert(instances.size() <= max_instance_count);

	D3D11_MAPPED_SUBRESOURCE resource;
	ZeroMemory(&resource, sizeof(resource));

	instance_count = instances.size();
	size_t size = sizeof(Instance) * instance_count;

	device_context->Map(instance_buffer.val, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, instances.data(), size);
	device_context->Unmap(instance_buffer.val, 0);
}
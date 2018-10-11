#include "Model.hpp"

//STD
#include <iostream>
#include <fstream>

Model::Model(ID3D11Device* device)
{
	load_from_file("Cube2.txt", 0);

	std::vector<Vertex> vertices(vertex_count);
	std::vector<unsigned long> indices(index_count);

	for (int i = 0; i < vertex_count; ++i)
	{
		vertices[i].position = dx::XMFLOAT3(model_data[i].x, model_data[i].y, model_data[i].z);
		vertices[i].colour = dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertices[i].normal = dx::XMFLOAT3(model_data[i].nx, model_data[i].ny, model_data[i].nz);
		//todo: texture;
		indices[i] = i;
	}

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

bool Model::load_from_file(std::string filename, int filetype = 0)
{
	if (filetype == 0)
	{
		std::ifstream fin;
		char input;
		int i;

		fin.open(filename);

		if (fin.fail())
		{
			return false;
		}

		fin >> vertex_count;

		index_count = vertex_count;

		model_data.resize(vertex_count);

		for (i = 0; i < vertex_count; i++)
		{
			fin >> model_data[i].x >> model_data[i].y >> model_data[i].z;
			fin >> model_data[i].tu >> model_data[i].tv;
			fin >> model_data[i].nx >> model_data[i].ny >> model_data[i].nz;
		}

		fin.close();

		return true;
	}
	else
	{

	}
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
	if (instances.capacity() > max_instance_count)
	{
		std::cout << "Grew instance buffer from " << max_instance_count << " to ";
		if (instances.capacity() > max_instance_count * 2)
		{
			create_instance_buffer(device, instances.capacity());
		}
		else
		{
			create_instance_buffer(device, max_instance_count * 2);
		}
		std::cout << max_instance_count << "\n";
	}
	assert(instances.capacity() <= max_instance_count);

	D3D11_MAPPED_SUBRESOURCE resource;
	ZeroMemory(&resource, sizeof(resource));

	instance_count = instances.size();
	size_t size = sizeof(Instance) * instance_count;

	device_context->Map(instance_buffer.val, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, instances.data(), size);
	device_context->Unmap(instance_buffer.val, 0);
}
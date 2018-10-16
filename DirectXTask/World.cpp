#include "World.hpp"

//STD
#include <iostream>

World::World(ID3D11Device* device)
{
	constexpr float resolution = 1.0f;
	constexpr int terrainHeight = static_cast<int>(32 * resolution) + 1;
	constexpr int terrainWidth = static_cast<int>(32 * resolution) + 1;

	vertex_count = (terrainWidth - 1) * (terrainHeight - 1) * 6;
	index_count = vertex_count;
	std::cout << index_count / 3 << "\n";
	vertices.resize(vertex_count);
	indices.resize(index_count);

	int index = 0;
	for (int h = 0; h < terrainHeight; ++h)
	{
		for (int w = 0; w < terrainWidth; ++w)
		{
			//float y = (std::rand() % 100) / 100.0f - 3;
			float y = 0;
			vertices[index].position = { static_cast<float>(w) * (1.0f / resolution), y, static_cast<float>(h) * (1.0f / resolution) };
			vertices[index].colour = { 0.8f, 0.8f, 0.75f, 1.0f };
			vertices[index].normal = { (std::rand() % 100) / 100.0f, (std::rand() % 100) / 100.0f, (std::rand() % 100) / 100.0f };
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

	D3D11_BUFFER_DESC vertex_buffer_desc;
	vertex_buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
	vertex_buffer_desc.ByteWidth = sizeof(Vertex) * vertex_count;
	vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertex_buffer_desc.CPUAccessFlags = 0;
	vertex_buffer_desc.MiscFlags = 0;
	vertex_buffer_desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertex_data;
	vertex_data.pSysMem = vertices.data();
	vertex_data.SysMemPitch = 0;
	vertex_data.SysMemSlicePitch = 0;

	HRESULT result = device->CreateBuffer(&vertex_buffer_desc, &vertex_data, &vertex_buffer);
	if (FAILED(result))
	{
		throw;
	}

	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * index_count;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&indexBufferDesc, &indexData, &index_buffer);
	if (FAILED(result))
	{
		throw;
	}
	
	instance_count = 1;
	std::vector<Instance> instances(instance_count);

	instances[0].position = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);
	instances[0].rotation = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);
	instances[0].scale = dx::XMFLOAT3(1.0f, 1.0f, 1.0f);
	instances[0].colour = dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	D3D11_BUFFER_DESC instance_buffer_desc;
	instance_buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
	instance_buffer_desc.ByteWidth = sizeof(Instance) * instance_count;
	instance_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instance_buffer_desc.CPUAccessFlags = 0;
	instance_buffer_desc.MiscFlags = 0;
	instance_buffer_desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA instance_data;
	instance_data.pSysMem = instances.data();
	instance_data.SysMemPitch = 0;
	instance_data.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&instance_buffer_desc, &instance_data, &instance_buffer);
	if (FAILED(result))
	{
		throw;
	}
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

dx::XMFLOAT3 World::triangle_intersection(dx::XMVECTOR from, dx::XMVECTOR to)
{
	dx::XMVECTOR direction = dx::XMVectorSubtract(to, from);
	std::array<dx::XMFLOAT3, 3> triangle;

	for (int i = 0; i < indices.size(); i += 3)
	{
		triangle[0] = vertices[indices[i]].position;
		triangle[1] = vertices[indices[i + 1]].position;
		triangle[2] = vertices[indices[i + 2]].position;

		std::array<float, 3> start_vec = {
			dx::XMVectorGetX(from),
			dx::XMVectorGetY(from),
			dx::XMVectorGetZ(from),
		};

		std::array<float, 3> end_vec = {
			dx::XMVectorGetX(to),
			dx::XMVectorGetY(to),
			dx::XMVectorGetZ(to),
		};

		std::array<float, 3> dir_vec = {
			dx::XMVectorGetX(direction),
			dx::XMVectorGetY(direction),
			dx::XMVectorGetZ(direction),
		};

		std::array<float, 3> edge1;
		std::array<float, 3> edge2;

		// Calculate the two edges from the three points given.
		edge1[0] = triangle[1].x - triangle[0].x;
		edge1[1] = triangle[1].y - triangle[0].y;
		edge1[2] = triangle[1].z - triangle[0].z;

		edge2[0] = triangle[2].x - triangle[0].x;
		edge2[1] = triangle[2].y - triangle[0].y;
		edge2[2] = triangle[2].z - triangle[0].z;

		std::array<float, 3> normal =
		{
			(edge1[1] * edge2[2]) - (edge1[2] * edge2[1]),
			(edge1[2] * edge2[0]) - (edge1[0] * edge2[2]),
			(edge1[0] * edge2[1]) - (edge1[1] * edge2[0]),
		};

		float magnitude = (float)sqrt((normal[0] * normal[0]) + (normal[1] * normal[1]) + (normal[2] * normal[2]));
		if (fabsf(magnitude) < 0.001f)
		{
			continue;
		}
		normal[0] = normal[0] / magnitude;
		normal[1] = normal[1] / magnitude;
		normal[2] = normal[2] / magnitude;

		// Find the distance from the origin to the plane.
		float D = ((-normal[0] * triangle[0].x) + (-normal[1] * triangle[0].y) + (-normal[2] * triangle[0].z));

		// Get the denominator of the equation.
		float denominator = ((normal[0] * dir_vec[0]) + (normal[1] * dir_vec[1]) + (normal[2] * dir_vec[2]));

		// Make sure the result doesn't get too close to zero to prevent divide by zero.
		if (fabs(denominator) < 0.1f)
		{
			continue;
		}

		// Get the numerator of the equation.
		float numerator = -1.0f * (((normal[0] * start_vec[0]) + (normal[1] * start_vec[1]) + (normal[2] * start_vec[2])) + D);

		// Calculate where we intersect the triangle.
		float t = numerator / denominator;

		// Find the intersection vector.
		std::array<float, 3> Q = {
			start_vec[0] + (dir_vec[0] * t),
			start_vec[1] + (dir_vec[1] * t),
			start_vec[2] + (dir_vec[2] * t),
		};

		// Find the three edges of the triangle.
		std::array<float, 3> e1 = {
			triangle[1].x - triangle[0].x,
			triangle[1].y - triangle[0].y,
			triangle[1].z - triangle[0].z,
		};

		std::array<float, 3> e2 = {
			triangle[2].x - triangle[1].x,
			triangle[2].y - triangle[1].y,
			triangle[2].z - triangle[1].z,
		};

		std::array<float, 3> e3 = {
			triangle[0].x - triangle[2].x,
			triangle[0].y - triangle[2].y,
			triangle[0].z - triangle[2].z,
		};

		// Calculate the normal for the first edge.
		std::array<float, 3> edge_normals = {
			(e1[1] * normal[2]) - (e1[2] * normal[1]),
			(e1[2] * normal[0]) - (e1[0] * normal[2]),
			(e1[0] * normal[1]) - (e1[1] * normal[0]),
		};

		// Calculate the determinant to see if it is on the inside, outside, or directly on the edge.
		std::array<float, 3> temp = {
			Q[0] - triangle[0].x,
			Q[1] - triangle[0].y,
			Q[2] - triangle[0].z,
		};

		float determinant = ((edge_normals[0] * temp[0]) + (edge_normals[1] * temp[1]) + (edge_normals[2] * temp[2]));

		// Check if it is outside.
		if (determinant > 0.000001f)
		{
			continue;
		}

		// Calculate the normal for the second edge.
		edge_normals[0] = (e2[1] * normal[2]) - (e2[2] * normal[1]);
		edge_normals[1] = (e2[2] * normal[0]) - (e2[0] * normal[2]);
		edge_normals[2] = (e2[0] * normal[1]) - (e2[1] * normal[0]);

		// Calculate the determinant to see if it is on the inside, outside, or directly on the edge.
		temp[0] = Q[0] - triangle[1].x;
		temp[1] = Q[1] - triangle[1].y;
		temp[2] = Q[2] - triangle[1].z;

		determinant = ((edge_normals[0] * temp[0]) + (edge_normals[1] * temp[1]) + (edge_normals[2] * temp[2]));

		// Check if it is outside.
		if (determinant > 0.000001f)
		{
			continue;
		}

		// Calculate the normal for the third edge.
		edge_normals[0] = (e3[1] * normal[2]) - (e3[2] * normal[1]);
		edge_normals[1] = (e3[2] * normal[0]) - (e3[0] * normal[2]);
		edge_normals[2] = (e3[0] * normal[1]) - (e3[1] * normal[0]);

		// Calculate the determinant to see if it is on the inside, outside, or directly on the edge.
		temp[0] = Q[0] - triangle[2].x;
		temp[1] = Q[1] - triangle[2].y;
		temp[2] = Q[2] - triangle[2].z;

		determinant = ((edge_normals[0] * temp[0]) + (edge_normals[1] * temp[1]) + (edge_normals[2] * temp[2]));

		// Check if it is outside.
		if (determinant > 0.000001f)
		{
			continue;
		}

		// Now we have our intersection position.
		dx::XMFLOAT3 intersection_pos = { Q[0], Q[1], Q[2] };

		//std::cout << dx::XMVectorGetX(from) << ", " << dx::XMVectorGetY(from) << ", " << dx::XMVectorGetZ(from) << "\n";
		if (!isnan(intersection_pos.x))
		{
			return intersection_pos;
		}
	}

	return { std::nanf(""), std::nanf(""), std::nanf("") };
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
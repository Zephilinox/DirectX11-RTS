#pragma once

//STD
#include <memory>

//SELF
#include "Camera.hpp"
#include "Direct3D.hpp"
#include "Input.hpp"
#include "ColourShader.hpp"
#include "Model.hpp"

class World
{
public:
	World(ID3D11Device* device);
	~World();

	bool update(Input* input, float dt);
	bool draw(ID3D11DeviceContext* device_context);

	int get_index_count();

//private:
	struct Vertex
	{
		dx::XMFLOAT3 position;
		dx::XMFLOAT4 colour;
	};

	struct Instance
	{
		dx::XMFLOAT3 position;
		dx::XMFLOAT4 colour;
	};

	ID3D11Buffer* vertex_buffer;
	ID3D11Buffer* index_buffer;
	ID3D11Buffer* instance_buffer;
	int vertex_count;
	int index_count;
	int instance_count;
};
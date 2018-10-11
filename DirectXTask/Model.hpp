#pragma once

//STD
#include <vector>

//LIBS
#include <d3d11.h>
#include <directxmath.h>
namespace dx = DirectX;

//SELF
#include "Direct3D.hpp"

class Model
{
public:
	struct Instance
	{
		dx::XMFLOAT3 position;
		dx::XMFLOAT3 rotation;
		dx::XMFLOAT3 scale;
		dx::XMFLOAT4 colour;
	};

	struct Vertex
	{
		dx::XMFLOAT3 position;
		dx::XMFLOAT4 colour;
		dx::XMFLOAT3 normal;
	};

	Model(ID3D11Device* device);
	~Model();

	bool load_from_file(std::string filename, int filetype);

	void render(ID3D11DeviceContext* device_context);

	int get_vertex_count();
	int get_instance_count();
	int get_index_count();

	void create_instance_buffer(ID3D11Device* device, int max_instances);
	void update_instance_buffer(ID3D11Device* device, ID3D11DeviceContext* device_context, std::vector<Instance>& instances);

private:

	struct ModelData
	{
		float x;
		float y;
		float z;
		float tu;
		float tv;
		float nx;
		float ny;
		float nz;
	};

	std::vector<ModelData> model_data;
	D3DRAII<ID3D11Buffer> vertex_buffer;
	D3DRAII<ID3D11Buffer> index_buffer;
	D3DRAII<ID3D11Buffer> instance_buffer;
	int vertex_count = 0;
	int index_count = 0;
	int instance_count = 0;
	int max_instance_count = 0;
};
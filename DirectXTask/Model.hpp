#pragma once

//STD
#include <vector>

//LIBS
#include <d3d11.h>
#include <directxmath.h>
namespace dx = DirectX;

class Model
{
public:
	struct InstanceType
	{
		dx::XMFLOAT3 position;
		dx::XMFLOAT4 colour;
	};

	Model(ID3D11Device* device);
	~Model();

	void render(ID3D11DeviceContext* device_context);

	int get_vertex_count();
	int get_instance_count();
	int get_index_count();

	void create_instances(ID3D11Device* device, float x, float y, float z);
	void update_instances(ID3D11DeviceContext* device_context, std::vector<InstanceType> instances);

private:
	struct VertexType
	{
		dx::XMFLOAT3 position;
		dx::XMFLOAT4 colour;
	};

	void stop_buffers();
	void render_buffers(ID3D11DeviceContext* device_context);

	//Do not wrap, need to call release() manually
	ID3D11Buffer* vertex_buffer;
	ID3D11Buffer* index_buffer;
	ID3D11Buffer* instance_buffer;
	int vertex_count = 0;
	int index_count = 0;
	int instance_count = 0;
};
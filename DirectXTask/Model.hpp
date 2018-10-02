#pragma once

//LIBS
#include <d3d11.h>
#include <directxmath.h>
namespace dx = DirectX;

class Model
{
public:
	Model(ID3D11Device* device);
	~Model();

	void render(ID3D11DeviceContext* device_context);

	int get_index_count();

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
	int vertex_count = 0;
	int index_count = 0;
};
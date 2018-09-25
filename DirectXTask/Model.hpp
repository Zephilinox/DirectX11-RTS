#pragma once

//LIBS
#include <d3d11.h>
#include <directxmath.h>
namespace dx = DirectX;

class Model
{
public:
	bool init(ID3D11Device* device);
	void stop();
	void render(ID3D11DeviceContext* device_context);

	int get_index_count();

private:
	struct VertexType
	{
		dx::XMFLOAT3 position;
		dx::XMFLOAT4 colour;
	};

	bool init_buffers(ID3D11Device* device);
	void stop_buffers();
	void render_buffers(ID3D11DeviceContext* device_context);

	ID3D11Buffer* vertex_buffer = nullptr;
	ID3D11Buffer* index_buffer = nullptr;
	int vertex_count = 0;
	int index_count = 0;
};
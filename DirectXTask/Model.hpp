#pragma once

//STD
#include <vector>

//LIBS
#include <d3d11.h>
#include <directxmath.h>
namespace dx = DirectX;

//SELF
#include "Direct3D.hpp"
#include "ColourShader.hpp"
#include "Texture.hpp"

class Model
{
public:
	Model(ID3D11Device* device, ID3D11DeviceContext* device_context, std::string filename);

	void render(ID3D11DeviceContext* device_context);

	int get_vertex_count();
	int get_instance_count();
	int get_index_count();

	void create_instance_buffer(ID3D11Device* device, int max_instances);
	void update_instance_buffer(ID3D11Device* device, ID3D11DeviceContext* device_context, std::vector<ColourShader::Instance>& instances);
	ID3D11ShaderResourceView* get_texture();

//private:
	bool load_from_file(std::string filename, int filetype);

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

	std::unique_ptr<Texture> texture;
};
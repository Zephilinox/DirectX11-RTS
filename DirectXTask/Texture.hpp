#pragma once

//STD
#include <string>
#include <vector>

//LIBS
#include <d3d11.h>

//SELF
#include "Direct3D.hpp"

class Texture
{
public:
	Texture(ID3D11Device* device, ID3D11DeviceContext* device_context, std::string path);

	ID3D11ShaderResourceView* get_texture_view();

private:
	struct TargaHeader
	{
		unsigned char data1[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;
	};

	bool load_targa(std::string path, int& width, int& height);

	std::vector<unsigned char> targa_data;
	D3DRAII<ID3D11Texture2D> texture;
	D3DRAII<ID3D11ShaderResourceView> texture_view;
};
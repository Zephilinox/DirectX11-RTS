#include "Texture.hpp"

Texture::Texture(ID3D11Device* device, ID3D11DeviceContext* device_context, std::string path)
{
	int width = 0;
	int height = 0;

	HRESULT result = load_targa(path, width, height);
	if (!result)
	{
		throw;
	}

	D3D11_TEXTURE2D_DESC texture_desc;
	texture_desc.Width = width;
	texture_desc.Height = height;
	texture_desc.MipLevels = 0;
	texture_desc.ArraySize = 1;
	texture_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texture_desc.SampleDesc.Count = 1;
	texture_desc.SampleDesc.Quality = 0;
	texture_desc.Usage = D3D11_USAGE_DEFAULT;
	texture_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	texture_desc.CPUAccessFlags = 0;
	texture_desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	result = device->CreateTexture2D(&texture_desc, nullptr, &texture.val);
	if (FAILED(result))
	{
		throw;
	}

	unsigned int row_pitch = (width * 4) * sizeof(unsigned char);
	device_context->UpdateSubresource(texture.val, 0, nullptr, targa_data.data(), row_pitch, 0);

	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
	srv_desc.Format = texture_desc.Format;
	srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv_desc.Texture2D.MostDetailedMip = 0;
	srv_desc.Texture2D.MipLevels = 1;

	result = device->CreateShaderResourceView(texture.val, &srv_desc, &texture_view.val);

	if (FAILED(result))
	{
		throw;
	}
}

ID3D11ShaderResourceView* Texture::get_texture_view()
{
	return texture_view.val;
}

bool Texture::load_targa(std::string path, int & width, int & height)
{
	FILE* file_ptr;
	int error = fopen_s(&file_ptr, path.c_str(), "rb");
	if (error != 0)
	{
		return false;
	}

	TargaHeader targa_header;
	unsigned int count = (unsigned int)fread(&targa_header, sizeof(TargaHeader), 1, file_ptr);
	if (count != 1)
	{
		return false;
	}

	height = (int)targa_header.height;
	width = (int)targa_header.width;
	int bpp = (int)targa_header.bpp;

	if (bpp != 32)
	{
		return false;
	}

	int image_size = width * height * 4;

	std::vector<unsigned char> targa_image;
	targa_image.resize(image_size);

	count = (unsigned int)fread(targa_image.data(), 1, image_size, file_ptr);
	if (count != image_size)
	{
		return false;
	}

	error = fclose(file_ptr);
	if (error != 0)
	{
		return false;
	}

	targa_data.resize(image_size);

	int index = 0;

	int k = (width * height * 4) - (width * 4);

	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			targa_data[index + 0] = targa_image[k + 2];  // Red.
			targa_data[index + 1] = targa_image[k + 1];  // Green.
			targa_data[index + 2] = targa_image[k + 0];  // Blue
			targa_data[index + 3] = targa_image[k + 3];  // Alpha

			k += 4;
			index += 4;
		}

		k -= (width * 8);
	}

	return true;
}

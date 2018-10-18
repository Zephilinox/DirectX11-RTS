#pragma once

//LIBS
#include <memory>

//SELF
#include "Direct3D.hpp"
#include "Camera.hpp"
#include "Model.hpp"
#include "ColourShader.hpp"
#include "World.hpp"
#include "Pathfinding.hpp"
#include "Entity.hpp"

class Graphics
{
public:
	Graphics(int width, int height, HWND window);

	void begin(float r, float g, float b, Camera* camera);
	void draw();
	void draw(ID3D11Buffer* vertex_buffer, ID3D11Buffer* index_buffer, ID3D11Buffer* instance_buffer, int vertex_count, int index_count, int instance_count);
	void end();

	static constexpr bool fullscreen = false;
	static constexpr bool vsync = false;
	static constexpr float screen_depth = 1000.0f;
	static constexpr float screen_near = 0.1f;

	std::unique_ptr<Direct3D> direct3d;
private:
	float time = 0;
	HWND window;

	std::unique_ptr<ColourShader> colour_shader;

	Input* input;
	dx::XMMATRIX view_matrix;
};
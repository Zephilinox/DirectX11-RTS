#include "Graphics.hpp"

//STD
#include <iostream>
#include <iomanip>

Graphics::Graphics(int width, int height, HWND window)
{
	this->window = window;

	try
	{
		direct3d = std::make_unique<Direct3D>(width, height, vsync, window, fullscreen, screen_depth, screen_near);
	}
	catch (...)
	{
		MessageBox(window, "Could not init Direct3D", "ERROR", MB_OK);
		throw;
	}

	camera = std::make_unique<Camera>();
	camera->set_pos(32.0f, 20.0f, 0.0f);
	camera->set_rot(30, 0, 0);

	try
	{
		sphere = std::make_unique<Model>(direct3d->get_device(), "Sphere.txt");
		cube = std::make_unique<Model>(direct3d->get_device(), "Cube2.txt");
	}
	catch (...)
	{
		MessageBoxW(window, L"Could not init Model", L"ERROR", MB_OK);
	}

	float deg2rad = 0.0174533f;

	sphere_instances.push_back({
		{ 30.0f - (std::cosf(time) * 4 + 4), 1.0f, 30.0f - (std::sinf(time) * 4 + 4) },
		{ 0.0f, 0.0f, 0.0f },
		{ 1.00f, 1.0f, 1.0f},
		{ 0, 1.0f, 0.8f, 1.0f }
	});

	sphere_instances.push_back({
		{ 32.0f, 1.0f, 32.0f },
		{ std::sinf(time) * 180.0f * deg2rad, std::sinf(time) * 180.0f * deg2rad, std::sinf(time) * 180.0f * deg2rad },
		{ 1.00f, 1.0f, 1.0f},
		{ 1.0f, 0.8f, 0.0f, 1.0f }
	});

	sphere_instances.push_back({
		{ 40.0f, 1.0f + std::cosf(time) * 3 + 3, 40.0f },
		{ 0.0f, std::sinf(time) * 180.0f * deg2rad, 0.0f },
		{ 1.00f, 1.0f, 1.0f},
		{ 1.0f, 0.4f, 0.4f, 1.0f }
	});

	sphere_instances.push_back({
		{ 44.0f, 1.0f, 44.0f },
		{ 0.0f, 0.0f, std::cosf(time) * 180.0f * deg2rad },
		{ 1.00f, 1.0f, 1.0f},
		{ 0.0f, 0.8f, 1.0f, 1.0f }
	});

	try
	{
		world = std::make_unique<World>(direct3d->get_device());
	}
	catch (...)
	{
		MessageBoxW(window, L"Could not init World", L"ERROR", MB_OK);
	}

	colour_shader = std::make_unique<ColourShader>(direct3d->get_device(), window);
}

bool Graphics::update(Input* input, float dt)
{
	time += dt;
	camera->update(input, dt);
	this->input = input;

	float deg2rad = 0.0174533f;

	if (input->is_key_down('C'))
	{
		for (int i = 0; i < 100; ++i)
		{
			float rand_x = (rand() % 51200) / 100.0f;
			rand_x -= 256 - 64;
			float rand_y = (rand() % 51200) / 100.0f + 32.0f;
			float rand_z = (rand() % 51200) / 100.0f;
			rand_z -= 256 - 64;

			sphere_instances.push_back({
				{ rand_x, rand_y, rand_z},
				{ 0.0f, 0.0f, 0.0f },
				{ 1.0f, 1.0f, 1.0f },
				{ (std::sin(time * 10.0f) + 1.0f) / 2.0f, 0.2f, 0.5f, 1.0f }
			});
		}

		std::cout.imbue(std::locale(""));
		std::cout << "\nInstances: " << sphere_instances.size() << "\n";
		int instances_mb = static_cast<int>((sphere_instances.size() * sizeof(Model::Instance)) / 1024.0f / 1024.0f);
		std::cout << "Instances Memory: " << instances_mb << "MB\n";
		std::cout << "Vertices: " << sphere_instances.size() * sphere->get_vertex_count() << "\n";
	}

	static float rot = std::cosf(time) * 180.0f * deg2rad;
	rot += 1.0f * dt;
	for (auto& instance : sphere_instances)
	{
		instance.rotation.x = rot;
		instance.rotation.y = rot;
		instance.rotation.z = rot;
	}

	for (auto& instance : cube_instances)
	{
		instance.rotation.x = rot;
		instance.rotation.y = rot;
		instance.rotation.z = rot;
	}

	POINT pos;
	if (GetCursorPos(&pos))
	{
		if (ScreenToClient(window, &pos))
		{
			camera->draw();
			dx::XMMATRIX world_matrix = direct3d->get_world_matrix();
			dx::XMMATRIX view_matrix = camera->get_view_matrix();
			dx::XMMATRIX projection_matrix = direct3d->get_projection_matrix();

			//float rel_pos_x = pos.x / 1280.0f;
			//float rel_pos_y = pos.y / 720.0f;
			//rel_pos_x *= 2;
			//rel_pos_x -= 1.0f;
			//rel_pos_y *= 2;
			//rel_pos_y -= 1.0f;
			//std::cout << pos.x << "," << pos.y << "\n";
			//std::cout << rel_pos_x << "," << rel_pos_y << "\n";
			dx::XMFLOAT3 near_pos{ static_cast<float>(pos.x), static_cast<float>(pos.y), 0.0f };
			dx::XMFLOAT3 far_pos{ static_cast<float>(pos.x), static_cast<float>(pos.y), 1.0f };
			dx::XMVECTOR near_pos_vec = dx::XMLoadFloat3(&near_pos);
			dx::XMVECTOR far_pos_vec = dx::XMLoadFloat3(&far_pos);

			near_pos_vec = dx::XMVector3Unproject(
				near_pos_vec,
				0, 0,
				1280.0f, 720.0f,
				0.0f, 1.0f,
				projection_matrix,
				view_matrix,
				world_matrix);

			far_pos_vec = dx::XMVector3Unproject(
				far_pos_vec,
				0, 0,
				1280.0f, 720.0f,
				0.0f, 1.0f,
				projection_matrix,
				view_matrix,
				world_matrix);

			auto direction = dx::XMVectorSubtract(near_pos_vec, far_pos_vec);

			if ((GetKeyState(VK_RBUTTON) & 0x100) != 0)
			{
				dx::XMFLOAT3 paint_pos{ static_cast<float>(pos.x), static_cast<float>(pos.y), 0.995f };
				dx::XMVECTOR paint_pos_vec = dx::XMLoadFloat3(&paint_pos);
				paint_pos_vec = dx::XMVector3Unproject(
					paint_pos_vec,
					0, 0,
					1280.0f, 720.0f,
					0.0f, 1.0f,
					projection_matrix,
					view_matrix,
					world_matrix);

				dx::XMFLOAT3 pos;
				dx::XMStoreFloat3(&pos, paint_pos_vec);
				cube_instances.push_back({
					pos,
					{0, 0, 0},
					{1.0f, 1.0f, 1.0f},
					{0.8f, 0.2f, 0.2f, 1.0f},
				});
			}

			if ((GetKeyState(VK_LBUTTON) & 0x100) != 0)
			{
				std::array<dx::XMFLOAT3, 3> triangle;
				int nan_count = 0;
				int fail_count = 0;
				for (int i = 0; i < world->indices.size(); i += 3)
				{
					triangle[0] = world->vertices[world->indices[i]].position;
					triangle[1] = world->vertices[world->indices[i+1]].position;
					triangle[2] = world->vertices[world->indices[i+2]].position;

					std::array<float, 3> start_vec = {
						dx::XMVectorGetX(near_pos_vec),
						dx::XMVectorGetY(near_pos_vec),
						dx::XMVectorGetZ(near_pos_vec),
					};

					std::array<float, 3> end_vec = {
						dx::XMVectorGetX(far_pos_vec),
						dx::XMVectorGetY(far_pos_vec),
						dx::XMVectorGetZ(far_pos_vec),
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
						fail_count++;
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
						fail_count++;
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
						fail_count++;
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
						fail_count++;
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
						fail_count++;
						continue;
					}

					// Now we have our intersection position.
					dx::XMFLOAT3 intersection_pos = { Q[0], Q[1], Q[2] };

					//std::cout << dx::XMVectorGetX(near_pos_vec) << ", " << dx::XMVectorGetY(near_pos_vec) << ", " << dx::XMVectorGetZ(near_pos_vec) << "\n";
					if (!isnan(intersection_pos.x))
					{
						std::cout << intersection_pos.x << ", " << intersection_pos.y << ", " << intersection_pos.z << "\n";
						dx::XMFLOAT3 pos;
						dx::XMStoreFloat3(&pos, far_pos_vec);
						cube_instances.push_back({
							intersection_pos,
							{0, 0, 0},
							{0.2f, 0.2f, 0.2f},
							{0.2f, 0.2f, 0.2f, 1.0f},
						});
						break;
					}
					else
					{
						nan_count++;
					}
				}

				std::cout << "NaN: " << nan_count << "\n";
				std::cout << "Fail: " << fail_count << "\n";
				std::cout << "Triangles: " << world->vertex_count / 3 << "\n";
			}
		}
	}
	return false;
}

bool Graphics::draw()
{
	float deg2rad = 0.0174533f;
	float rot = std::cosf(time) * 18.0f * deg2rad;
	direct3d->begin(0.9f, 0.9f, 0.85f, 1.0f);
	dx::XMFLOAT3 light_direction = { -0.3f, rot, rot };
	dx::XMFLOAT4 diffuse_colour = { 1.0f, 1.0f, 1.0f, 1.0f };

	camera->draw();
	dx::XMMATRIX world_matrix = direct3d->get_world_matrix();
	dx::XMMATRIX view_matrix =	camera->get_view_matrix();
	dx::XMMATRIX projection_matrix = direct3d->get_projection_matrix();
	
	direct3d->set_wireframe(false);
	sphere->update_instance_buffer(direct3d->get_device(), direct3d->get_device_context(), sphere_instances);
	sphere->render(direct3d->get_device_context());
	
	bool result = colour_shader->render(direct3d->get_device_context(), sphere->get_index_count(), sphere->get_vertex_count(), sphere->get_instance_count(), world_matrix, view_matrix, projection_matrix, light_direction, diffuse_colour);
	if (!result)
	{
		return false;
	}

	direct3d->set_wireframe(false);
	cube->update_instance_buffer(direct3d->get_device(), direct3d->get_device_context(), cube_instances);
	cube->render(direct3d->get_device_context());

	result = colour_shader->render(direct3d->get_device_context(), cube->get_index_count(), cube->get_vertex_count(), cube->get_instance_count(), world_matrix, view_matrix, projection_matrix, light_direction, diffuse_colour);
	if (!result)
	{
		return false;
	}

	world_matrix = dx::XMMatrixIdentity();
	
	direct3d->set_wireframe(false);
	world->draw(direct3d->get_device_context());
	result = colour_shader->render(direct3d->get_device_context(), world->get_index_count(), world->get_index_count(), 1, world_matrix, view_matrix, projection_matrix, light_direction, diffuse_colour);
	if (!result)
	{
		return false;
	}

	direct3d->end();

	return false;
}

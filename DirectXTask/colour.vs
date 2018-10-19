cbuffer MatrixBuffer
{
	matrix world;
	matrix view;
	matrix projection;
};

struct Vertex
{
	float4 position: POSITION0;
	float2 tex: TEXCOORD0;
	float3 normal: NORMAL;
};

struct Instance
{
	float3 position: POSITION1;
	float3 rotation: POSITION2;
	float3 scale: POSITION3;
	float4 colour: COLOR0;
};

struct Pixel
{
	float4 position: SV_POSITION;
	float4 colour: COLOR;
	float2 tex: TEXCOORD;
	float3 normal: NORMAL;
};

Pixel ColourVertexShader(Vertex vertex, Instance instance)
{
	matrix scale = {
	instance.scale.x, 0, 0, 0,
	0, instance.scale.y, 0, 0,
	0, 0, instance.scale.z, 0,
	0, 0, 0, 1
	};
	
	matrix rotation_x = {
	1, 0, 0, 0,
	0, cos(instance.rotation.x), -sin(instance.rotation.x), 0,
	0, sin(instance.rotation.x), cos(instance.rotation.x), 0,
	0, 0, 0, 1
	};

	matrix rotation_y = {
	cos(instance.rotation.y), 0, sin(instance.rotation.y), 0,
	0, 1, 0, 0,
	-sin(instance.rotation.y), 0, cos(instance.rotation.y), 0,
	0, 0, 0, 1
	};

	matrix rotation_z = {
	cos(instance.rotation.z), -sin(instance.rotation.z), 0, 0,
	sin(instance.rotation.z), cos(instance.rotation.z), 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
	};

	matrix position = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	instance.position.x, instance.position.y, instance.position.z, 1
	};

	vertex.position.w = 1.0f;

	matrix transform = scale;
	transform = mul(transform, rotation_x);
	transform = mul(transform, rotation_y);
	transform = mul(transform, rotation_y);
	transform = mul(transform, position);
	
	Pixel output;
	output.position = mul(vertex.position, transform);
	output.position = mul(output.position, view);
	output.position = mul(output.position, projection);

	output.colour = instance.colour;
	output.tex = vertex.tex;

	output.normal = mul(vertex.normal, (float3x3)transform);
	output.normal = normalize(output.normal);
	
	return output;
}
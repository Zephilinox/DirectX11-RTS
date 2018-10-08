cbuffer MatrixBuffer
{
	matrix world;
	matrix view;
	matrix projection;
};

struct Vertex
{
	float4 position: POSITION0;
	float4 colour: COLOR0;
};

struct Instance
{
	float3 position: POSITION1;
	float3 rotation: POSITION2;
	float3 scale: POSITION3;
	float4 colour: COLOR1;
};

struct Pixel
{
	float4 position: SV_POSITION;
	float4 colour : COLOR;
};

Pixel ColourVertexShader(Vertex vertex, Instance instance)
{
	matrix position = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	instance.position.x, instance.position.y, instance.position.z, 1
	};

	matrix rotation = {
	cos(instance.rotation.y) * cos(instance.rotation.z),	sin(instance.rotation.z),								sin(instance.rotation.y), 0,
	-sin(instance.rotation.z),								cos(instance.rotation.x) * cos(instance.rotation.z),	-sin(instance.rotation.x), 0,
	-sin(instance.rotation.y),								sin(instance.rotation.x),								cos(instance.rotation.x) * cos(instance.rotation.y), 0,
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

	matrix scale = {
	instance.scale.x, 0, 0, 0,
	0, instance.scale.y, 0, 0,
	0, 0, instance.scale.z, 0,
	0, 0, 0, 1
	};

	matrix transform ={
		instance.scale.x * cos(instance.rotation.y) * cos(instance.rotation.z),	 sin(instance.rotation.z),													sin(instance.rotation.y),												0,
		-sin(instance.rotation.z),												 instance.scale.y * cos(instance.rotation.x) * cos(instance.rotation.z),	-sin(instance.rotation.x),												0,
		-sin(instance.rotation.y),												 sin(instance.rotation.x),													instance.scale.z * cos(instance.rotation.x) * cos(instance.rotation.y), 0,
		instance.position.x,													 instance.position.y,														instance.position.z,													1
	};

	vertex.position.w = 1.0f;

	Pixel output;
	output.position = mul(vertex.position, scale);
	output.position = mul(output.position, rotation_x);
	output.position = mul(output.position, rotation_y);
	output.position = mul(output.position, rotation_z);
	output.position = mul(output.position, position);
	output.position = mul(output.position, view);
	output.position = mul(output.position, projection);

	output.colour = vertex.colour * instance.colour;

	return output;
}
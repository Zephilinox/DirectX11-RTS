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
	matrix transform ={
		instance.scale.x * cos(instance.rotation.y) * cos(instance.rotation.z),	 sin(instance.rotation.z),													sin(instance.rotation.y),												0,
		-sin(instance.rotation.z),												 instance.scale.y * cos(instance.rotation.x) * cos(instance.rotation.z),	-sin(instance.rotation.x),												0,
		-sin(instance.rotation.y),												 sin(instance.rotation.x),													instance.scale.z * cos(instance.rotation.x) * cos(instance.rotation.y), 0,
		instance.position.x,													 instance.position.y,														instance.position.z,													1
	};

	vertex.position.w = 1.0f;

	Pixel output;
	output.position = mul(vertex.position, transform);
	output.position = mul(output.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, projection);

	output.colour = vertex.colour * instance.colour;

	return output;
}
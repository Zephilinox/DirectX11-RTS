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
    vertex.position.x += instance.position.x + instance.rotation.x;
    vertex.position.y += instance.position.y;
    vertex.position.z += instance.position.z;
	vertex.position.w = 1.0f;

	Pixel output;
	output.position = mul(vertex.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, projection);

	output.colour = vertex.colour * instance.colour;

	return output;
}
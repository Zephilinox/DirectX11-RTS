cbuffer MatrixBuffer
{
	matrix world;
	matrix view;
	matrix projection;
};

struct VertexInputType
{
	float4 position: POSITION;
	float4 colour: COLOR;
};

struct InstanceInputType
{
	float3 position: POSITION1;
	float4 colour: COLOR1;
};

struct PixelInputType
{
	float4 position: SV_POSITION;
	float4 colour : COLOR;
};

PixelInputType ColourVertexShader(VertexInputType input, InstanceInputType instance)
{
	input.position.w = 1.0f;
    input.position.x += instance.position.x;
    input.position.y += instance.position.y;
    input.position.z += instance.position.z;

	PixelInputType output;
	output.position = mul(input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, projection);

	output.colour = input.colour * instance.colour;

	return output;
}
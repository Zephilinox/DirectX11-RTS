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
	float3 instance_position: TEXCOORD0;
};

struct PixelInputType
{
	float4 position: SV_POSITION;
	float4 colour : COLOR;
};

PixelInputType ColourVertexShader(VertexInputType input)
{
	input.position.w = 1.0f;
    input.position.x += input.instance_position.x;
    input.position.y += input.instance_position.y;
    input.position.z += input.instance_position.z;

	PixelInputType output;
	output.position = mul(input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, projection);

	output.colour = input.colour;

	return output;
}
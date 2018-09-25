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

struct PixelInputType
{
	float4 position: SV_POSITION;
	float4 colour : COLOR;
};

PixelInputType ColourVertexShader(VertexInputType input)
{
	input.position.w = 1.0f;

	PixelInputType output;
	output.position = mul(input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, projection);

	output.colour = input.colour;

	return output;
}
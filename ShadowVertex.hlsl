
struct VS_IN //Input to vertex shader. Must match struct of vertex buffer.
{
	float3 Pos : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};

struct VS_OUT //Output. Must match input of pixel shader. 
{
	float4 Pos : SV_POSITION;
};


cbuffer matrixBuffer:register(b0) {
	matrix viewMatrix;
	matrix projectionMatrix;

	matrix lightViewMatrix;
	matrix lightProjectionMatrix;
};

cbuffer worldBuffer:register(b1) { 
	matrix worldMatrix;
};


VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.Pos = mul(input.Pos, mul(worldMatrix, mul(lightViewMatrix, lightProjectionMatrix)));

	return output;
}
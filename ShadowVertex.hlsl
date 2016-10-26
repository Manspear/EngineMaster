
struct VS_IN //Input to vertex shader. Must match struct of vertex buffer.
{
	float3 Pos : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};

struct VS_OUT //Output. Must match input of pixel shader. 
{
	float4 Pos : SV_POSITION;
	float2 uv : TEXCOORD;
	float3 normal : TEXCOORD3;

	float4 lightViewPos: TEXCOORD1;
	float3 lightPos : TEXCOORD2;
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

	float3 lightPos = float3(4, 3, -3);
	float4 worldPos;
	float4 input4 = float4(input.Pos, 1.0f);

	output.Pos = mul(input4, worldMatrix);
	output.Pos = mul(output.Pos, viewMatrix);
	output.Pos = mul(output.Pos, projectionMatrix);

	output.lightViewPos = mul(input4, worldMatrix);
	output.lightViewPos = mul(output.lightViewPos, lightViewMatrix);
	output.lightViewPos = mul(output.lightViewPos, lightProjectionMatrix);


	output.uv = input.uv;
	
	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	//calc vertex world pos
	worldPos = mul(input4, worldMatrix);
	
	//make vector between light and vertex pos
	output.lightPos = lightPos.xyz - worldPos.xyz;
	output.lightPos = normalize(output.lightPos);


	return output;
}
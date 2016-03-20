struct VS_IN //Input to vertex shader. Must match struct of vertex buffer.
{
	float3 Pos : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
	float3 bsPos : POSITION1;
	float3 bsn : NORMAL1;
	float2 bsuv : TEXCOORD1;
};

cbuffer weightBuff:register(b0)
{
	float weight;
	float padding0;
	float padding1;
	float padding2;
};

struct VS_OUT //Output. Must match input of pixel shader. 
{
	float4 Pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};
//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------



VS_OUT VS_main(VS_IN input)
{

	VS_OUT output = (VS_OUT)0;

	float3 positionDiff = input.bsPos - input.Pos;

	float3 position = input.Pos + weight * positionDiff;
	output.Pos = float4(position, 1);

	output.normal = input.normal;
	output.uv = input.uv;

	return output;
}
struct VS_IN //Input to vertex shader. Must match struct of vertex buffer.
{
	float3 Pos : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
	float3 bsPos : POSITION1;
	float3 bsn : NORMAL1;
	float2 bsuv : TEXCOORD1;
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

	VS_OUT output = (VS_OUT)0; //Hmm... What is this? Am I zeroing-out my output?


	float3 position = input.Pos;

	position = 1.0 * input.bsPos; //typecasting float3 to float4. The 1 is the w-part of the 4x1 point
	output.Pos = float4(position, 1);

	output.normal = input.bsn;
	output.uv = input.uv;

	return output;
}
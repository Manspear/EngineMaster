struct VS_IN //Input to vertex shader. Must match struct of vertex buffer.
{
	float3 Pos : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
	float4 lightPos : TEXCOORD1;
};

struct VS_OUT //Output. Must match input of pixel shader. 
{
	float4 Pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
	float4 lightPos : TEXCOORD1;
};
//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------



VS_OUT VS_main(VS_IN input)
{

	VS_OUT output = (VS_OUT)0;

	output.Pos = float4(input.Pos, 1); //typecasting float3 to float4. The 1 is the w-part of the 4x1 point

	output.normal = input.normal;
	output.uv = input.uv;

	//This must be multiplied with world * lightViewProj in the geometry shader
	output.lightPos = float4(input.Pos, 1);

	return output;
}
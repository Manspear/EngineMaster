struct VS_IN //Input to vertex shader. Must match struct of vertex buffer.
{
	float3 Pos : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
	uint4 influences : BLENDINDICES;
	float4 weights : BLENDWEIGHT;
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
cbuffer jointBuffer:register(b0) { //Gotten from the GModel class.
	matrix jointTransform[36];
}


VS_OUT VSSkeletal_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0; //Hmm... What is this? Am I zeroing-out my output?

	output.Pos = float4(0.f, 0.f, 0.f, 1.f); //typecasting float3 to float4. The 1 is the w-part of the 4x1 point
	output.normal = float4(0.f, 0.f, 0.f, 1.f);

	for (int i = 0; i < 4; i++)
	{
		
		output.Pos += mul(jointTransform[input.influences[i]], float4(input.Pos, 1.0)) * input.weights[i];
		output.normal += mul(jointTransform[input.influences[i]], float4(input.normal, 1.0) * input.weights[i]).xyz;
	}
	//The worldmatrix is already calculated on the CPU into the jointTransform[36], so it shouldn't be needed here.

	output.uv = input.uv;

	return output;
}
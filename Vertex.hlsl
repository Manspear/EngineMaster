struct VS_IN //Input to vertex shader. Must match struct of vertex buffer.
{
	float3 Pos : POSITION;
	float2 uv : TEXCOORD;
};

struct VS_OUT //Output. Must match input of pixel shader. 
{
	float4 Pos : SV_POSITION;
	float2 uv : TEXCOORD;
};
//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------

//cbuffer matrixBuffer:register(b0) { 
//	//register b0 says that this buffer takes up slot 0 in the 
//	//"buffer array" belonging to the Vertex Shader. So if more
//	//buffers are to be added to the Vertex Buffer, they must 
//	//be registered as b1, b2, etc...
//	matrix worldMatrix;
//	matrix viewMatrix;
//	matrix projectionMatrix;
//};

VS_OUT VS_main(VS_IN input)
{
	//mul is a multiplication function in HLSL. (slide in transformations)
	VS_OUT output = (VS_OUT)0; //Hmm... What is this? Am I zeroing-out my output?
	
	output.Pos = float4(input.Pos, 1); //typecasting float3 to float4. The 1 is the w-part of the 4x1 point
	
	//matrix transformMatrix; //When multiplying a point with a vector, you get a point. Multiplication of point with matrix, gets you  a new point.
	//transformMatrix = mul(worldMatrix, viewMatrix);
	//transformMatrix = mul(transformMatrix, projectionMatrix); //More expensive to multiply matrices with matrices. Cheaper matrix x vector.
	//output.Pos = mul(output.Pos, transformMatrix);
	
	output.uv = input.uv;

	return output;
}
struct VS_IN //Input to vertex shader. Must match struct of vertex buffer.
{
	float4 Pos : SV_Position;//Homogeneous clip space  /w för att få Normalised device space   utifrån kameran
	float3 normal : normal;
	float2 UV : TEXCOORD;
	float4 worldPosition : WORLDSPACE;//pixel, pixlarna på en triangel

	float3 tangent : TANGENT;
	float3 biTangent : BITANGENT;
	float4 camDir :WORLDSPACE2;

};
// gör en ny cbuffer med Material,  ambient diffuse specular, reflect
cbuffer matrixBuffer:register(b0) {
	//register b0 says that this buffer takes up slot 0 in the 
	//"buffer array" belonging to the Vertex Shader. So if more
	//buffers are to be added to the Vertex Buffer, they must 
	//be registered as b1, b2, etc...
	matrix viewMatrix;
	matrix projectionMatrix;

	float4 camPos;
	float4 camDir;
};

struct VS_OUT //Output. Must match input of pixel shader. 
{
	float4 Pos : SV_Position;//Homogeneous clip space  /w för att få Normalised device space   utifrån kameran
	float3 normal : normal;
	float2 UV : TEXCOORD;
	float4 worldPosition : WORLDSPACE;//pixel, pixlarna på en triangel
	bool gReflectionEnable : ballan;

	float3 tangent : TANGENT;
	float3 biTangent : BITANGENT;
	float4 camDir :WORLDSPACE2;

};
//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------



VS_OUT VS_main(VS_IN input)
{

	VS_OUT output = (VS_OUT)0; //Hmm... What is this? Am I zeroing-out my output?

	output.Pos = input.Pos; // typecasting float3 to float4. The 1 is the w-part of the 4x1 point

	output.normal = input.normal;
	output.UV = input.UV;

	return output;
}
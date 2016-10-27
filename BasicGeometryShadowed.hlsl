struct GSINPUT {
	float4 Pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 UV : TEXCOORD;
	float4 lightPos : TEXCOORD1;
};

struct GSOutput
{
	float4 Pos : SV_Position;
	float3 normal : NORMAL;
	float2 UV  : TEXCOORD;
	float4 worldPosition : WORLDSPACE;
	float4 lightPos : TEXCOORD1;

	float3 tangent : TANGENT;
	float3 biTanget : BITANGENT;
	float4 camPos : WORLDSPACE1;
	float4 camDir :WORLDSPACE2;
};

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

cbuffer worldBuffer:register(b1) { //Gotten from the GModel class.
	matrix worldMatrix;
};

cbuffer lightBuffer:register(b2)
{
	matrix lightViewMat;
	matrix lightProjectionMat;
};

[maxvertexcount(3)] //returns a maximum of x vertices


void GS_main(triangle GSINPUT input[3] : SV_POSITION, inout TriangleStream< GSOutput > output)
{
	GSOutput element = (GSOutput)0;

	matrix allMatrix = mul(mul(worldMatrix, viewMatrix), projectionMatrix);

	//Normal //still used in tangent bitangent
	float3 edge1 = input[1].Pos - input[0].Pos;
	float3 edge2 = input[2].Pos - input[0].Pos;
	float2 uvEdge1 = input[1].UV - input[0].UV;
	float2 uvEdge2 = input[2].UV - input[0].UV;

	float3 normal = normalize(cross(edge1, edge2));//will not be sent to pixel shader

												   //if (dot(camPos - input[0].Pos, normal) < 0 && dot(camPos - input[1].Pos, normal) < 0 && dot(camPos - input[2].Pos, normal) < 0)
												   //	normal = normal*-1.0f;


	float3 tangent = (uvEdge2[1] * edge1 - uvEdge1[1] * edge2) *(1 / (uvEdge1[0] * uvEdge2[1] - uvEdge2[0] * uvEdge1[1]));
	float3 biTangent = -cross(normal, tangent);
	normalize(tangent); normalize(biTangent);

	tangent = mul(float4(tangent, 1), worldMatrix).xyz;
	biTangent = mul(float4(biTangent, 1), worldMatrix).xyz;
	//End of Normal
	element.camPos = camPos;
	for (uint i = 0; i < 3; i++)
	{
		element.Pos = mul(input[i].Pos, allMatrix);
		element.UV = input[i].UV;
		element.normal = normalize(mul(float4(input[i].normal, 0), worldMatrix).xyz); //get the normal into worldspace

		element.worldPosition = mul(input[i].Pos, worldMatrix);

		element.lightPos = mul(element.lightPos, mul(worldMatrix, mul(lightViewMat, lightProjectionMat)));

		element.tangent = tangent;
		element.biTanget = biTangent;

		if (dot(camPos - element.worldPosition, normal) > 0)
			output.Append(element);
	}
	
	output.RestartStrip();
}


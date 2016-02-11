struct GSOutput
{
	float4 Pos : SV_Position;
	float3 normal : NORMAL;
	float2 UV  : TEXCOORD;
	float4 worldPosition : WORLDSPACE;

	float3 tangent : TANGENT;
	float3 biTanget : BITANGENT;
};

cbuffer matrixBuffer:register(b0) {
	//register b0 says that this buffer takes up slot 0 in the 
	//"buffer array" belonging to the Vertex Shader. So if more
	//buffers are to be added to the Vertex Buffer, they must 
	//be registered as b1, b2, etc...
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

[maxvertexcount(6)] //geometry shader will return X vertex


void GS_main(triangle GSOutput input[3] : SV_POSITION, inout TriangleStream< GSOutput > output)
{
	GSOutput element = (GSOutput)0;
	matrix allMatrix = mul(mul(worldMatrix, viewMatrix), projectionMatrix);

	//Normal
	float3 edge1 = input[1].Pos - input[0].Pos;
	float3 edge2 = input[2].Pos - input[0].Pos;
	
	
	
	
	float2 uvEdge1 = input[1].UV - input[0].UV;
	float2 uvEdge2 = input[2].UV - input[0].UV;

	float3 normal = normalize(cross(edge1, edge2));
	//float3 tangent;





	float3 tangent = (uvEdge2[1] * edge1 - uvEdge1[1] * edge2) *(1 / (uvEdge1[0] * uvEdge2[1] - uvEdge2[0] * uvEdge1[1]));
	
	
	
	
	
	//tangent[0] = (uvEdge1[1] * edge1[0] - uvEdge2[1] * edge2[0]) * (1.0f / (uvEdge1[0] * uvEdge2[1] - uvEdge2[0] * uvEdge1[1]));
	//tangent[1] = (uvEdge1[1] * edge1[1] - uvEdge2[1] * edge2[1]) * (1.0f / (uvEdge1[0] * uvEdge2[1] - uvEdge2[0] * uvEdge1[1]));
	//tangent[2] = (uvEdge1[1] * edge1[2] - uvEdge2[1] * edge2[2]) * (1.0f / (uvEdge1[0] * uvEdge2[1] - uvEdge2[0] * uvEdge1[1]));

	float3 biTangent = -cross(normal, tangent);
	
	//tangent = (2, 321231, 4);
	//biTangent = (1, 14, 245);

	normalize(tangent); normalize(biTangent); //This should be done before sending to GPU

	tangent = mul(float4(tangent, 1), worldMatrix).xyz;
	biTangent = mul(float4(biTangent, 1), worldMatrix).xyz;

	//End of Normal







	for (uint i = 0; i < 3; i++)
	{ //First loop
		element.Pos = mul(input[i].Pos, allMatrix);
		element.UV = input[i].UV;
		element.normal = mul(float4(normal, 0), worldMatrix).xyz; //get the normal into worldspace

		element.worldPosition = mul(input[i].Pos, worldMatrix);
		element.tangent = tangent;
		element.biTanget = biTangent;


		output.Append(element);

	}

	output.RestartStrip();
}


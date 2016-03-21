struct GSInput
{
    float4 Pos : SV_Position;
};

struct GSOutput
{
	float4 Pos : SV_Position;
	float3 normal : NORMAL;
	float2 UV  : TEXCOORD;
	float4 worldPosition : WORLDSPACE;
};

cbuffer matrixBuffer : register(b0)
{
	//register b0 says that this buffer takes up slot 0 in the 
	//"buffer array" belonging to the Vertex Shader. So if more
	//buffers are to be added to the Vertex Buffer, they must 
	//be registered as b1, b2, etc...
	matrix viewMatrix;
	matrix projectionMatrix;

	float4 camPos;
	float4 camDir;
};

cbuffer worldBuffer : register(b1)
{ //Gotten from the GModel class.
	matrix worldMatrix;
};

[maxvertexcount(4)]
void main(point GSOutput input[1] : SV_POSITION, inout TriangleStream< GSOutput > output)
{
	GSOutput element = (GSOutput)0;
	float4 verts[4];
	float2 uvs[4];

	float3 planeNormal = normalize(input[0].Pos - camPos);
	float3 upVector = float3(0.0f, 1.0f, 0.0f);
	float3 rightVector = normalize(cross(planeNormal, upVector));

	float width = 3.0f;
	float height = 3.0f;

	rightVector *= (width/2);
	upVector *= height;
	
	verts[0] = float4((input[0].Pos - rightVector), 1.0f);
	verts[1] = float4((input[0].Pos + rightVector), 1.0f);
	verts[2] = float4((input[0].Pos - rightVector + upVector), 1.0f);
	verts[3] = float4((input[0].Pos + rightVector + upVector), 1.0f);

	uvs[0] = float2(0.0f, 1.0f);
	uvs[1] = float2(1.0f, 1.0f);
	uvs[2] = float2(0.0f, 0.0f);
	uvs[3] = float2(1.0f, 0.0f);

	matrix allMatrix = mul(mul(worldMatrix, viewMatrix), projectionMatrix);

	float3 edge1 = verts[1] - input[0].Pos;
	float3 edge2 = verts[2] - input[0].Pos;

	float3 normal = normalize(cross(edge1, edge2));//will not be sent to pixel shader

	float3 norW = normalize(mul(float4(input[0].normal, 0), worldMatrix).xyz);

	
	for (uint i = 0; i < 4; i++)
	{
		element.Pos = mul(verts[i], allMatrix);
		element.UV = uvs[0];
		element.normal = norW; //get the normal into worldspace

		element.worldPosition = mul(verts[i], worldMatrix);

		if (dot(camPos - element.worldPosition, normal) > 0)
			output.Append(element);
	}

	output.RestartStrip();
}
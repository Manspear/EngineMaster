struct GSInput
{
    float4 Pos : SV_Position;
    float3 Color : COLOR;
};

struct GSOutput
{
	float4 Pos : SV_Position;
	float2 UV  : TEXCOORD;
	float4 worldPosition : WORLDSPACE;
    float3 Color : COLOR;
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
void main(point GSInput input[1] : SV_POSITION, inout TriangleStream< GSOutput > output)
{
	GSOutput element = (GSOutput)0;
	float4 verts[4];
	float2 uvs[4];

    matrix wv = mul(worldMatrix, viewMatrix);

    float size = 0.1f;

    //TRANSFORM ZE POSITION IN WOERLD VIEW
    float4 pos = mul(input[0].Pos, wv);

    //THEN DO MATH WITH X AND Y IN WOERLDVIEW BEFOER MULTIPLYING PROJECTION MATRIZ FOR BILLBOARDING ACTIONPOWER
    float2 v0 = pos.xy + float2(-0.5, -0.5) * size;
    verts[0] = mul(float4(v0, pos.zw), projectionMatrix);
    float2 v1 = pos.xy + float2(-0.5, 0.5) * size;
    verts[1] = mul(float4(v1, pos.zw), projectionMatrix);
    float2 v2 = pos.xy + float2(0.5, -0.5) * size;
    verts[2] = mul(float4(v2, pos.zw), projectionMatrix);
    float2 v3 = pos.xy + float2(0.5, 0.5) * size;
    verts[3] = mul(float4(v3, pos.zw), projectionMatrix);

	uvs[0] = float2(0.0f, 1.0f);
	uvs[1] = float2(1.0f, 1.0f);
	uvs[2] = float2(0.0f, 0.0f);
	uvs[3] = float2(1.0f, 0.0f);

	
	for (uint i = 0; i < 4; i++)
	{
        element.Pos = verts[i];
		element.UV = uvs[i];
        element.Color = input[0].Color;
		element.worldPosition = mul(verts[i], worldMatrix);

		output.Append(element);
	}

	output.RestartStrip();
}
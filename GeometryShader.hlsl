//Gotta remember to make a shader
//struct GSOutput
//{
//	float4 pos : SV_POSITION;
//};

struct GS_Input //Input. Must match output of vertex shader. 
{
	float4 Pos : SV_POSITION;
	float2 uv : TEXCOORD;
};

struct GS_Output //Output. Must match input of fragment shader. 
{
	float4 wPos : POSITION0;
	float4 Pos : SV_POSITION;
	float2 uv : UV;
	float4 normal : NORMAL;
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

[maxvertexcount(6)] //says geometry shader will shit out 6 vertices


void GS_main(
	triangle GS_Input input[3] : SV_POSITION, //Three vertices in list 
	inout TriangleStream< GS_Output > output //The output where they are reinterpreted as primitives
	)
{
	//Making matrix containing all transform-matrices
	matrix transformMatrix; 
	transformMatrix = mul(worldMatrix, viewMatrix);
	transformMatrix = mul(transformMatrix, projectionMatrix); //wvp

	//Calculating the normal of the face
	float4 triangleSide1 = input[1].Pos - input[0].Pos;
	float4 triangleSide2 = input[2].Pos - input[0].Pos;
	float4 normal = float4(cross((float3) triangleSide1, (float3) triangleSide2), 0);
	normal = normalize(normal); 

	//declaring the variable that we append to the Trianglestream 
	GS_Output element;

	//Here the normal I calculated is multiplied with the "Transformmatrix"
	//This is done so that the same transformations made on the face 
	//are applied to the normal. The main part of the transformmatrix
	//is the world-matrix. This contains the rotation.
	//It's important that the normal of the face rotates as much as the face itself.
	element.normal = mul(normal, worldMatrix);                       
	element.normal = normalize(element.normal);

	//In this loop I apply the matrixtransformation.
	for (uint i = 0; i < 3; i++)
	{
		element.wPos = mul(input[i].Pos, worldMatrix);
		element.Pos = mul(input[i].Pos, transformMatrix);
		element.uv = input[i].uv;
		output.Append(element); //my TriangleStream called output will create a triangle for every 3 vertex it receives
	}
	output.RestartStrip();
	for (uint z = 0; z < 3; z++) { //This triangle isn't showing...
								   //and now to make the "moved" quad:

								   //element.Pos = input[z].Pos; //Hmm... The second quad created here is much larger than the other quad.

								   //normal = mul(normal, transformMatrix);

		element.wPos = mul((input[z].Pos + normal), worldMatrix);
		element.Pos = mul((input[z].Pos + normal), transformMatrix); //this doesn't work for some reason as I think.
		element.uv = input[z].uv;
		output.Append(element);
	}
}

//void GS_main(
	//	triangle GS_Input input[3] : SV_POSITION, //Three vertices in list 
	//	inout TriangleStream< GS_Output > output //The output where they are reinterpreted as primitives
	//)
	//{	
	//	matrix transformMatrix; //My complete transformationmatrix!
	//	transformMatrix = mul(worldMatrix, viewMatrix);
	//	transformMatrix = mul(transformMatrix, projectionMatrix);
	//
	//	
	//
	//	input[0].Pos = mul(input[0].Pos, transformMatrix);
	//	input[1].Pos = mul(input[1].Pos, transformMatrix);
	//	input[2].Pos = mul(input[2].Pos, transformMatrix); //transformMatrix is a matrix holding the info relative to the camera.
	//
	//	float4 triangleSide1 = input[1].Pos - input[0].Pos;
	//	float4 triangleSide2 = input[2].Pos - input[0].Pos;
	//	float4 normal = float4(cross((float3) triangleSide1, (float3) triangleSide2), 0); //Changed the w-value to 0 in anticipation of the addition l8er on.
	//	normal = normalize(normal);
	//
	//	GS_Output element;
	//	for (uint i = 0; i < 3; i++)
	//	{
	//		element.Pos = input[i].Pos;
	//		element.Color = input[i].Color;
	//		output.Append(element); //my TriangleStream called output will create a triangle for every 3 vertex it receives
	//	}
	//	output.RestartStrip();
	//	for (uint z = 0; z < 3; z++) { //This triangle isn't showing...
	//		//and now to make the "moved" quad:
	//
	//		//element.Pos = input[z].Pos; //Hmm... The second quad created here is much larger than the other quad.
	//		
	//		//normal = mul(normal, transformMatrix);
	//
	//		element.Pos = input[z].Pos + normal; //this doesn't work for some reason as I think.
	//		element.Color = input[z].Color;      
	//		output.Append(element);            
	//	}
	//}
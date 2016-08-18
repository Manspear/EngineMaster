struct VS_IN //Input to vertex shader. Must match struct of vertex buffer.
{
	float3 Pos : POSITION0;//Homogeneous clip space  /w f�r att f� Normalised device space   utifr�n kameran
	float3 normal : NORMAL0;
	float2 UV : TEXCOORD0;
};


struct VS_OUT //Output. Must match input of pixel shader. 
{
	float4 Pos : SV_Position;//Homogeneous clip space  /w f�r att f� Normalised device space   utifr�n kameran
	float3 normal : normal;
	float2 UV : TEXCOORD;
	float4 worldPosition : WORLDSPACE;
};

//Pixelshadern vill ha i clip space.
//n�r de kommer in hit och �r omultiplicerade s� �r de i modelspace, / objectspace

//cbuffern h�r
// g�r en ny cbuffer med Material,  ambient diffuse specular, reflect
cbuffer matrixBuffer:register(b0)
{
	matrix viewMatrix;
	matrix projectionMatrix;

	float4 camPos;
	float4 camDir;
};

cbuffer worldBuffer:register(b1) // det objektet som f�r dcm p� sig m�ste ha en worldmatrix. Har alla modeller det? isf ska jag skicka den hit
{ //Gotten from the GModel class.
	matrix worldMatrix;
};
//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------


VS_OUT VS_main(VS_IN input)
{

	VS_OUT output = (VS_OUT)0; //Hmm... What is this? Am I zeroing-out my output?

	matrix allMatrix = mul(mul(worldMatrix, viewMatrix), projectionMatrix);

	output.Pos = mul(float4(input.Pos, 1), allMatrix); // typecasting float3 to float4. The 1 is the w-part of the 4x1 point
													   //eftersom att det �r en ,1 d�r nu s� kan du flytta sig. En vector har ingen position, bara en riktning. Om man s�tter en ,0 s� kan den bara scalas och roteras.

	output.normal = normalize(mul(float4(input.normal, 0), worldMatrix).xyz); // eftersom .xyz s� blev det en float 3 igen
	output.UV = input.UV;
	output.worldPosition = mul(float4(input.Pos, 1), worldMatrix);

	return output;
}
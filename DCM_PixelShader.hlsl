//det är bra att ha en separat pixel shader för detta om inte alla material ska vara reflektiva. 
// se till att ha pixeln i world space. Gör en vektor beräkning från kameran till den pixeln för att få en vektor. sen speglar jag den som i linjär algebra med normalen. finns en inbyggd funktion för detta

SamplerState DCM_TriLinearSample
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

Texture2D txDiffuse[2]; //: register(t0); //This receives the texture
SamplerState sampAni;
TextureCube gCubeMap;
//cbuffern här


struct PS_IN
{
	float4 Pos : SV_Position;//Homogeneous clip space  /w för att få Normalised device space   utifrån kameran
	float3 normal : normal;
	float2 UV : TEXCOORD;
	float4 worldPosition : WORLDSPACE;//pixel, pixlarna på en triangel
	float4 gReflectionEnable : TEXCOORD1;//needs to convert

	float3 tangent : TANGENT;
	float3 biTangent : BITANGENT;
	float4 camDir :WORLDSPACE2;
	float padda : TEXCOORDpadda;
};

float4 PS_main(PS_IN input) : SV_Target
{
	//float3 reflectivityVector = float3(x, y, z);//reflektionsvinkeln. Spec map vectorn. 
	//float4 color = gCubeMap.Sample(gTriLinearSample, v);
	float3 reflectHard = (1,1,1);
	
	float3 lightPos = normalize(float3 (4, 3, -3) - input.worldPosition);
	float3 toEye = float3 (4, 3, -3) - input.worldPosition; //Vector from worldPosition to camera. This is correct.
	float4 ambientLightColor = { 0.2, 0.2, 0.2, 0 };
	float4 diffuseColor = float4(1,1,1,1);

	float3 color, textureColor, bumpMap;

	
	bumpMap = (bumpMap * 2.0f) - 1.0f;
	input.normal = (bumpMap.x * input.tangent) + (bumpMap.y * input.biTangent) + (bumpMap.z * input.normal);
	input.normal = normalize(input.normal);

	float lightIntensity;

	//specular
	float shinypower = 3.0f;
	float4 specular = float4(1.0, 1.0, 1.0, 1.0);
	float3 r = reflect(normalize(lightPos - input.worldPosition), input.normal);
	float3 v = normalize(input.worldPosition * -1.0);

	float3 sl = specular * pow(max(dot(r, v), 0.0f), shinypower); //sl = Specular Lighting


	lightPos = normalize(lightPos);
	lightIntensity = dot(input.normal, lightPos); //angle of ray vs object determines the amount of reflected light. saturate means 0-1 dot product. If they are the same it returns 0 if 90 1

	color = saturate(diffuseColor * lightIntensity); //if lower than 0 set to 0, else if higher than 1 set to 1;
	color = color * textureColor;
	color = color + (ambientLightColor * textureColor);
	color = color + sl;

	//DMC vvvvvvvvvvvvvvvvvvvvvv


		float3 incident = input.camDir;
		float3 reflectionVector = reflect(incident, input.normal);//reflect ska finnas i material structen, kanske ska vara normalW
		float4 reflectionColor = gCubeMap.Sample(DCM_TriLinearSample, reflectionVector);// här annorlunda

		color += reflectHard * reflectionColor;//gMaterial.Reflect 


	//^^^^^^^^^^^^^^^^^^^^^^^^

	return gCubeMap.Sample(DCM_TriLinearSample, lightPos);
};
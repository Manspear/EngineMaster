//det �r bra att ha en separat pixel shader f�r detta om inte alla material ska vara reflektiva. 
// se till att ha pixeln i world space. G�r en vektor ber�kning fr�n kameran till den pixeln f�r att f� en vektor. sen speglar jag den som i linj�r algebra med normalen. finns en inbyggd funktion f�r detta

SamplerState DCM_TriLinearSam
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

Texture2D txDiffuse[2]; //: register(t0); //This receives the texture
SamplerState sampAni;
TextureCube gCubeMap;

struct PS_IN
{
	float4 Pos : SV_Position;//Homogeneous clip space  /w f�r att f� Normalised device space   utifr�n kameran
	float3 normal : normal;
	float2 UV : TEXCOORD;
	float4 worldPosition : WORLDSPACE;//pixel, pixlarna p� en triangel

	float3 tangent : TANGENT;
	float3 biTangent : BITANGENT;
};

float4 PS_main(PS_IN input) : SV_Target
{
	//float3 reflectivityVector = float3(x, y, z);//reflektionsvinkeln. Spec map vectorn. 
	//float4 color = gCubeMap.Sample(gTriLinearSample, v);

	float3 toEye = float3 (4, 3, -3) - input.worldPosition; //Vector from worldPosition to camera. This is correct.
	float4 ambientLightColor = { 0.2, 0.2, 0.2, 0 };
	float4 diffuseColor = float4(1,1,1,1);

	float3 color, textureColor, bumpMap;

	reflectionColor = float3 (txDiffuse[1].Sample(sampAni, input.UV).xyz);
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

	if(gReflectionEnable)
	{
		float3 incident = -toEye;//
		float3 reflectionVector = reflect(incident, pin.NormalW);//reflect ska finnas i material structen
		float4 reflectionColor = float3 (gCubeMap.Sample(sampAni, input.UV).xyz);// h�r annorlunda

		color += gMaterial.Reflect * reflectionColor;
	}

	//^^^^^^^^^^^^^^^^^^^^^^^^

	return gCubeMap.Sample(DCM_TriLinearSam, lightPos)
};
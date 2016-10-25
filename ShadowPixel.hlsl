Texture2D shaderTexture : register(t0);
//The depthMapTexture is the shadow map. This texture contains the scene depth buffer rendered from the light's perspective.
Texture2D depthMapTexture : register(t1);

SamplerState SampleTypeClamp : register(s0);
SamplerState SampleTypeWrap : register(s1);

///---------CBUFFERS---------------//
//----NONE, HARDCODE THIS SHEIT----//
//-----------COLORS----------------//


struct PS_IN
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 lightViewPosition : TEXCOORD1;
	float3 lightPos : TEXCOORD2;
};

float4 PS_main(PS_IN input) : SV_Target
{
	float bias;
	float4 color; 
	float2 projectTexCoord;
	float depthValue;
	float lightDepthValue;
	float lightIntensity;
	float4 textureColor;

	float4 ambientColor = float4(0.3f, 0.3f, 0.3f, 0.0f);
	float4 diffuseColor = float4(1.0f, 1.0f, 1.0f, 1.0f)

	bias = 0.001f;

	//default color output for all puxiles
	color = ambientColor;

	projectTexCoord.x = input.lightViewPosition.x / input.lightViewPosition.w / 2.0f + 0.5f;
	projectTexCoord.y = -input.lightViewPosition.y / input.lightViewPosition.w / 2.0f + 0.5f;

	//determinate if the projected coordinates are in the 0 - 1 range, if so then this pixel is in the view of the light.
	if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
	{
		//sample the shadowmap depth value from the depth texure
		depthValue = depthMapTexture.Sample(SampleTypeClamp, projectTexCoord).r; //greyscale texture, so sampling the red channel is enough

		lightDepthValue = input.lightViewPosition.z / input.lightViewPosition.w;

		lightDepthValue = lightDepthValue - bias;

		if (lightDepthValue < depthValue)
		{
			lightIntensity = saturate(dot(input.normal, input.lightPos));

			if (lightIntensity > 0.0f)
			{
				color += diffuseColor * lightIntensity;
				color = saturate(color);
			}
		}
	}

	textureColor = shaderTexture.Sample(SampleTypeWrap, input.tex);
	color = color * textureColor;


	return color;
};
Texture2D txDiffuse[2]; //: register(t0); //This receives the texture
SamplerState sampAni;

struct PS_IN
{

	float4 Pos : SV_Position;
	float3 normal : normal;
	float2 UV : TEXCOORD;
	float4 worldPosition : WORLDSPACE;

	float3 tangent : TANGENT;
	float3 biTangent : BITANGENT;
};


//BIG EXPLANATION OF PIXELSHADER INC 
//the pixelshader works on a pixel-by-pixel-basis. The pixels are made
//by interpolating between the vertices. Each pixel gets interpolated
//values corresponding to the values of the vertex-struct.
//The rasterizer uses the depth-buffer to use depth-culling, which "throws away"
//the pixels that the viewer won't see. The pixels that the viewer will see are those with the 
//depth-value closest to the camera.
//>>>>>>>>>>>>>>>>>>>>SO TO SUMMARIZE: <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//We here have pixels with vertex-variables like POSITION, TEXCOORD, and normal.
//These pixels have already went through the rasterizers depth-culling, and are therefore 
//the ones that will be seen on the screen, since depth-culling removes pixels based on depth value. 


float4 PS_main(PS_IN input) : SV_Target
{
	//Light Inits
	//Why is the lightposition a vector pointing at the light position? It's weird. 
	float3 lightPos = normalize(float3 (4, 3, -3) - input.worldPosition); //Vector from worldPosition to camera(is correct)
	float4 ambientLightColor = { 0.2, 0.2, 0.2, 0 };
	float4 diffuseColor = float4(1,1,1,1);

	//texture
	float3 color, textureColor, bumpMap;
	color = textureColor = float3 (txDiffuse[0].Sample(sampAni, input.UV).xyz);

	//Normal
	bumpMap = float3 (txDiffuse[1].Sample(sampAni, input.UV).xyz);
	bumpMap = (bumpMap * 2.0f) - 1.0f;
	input.normal = (bumpMap.x * input.tangent) + (bumpMap.y * input.biTangent) + (bumpMap.z * input.normal);
	input.normal = normalize(input.normal);
	
	
	//specular
	float shinypower = 3.0f;
	float4 specular = float4(1.0, 1.0, 1.0, 1.0);
	float3 r = reflect(normalize(lightPos - input.worldPosition), input.normal);
	float3 v = normalize(input.worldPosition * -1.0);

	float3 sl = specular * pow(max(dot(r, v), 0.0f), shinypower); //sl = Specular Lighting


	//lightCalc
	float lightIntensity = dot(input.normal, lightPos); //angle of ray vs object determines the amount of reflected light. 
	color = saturate(diffuseColor * lightIntensity); //if lower than 0 set to 0, else if higher than 1 set to 1;
	color = color * textureColor;
	color = color + (ambientLightColor * textureColor);
	color = color + sl;

	return float4(color,1);
};


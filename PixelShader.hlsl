Texture2D txDiffuse[2]; //: register(t0); //This receives the texture
Texture2D shadowMap;
SamplerState sampAni;
//SamplerState derp;

struct PS_IN
{
	float4 Pos : SV_Position;
	float3 normal : normal;
	float2 UV : TEXCOORD;
	float4 pixelPosition : WORLDSPACE;

	float3 tangent : TANGENT;
	float3 biTangent : BITANGENT;
	float4 camPos : WORLDSPACE1;
	float4 camDir : WORLDSPACE2;
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
	//float hej = shadowMap.Sample(sampAni, input.UV).x;
	//Light Inits
	float3 lightPoint = float3 (4, 3, -3);

	//Why is the lightposition a vector pointing at the light position? It's weird. 
	float3 lightPos = normalize(lightPoint - input.pixelPosition); //Vector from pixelPosition to camera(is correct)
	float4 ambientLightColor = { 0.3, 0.3, 0.3, 0 };
	float4 diffuseColor = float4(1, 1, 1, 1);
	float specShadow = 1.0;

	//texture
	float3 color, textureColor, bumpMap;
	color = textureColor = float3 (txDiffuse[0].Sample(sampAni, input.UV).xyz);

	float3 r = reflect(-lightPos, input.normal);
	float3 v = normalize(input.camPos - input.pixelPosition);
	
	//if (dot(r, v) < 0.0f)
	//	specShadow = 0.0;
	
	//Normal
	bumpMap = float3 (txDiffuse[1].Sample(sampAni, input.UV).xyz);
	bumpMap = (bumpMap * 2.0f) - 1.0f;
	input.normal = (bumpMap.x * input.tangent) + (bumpMap.y * input.biTangent) + (bumpMap.z * input.normal);
	input.normal = normalize(input.normal);
	
	float lightIntensity = dot(input.normal, lightPos);

	//specular
	float shinypower = 100.0f;
	float4 specular = float4(1.0, 1.0, 1.0, 1.0);
	r = reflect(-lightPos, input.normal);
	v = normalize(input.camPos - input.pixelPosition); 

	//float specShadow = saturate(4 * diffuseColor);

	float3 sl = specular * pow(max(dot(r, v), 0.0f), shinypower) * specShadow; //sl = Specular Lighting
	float3 diffuse = saturate(diffuseColor * lightIntensity) * textureColor;
	float3 ambient = (ambientLightColor * textureColor);

	//lightCalc
	 //angle of ray vs object determines the amount of reflected light. 
	color = diffuse + ambient + sl/* */;

	return float4(color,1);
	//return float4(sl + 0.1f, 1.f);// debug
};


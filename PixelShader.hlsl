Texture2D txDiffuse[2]; //: register(t0); //This receives the texture
Texture2D shadowMap;
SamplerState sampAni: register(s0)
{
	Filter = MIN_MAG_MIP_LINEAR;
	AdressU = Wrap;
	AdressV = Wrap;
};

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
	
	//Normal
	bumpMap = float3 (txDiffuse[1].Sample(sampAni, input.UV).xyz); // get the color value of this pixel on the normal map
	bumpMap = (bumpMap * 2.0f) - 1.0f; //Not really neccesay i think. 

	input.normal = (bumpMap.x * input.tangent) + (bumpMap.y * input.biTangent) + (bumpMap.z * input.normal);  //this decides how the pixel normal is affected by the nomalMap xyz=rgb
	input.normal = normalize(input.normal);
	
	float lightIntensity = dot(input.normal, lightPos);

	//specular
	float shinypower = 100.0f;
	float4 specular = float4(1.0, 1.0, 1.0, 1.0);
	r = reflect(-lightPos, input.normal);
	v = normalize(input.camPos - input.pixelPosition); 

	float3 sl = specular * pow(max(dot(r, v), 0.0f), shinypower) * specShadow; //sl = Specular Lighting
    //Saturate converts 255-point color values to values between 0 to 1
	float3 diffuse = saturate(diffuseColor * lightIntensity) * textureColor;
	float3 ambient = (ambientLightColor * textureColor);

	color = diffuse + ambient + sl;

	return float4(color,1);
};


Texture2D txDiffuse : register(t0); //This receives the texture
SamplerState sampAni;

struct PS_IN
{
	float4 wPos : POSITION0;
	float4 Pos : SV_POSITION; //SV_POSITION it used specifically for the rasterization-stage. And has no further use in the pipeline.
	float2 uv : TEXCOORD;
	float4 normal : NORMAL;
};

/*struct PS_OUT {
	float4 Pos : SV_POSITION;
	float2 uv : UV;
	float4 Color; //Do I have to have something typed like : COLOR here..?
};*/

//BIG EXPLANATION OF PIXELSHADER INC 
//the pixelshader works on a pixel-by-pixel-basis. The pixels are made
//by interpolating between the vertices. Each pixel gets interpolated
//values corresponding to the values of the vertex-struct.
//The rasterizer uses the depth-buffer to use depth-culling, which "throws away"
//the pixels that the viewer won't see. The pixels that the viewer will see are those with the 
//depth-value closest to the camera.
//>>>>>>>>>>>>>>>>>>>>SO TO SUMMARIZE: <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//We here have pixels with vertex-variables like POSITION, TEXCOORD, and NORMAL.
//These pixels have already went through the rasterizers depth-culling, and are therefore 
//the ones that will be seen on the screen, since depth-culling removes pixels based on depth value. 


float4 PS_main(PS_IN input) : SV_Target
{
	float4 lightPos = { 0, 0, -2.01, 1 };
	float4 diffuseLightColor = { 1, 1, 1, 1 };
	float4 ambientLightColor = { 0.1, 0.1, 0.1, 0 };

	float4 lightVector = lightPos - input.wPos; //From surface to lightsource
	//float4 lightVector = input.wPos - lightPos;
	//lightVector[3] = 1;
	lightVector = normalize(lightVector);
	input.normal = normalize(input.normal);
	float lightCam = dot(lightVector, input.normal); //lightCam determines how much light hits the camera.
	
	float3 s = txDiffuse.Sample(sampAni, input.uv).xyz; //sampAni = defaultsampler. .xyz says "take first 3 parts of array"
	//s now holds RGB-values ranging from 0-255. The RGB value is that of the texture at the UV-coordinate named "uv".
	float3 color;

	//Cool. Seems like mul(vector1, vector2) doesn't multiply vector1.x with vector1.x 
	//When I did it in a loop instead, the color-variable acted as I planned.
	float oldColor;
	for (int i = 0; i < 3; i++) {
		color[i] = mul(diffuseLightColor[i], lightCam);
		oldColor = s[i];
		s[i] = mul(s[i], color[i]); //Here I multiply ambientLightColor with s[i]. This to make ambientLightColor the same "gradient" as the texture
		s[i] = max(0, s[i]); //Light can become negative, i.e less than 0.
		s[i] = s[i] + ambientLightColor[i] * oldColor; //Always remember: What is this vector getting used for? --> Keep them in the same spaces, never mix
	}
	//float3 minLight = { 0,0,0 };
	//s = max(s, minLight);
	//float3 maxLight = { 1, 1, 1 };
	//s = min(s, maxLight);

	return float4 (s, 1.0f); //Here I say that THIS pixel with THESE texture coordinates, receives this color.
}; 

//>>>>>>>>>>>>>>>NOTE ABOUT READ AND WRITE ACCESS:
//"Write" is performed when the GPU changes a Resource
//"Read" is performed when the GPU uses that resource.
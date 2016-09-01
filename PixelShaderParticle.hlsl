
struct PS_IN
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
    float2 UV : TEXCOORD;
    float4 pixelPosition : WORLDSPACE;
};

float4 PS_main(PS_IN input) : SV_Target
{
	return float4(input.Color.xyz, 1.0f);
}
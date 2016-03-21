
struct PS_IN
{
    float4 Pos : SV_Position;
    float2 UV : TEXCOORD;
    float4 pixelPosition : WORLDSPACE;
    float3 Color : COLOR;

};

float4 PS_main(PS_IN input) : SV_Target
{
	return float4(input.Color, 1.0f);
}
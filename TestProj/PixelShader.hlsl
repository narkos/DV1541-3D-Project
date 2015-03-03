Texture2D txDiffuse : register(t0);
SamplerState sampAni
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;
	AdressU = WRAP;
	AdressV = WRAP;
};

struct PS_OUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 wpos : POSITION;
	float4 nor : NORMAL;
};

float4 PS_main(PS_OUT input) : SV_Target
{
	float4 a = input.nor;
	float3 s = txDiffuse.Sample(sampAni, input.tex).xyz;
	return float4(s, 1.0f);
}
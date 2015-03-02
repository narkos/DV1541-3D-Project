Texture2D txDiffuse : register(t0);
SamplerState sampAni
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;
	AdressU = WRAP;
	AdressV = WRAP;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float3 tex : TEXCOORD;
	float4 nor : NORMAL;
};

float4 PS_main(VS_OUT input) : SV_Target
{
	return float4(input.tex, 1.0f);
}
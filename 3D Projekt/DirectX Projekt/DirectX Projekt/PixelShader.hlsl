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



}
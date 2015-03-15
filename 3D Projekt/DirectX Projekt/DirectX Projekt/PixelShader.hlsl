Texture2D txDiffuse : register(t0);
SamplerState sampAni
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;
	AdressU = WRAP;
	AdressV = WRAP;
};

struct GS_OUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float4 nor : NORMAL;
};

float4 PS_main(GS_OUT input) : SV_Target
{
	//float3 s = txDiffuse.Sample(sampAni, input.tex);
	return float4(float3(0.0f,0.0f,1.0f ),1.0f);
	//return float4(input.tex, 1.0f);
	
}
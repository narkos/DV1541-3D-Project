Texture2D txDiffuse : register(t0);
SamplerState sampAni
{
	/*Filter = ANISOTROPIC;
	MaxAnisotropy = 4;
	AdressU = WRAP;
	AdressV = WRAP;*/
};
cbuffer cbPerObject : register(c0)
{
	matrix WVP;
	float4 diffuseColor;
	bool hasTexture;
};
struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float4 nor : NORMAL;
};

float4 PS_main(VS_OUT input) : SV_Target
{
	float3 outputColor = float3(0.0f, 0.0f, 0.0f);
	float3 diffuse = diffuseColor.xyz;
	if (hasTexture == true)
	{
		diffuse = txDiffuse.Sample(sampAni, input.tex);
		return float4(diffuse, 1.0f);
	}
	else
	{
		return float4(float3(0.0f, 0.0f, 1.0f), 1.0f);
	}
	
	

}
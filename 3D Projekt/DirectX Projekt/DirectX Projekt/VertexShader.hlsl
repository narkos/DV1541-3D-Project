cbuffer cbPerObject : register(c0)
{
	float4x4 WVP;
};

struct VS_IN
{
	float3 pos : POSITION;
	float2 tex : TEXCOORD;
	float3 nor  : NORMAL;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float4 nor  : NORMAL;
};

VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	//float4 position = float4(input.pos,1);

	output.pos = mul(float4(input.pos, 1.0f), WVP);
	
	//output.pos = float4(input.pos, 1);
	output.nor = float4(input.nor, 0);
	output.tex = input.tex;

	return output;
}
 
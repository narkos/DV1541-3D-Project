struct VS_IN
{
	float3 pos : POSITION;
	float3 tex : TEXCOORD;
	float4 nor  : NORMAL;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float3 tex : TEXCOORD;
	float4 nor  : NORMAL;
};

VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.pos = float4(input.pos, 1);
	//output.nor = float4(input.nor, 0);
	output.tex = input.tex;

	return output;
}

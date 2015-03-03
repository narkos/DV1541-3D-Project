struct GS_OUT
{
	float4 pos  : SV_POSITION;
	float3 wpos : POSITION;
	float4 tex  : TEXCOORD;
	float4 nor  : NORMAL;
};

struct GS_IN
{
	float4 pos  : SV_POSITION;
	float3 wpos : POSITION;
	float4 tex  : TEXCOORD;
	float4 nor  : NORMAL;
};

[maxvertexcount(3)]
void GS_main(triangle GS_IN input[3], inout TriangleStream< GS_OUT > output)
{
	for (uint i = 0; i < 3; i++)
	{
		GS_OUT element;

		element.pos = input[i].pos;
		element.tex = input[i].tex;
		element.nor = input[i].nor;
		element.wpos = input[i].wpos;


		output.Append(element);
	}
	output.RestartStrip();
}
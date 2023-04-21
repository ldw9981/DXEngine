struct ps_input
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
};

float4 main(ps_input input) : SV_TARGET
{
	return input.color;
}
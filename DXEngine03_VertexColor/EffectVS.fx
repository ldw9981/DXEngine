// 정점 셰이더 출력용 구조체.
struct vs_output
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
};

// 정점 셰이더.
vs_output main(float4 pos : POSITION,
	float4 color : COLOR)
{
	vs_output output;
	output.pos = pos;
	output.color = color;

	return output;
}
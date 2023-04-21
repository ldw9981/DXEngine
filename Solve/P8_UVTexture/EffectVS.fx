// 상수버퍼.
cbuffer cbPerObject
{
	matrix world;
	matrix view;
	matrix projection;
};

// 정점 셰이더 출력용 구조체.
struct vs_output
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
	float2 texCoord : TEXCOORD0;
};

// 정점 셰이더.
vs_output main(float4 pos : POSITION,
	float4 color : COLOR, float2 texCoord : TEXCOORD0)
{
	vs_output output;
	// 공간 변환.
	output.pos = mul(pos, world);		// 월드 변환.
	output.pos = mul(output.pos, view);		// 뷰 변환.
	output.pos = mul(output.pos, projection);		// 투영 변환.

	output.color = color;

	// 정점에서 입력받은 텍스처 좌표 반환.
	output.texCoord = texCoord;

	return output;
}
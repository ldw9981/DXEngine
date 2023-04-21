// 상수버퍼.
cbuffer cbPerObject
{
	matrix world;
	matrix view;
	matrix projection;
};

// 정점 셰이더 입력용 구조체.
struct vs_input
{
	float4 pos : POSITION;
	float2 texCoord : TEXCOORD0;
	float3 normal : NORMAL;
};

// 정점 셰이더 출력용 구조체.
struct vs_output
{
	float4 pos : SV_POSITION;
	//float4 color : COLOR;
	float2 texCoord : TEXCOORD0;
	float3 normal : NORMAL;
};

// 정점 셰이더.
vs_output main(vs_input input)
{
	vs_output output;
	// 공간 변환.
	output.pos = mul(input.pos, world);		// 월드 변환.
	output.pos = mul(output.pos, view);		// 뷰 변환.
	output.pos = mul(output.pos, projection);		// 투영 변환.

	//output.color = color;

	// 정점에서 입력받은 텍스처 좌표 반환.
	output.texCoord = input.texCoord;

	// 노멀 정보 반환.
	output.normal = input.normal;

	return output;
}
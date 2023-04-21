// 상수버퍼 - 공간변환행렬.
cbuffer cbPerObject : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
};

// 상수버퍼 - 라이트용.
cbuffer cbLight : register(b1)
{
	float4 worldLightPosition;
	float4 worldCameraPosition;
};

// 정점 셰이더 입력용 구조체.
struct vs_input
{
	float4 pos : POSITION;
	float2 texCoord : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};

// 정점 셰이더 출력용 구조체.
struct vs_output
{
	float4 pos : SV_POSITION;
	float2 texCoord : TEXCOORD0;
	float3 lightDir : TEXCOORD1;
	float3 viewDir : TEXCOORD2;

	float3 T : TEXCOORD3;
	float3 B : TEXCOORD4;
	float3 N : TEXCOORD5;
};

// 정점 셰이더.
vs_output main(vs_input input)
{
	vs_output output;
	// 공간 변환.
	output.pos = mul(input.pos, world);		// 월드 변환.

	// 라이트 방향 구하기.
	output.lightDir = normalize(output.pos - worldLightPosition);
	// 뷰 방향 구하기.
	output.viewDir = normalize(output.pos - worldCameraPosition);

	// 월드 좌표계 기준 노멀 값 구하기.
	output.N = normalize(mul(input.normal, (float3x3)world));
	// 월드 좌표계 기준 탄젠트 값 구하기.
	output.T = normalize(mul(input.tangent, (float3x3)world));
	// 월드 좌표계 기준 바이노멀 값 구하기.
	output.B = normalize(mul(input.binormal, (float3x3)world));

	output.pos = mul(output.pos, view);		// 뷰 변환.
	output.pos = mul(output.pos, projection);		// 투영 변환.

	// 정점에서 입력받은 텍스처 좌표 반환.
	output.texCoord = input.texCoord;

	return output;
}
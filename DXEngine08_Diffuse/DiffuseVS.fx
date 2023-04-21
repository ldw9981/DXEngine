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
	float3 diffuse : TEXCOORD1;
};

// 정점 셰이더.
vs_output main(vs_input input)
{
	vs_output output;
	// 공간 변환.
	output.pos = mul(input.pos, world);		// 월드 변환.

	// 라이트 방향 구하기.
	float4 lightDir = output.pos - worldLightPosition;
	
	// 단위벡터로 만들기 (정규화).
	lightDir = normalize(lightDir);

	// 월드 좌표계 기준 노멀 값 구하기.
	float3 worldNormal = normalize(mul(input.normal, (float3x3)world));

	// 내적.
	float3 diffuse = dot(-lightDir.xyz, worldNormal);
	/*diffuse = -lightDir.x * worldNormal.x + -lightDir.y * worldNormal.y + -lightDir.z * worldNormal.z;*/

	// 난반사광 반환.
	/*output.diffuse = diffuse;*/
	output.diffuse = diffuse * 0.5f + 0.5f;

	output.pos = mul(output.pos, view);		// 뷰 변환.
	output.pos = mul(output.pos, projection);		// 투영 변환.

	//output.color = color;

	// 정점에서 입력받은 텍스처 좌표 반환.
	output.texCoord = input.texCoord;

	// 노멀 정보 반환.
	output.normal = input.normal;

	return output;
}
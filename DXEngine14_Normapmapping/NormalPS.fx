// 픽셀 셰이더 입력 구조체.
struct ps_input
{
	float4 pos : SV_POSITION;
	float2 texCoord : TEXCOORD0;
	float3 lightDir : TEXCOORD1;
	float3 viewDir : TEXCOORD2;

	float3 T : TEXCOORD3;
	float3 B : TEXCOORD4;
	float3 N : TEXCOORD5;
};

// 텍스처 관련 리소스.
Texture2D diffuseMap : register (t0);
Texture2D normalMap : register (t1);
SamplerState texSampler : register(s0);

float4 main(ps_input input) : SV_TARGET
{
	// 텍스처에서 읽어온 노멀.
	float3 tangentNormal
		= normalMap.Sample(texSampler, input.texCoord);
	tangentNormal = normalize(tangentNormal * 2 - 1);

	// TBN 행렬 만들기.
	float3x3 TBN = float3x3(
		normalize(input.T),
		normalize(input.B),
		normalize(input.N)
	);

	float3 worldNormal = normalize( mul(tangentNormal, TBN) );			// 아래와같은결과
    //float3 worldNormal = (tangentNormal.x * input.T) + (tangentNormal.y * input.B) + (tangentNormal.z * input.N);
    worldNormal = normalize(worldNormal);	

	// 라이트 벡터.
	float3 lightDir = normalize(input.lightDir);

	// 난반사광 구하기.
	float3 diffuse = saturate(dot(worldNormal, -lightDir));
	// 하프 램버트(Half Lambert).
	//diffuse = pow(diffuse * 0.5f + 0.5f, 2.0f);

	// 디퓨즈맵 읽기.
	float4 albedo = diffuseMap.Sample(texSampler, input.texCoord);		

	// 최종 디퓨즈 정보.
	diffuse = diffuse * albedo;

	// 정반사광(specular).
	float3 specular = 0;
	if (diffuse.x > 0)
	{
		// 반사벡터/뷰 벡터 구하기.
		float3 reflection = reflect(lightDir, worldNormal);
		float3 viewDir = normalize(input.viewDir);

		specular = dot(reflection, -viewDir);
		specular = saturate(specular);
		specular = pow(specular, 256.0f);
	}

	// 앰비언트 라이트.
	float3 ambient = float3(0.05f, 0.05f, 0.05f);

	// 최종 색상 반환.
    return float4(ambient + diffuse + specular, 1);
}
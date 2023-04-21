// ÇÈ¼¿ ¼ÎÀÌ´õ ÀÔ·Â¿ë ±¸Á¶Ã¼.
struct ps_input
{
	float4 pos : SV_POSITION;
	float2 texCoord : TEXCOORD0;
	float3 normal : NORMAL;
	float3 diffuse : TEXCOORD1;

	float4 viewDir : TEXCOORD2;
	float3 reflection : TEXCOORD3;
};

// ÅØ½ºÃ³ Á¤º¸.
Texture2D objTexture;
SamplerState objSampler;

// ÇÈ¼¿ ¼ÎÀÌ´õ(½¦ÀÌ´õ/¼ÎÀÌ´õ).
float4 main(ps_input input) : SV_TARGET
{
	// ÅØ½ºÃ³ »ùÇÃ¸µ (»ö»ó Á¤º¸ ÃßÃâ).
	float4 texColor = objTexture.Sample(
		objSampler, input.texCoord);

	// Diffuse »ö»ó ÃßÃâ.
	float3 diffuse = saturate(input.diffuse);

	// ½ºÆäÅ§·¯ °è»ê.
	float3 reflection = normalize(input.reflection);
	float3 viewDir = normalize(input.viewDir.xyz);
	float3 specular = 0;

	if (diffuse.x > 0)
	{
		// ³»Àû (ºä º¤ÅÍ, ¹Ý»çº¤ÅÍ).
		specular = dot(reflection, -viewDir);
		// 0-1 °íÁ¤.
		specular = saturate(specular);
		// Á¤¹Ý»ç ¿µ¿ª ÁÙÀÌ°í ºû ÁõÆø½ÃÅ°±â.
		specular = pow(specular, 20.0f);
	}

	// ÃÖÁ¾ »ö»ó.
	float3 finalColor = texColor.rgb * diffuse + specular;

	//return float4(finalColor, 1);
	return float4(specular, 1);
}
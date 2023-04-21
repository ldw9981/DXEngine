// ÇÈ¼¿ ¼ÎÀÌ´õ ÀÔ·Â¿ë ±¸Á¶Ã¼.
struct ps_input
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
	float2 texCoord : TEXCOORD0;
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

	// ÅØ½ºÃ³ »ö»ó ¹ÝÈ¯.
	return texColor;
}
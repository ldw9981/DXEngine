// ÇÈ¼¿ ¼ÎÀÌ´õ ÀÔ·Â¿ë ±¸Á¶Ã¼.
struct ps_input
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
};

// ÇÈ¼¿ ¼ÎÀÌ´õ(½¦ÀÌ´õ/¼ÎÀÌ´õ).
float4 main(ps_input input) : SV_TARGET
{
	return input.color;
}
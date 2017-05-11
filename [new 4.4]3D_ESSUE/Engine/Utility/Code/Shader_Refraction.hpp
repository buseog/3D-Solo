

texture g_BaseTexture;

sampler BaseSampler = sampler_state
{
	texture = g_BaseTexture;
	
	addressU = clamp;
	addressV = clamp;
};

texture g_RefractionTexture;

sampler RefractionSampler = sampler_state
{
	texture = g_RefractionTexture;
};

struct PS_IN
{
	float2 vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	vector vColor : COLOR0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0.f;

	vector vRefraction = tex2D(RefractionSampler, In.vTexUV);

	Out.vColor = tex2D(BaseSampler, In.vTexUV + float2(vRefraction.x, vRefraction.y));

	return Out;
}


technique Default_Device
{
	pass Default_Rendering
	{
		AlphaBlendEnable = true;
		SrcBlend = srcalpha;
		DestBlend = invsrcalpha;

		ZEnable = false;
		ZWriteEnable = false;

		PixelShader = compile ps_3_0 PS_MAIN();
	}	
};
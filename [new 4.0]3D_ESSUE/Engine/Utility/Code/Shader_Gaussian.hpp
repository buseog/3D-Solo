
texture g_BaseTexture;

sampler BaseSampler = sampler_state
{
	texture = g_BaseTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

texture g_GaussianTexture;

sampler GaussianSampler = sampler_state
{
	texture = g_GaussianTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
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

	vector			vColor = tex2D(BaseSampler, In.vTexUV);

	for (int i = 0; i < 10; ++i)
	{
		float2 NewTexUV;
		NewTexUV.x = In.vTexUV.x;
		NewTexUV.y = In.vTexUV.y + (i / 600);
		vector vBlur = pow(tex2D(BaseSampler, NewTexUV), 10);
		vColor += vBlur;
	}

	Out.vColor = vColor;

	return Out;
}


technique Default_Device
{
	pass Default_Rendering
	{
		ZEnable = false;

		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_MAIN();
	}	
};

float4 gaussFilter[7] =
{
	0.f, 0.f, 0.f, 270.f/600.f,
	0.f, 1.f, 0.f, 100.f/600.f,
	0.f, 2.f, 0.f, 90.f/600.f,
	0.f, 3.f, 0.f,  70.f/600.f,
	0.f, 4.f, 0.f,  40.f/600.f,
	0.f, 5.f, 0.f,  20.f/600.f,
	0.f, 6.f, 0.f,  10.f/600.f,
};
float texScaler = 1.f/200.f;

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

	vector			vColor = 0.f;

	for (int i = 0; i < 7; ++i)
	{
		float2 NewTexUV;
		NewTexUV.x = In.vTexUV.x + gaussFilter[i].x * texScaler;
		NewTexUV.y = min(In.vTexUV.y + gaussFilter[i].y * texScaler, 1.f);
		
		vColor += tex2D(BaseSampler, NewTexUV) * gaussFilter[i].w;
	}

	Out.vColor = vColor;

	return Out;
}


technique Default_Device
{
	pass Default_Rendering
	{
		ZEnable = false;

		PixelShader = compile ps_3_0 PS_MAIN();
	}	
};
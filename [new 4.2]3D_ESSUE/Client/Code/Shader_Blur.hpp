
Texture g_BaseTexture;

sampler BaseSampler = sampler_state
{
	texture = g_BaseTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

Texture g_BlurTexture;

sampler BlurSampler = sampler_state
{
	texture = g_BlurTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

struct PS_IN
{
	float2	vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	vector	vColor : COLOR0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0.f;

	vector	vColor = tex2D(BaseSampler, In.vTexUV);

	vector	vBlur = tex2D(BlurSampler, In.vTexUV);

	Out.vColor = vColor + vBlur;

	return Out; 
}

technique Default_Device
{
	pass Default_Rendering
	{
		//ZWriteEnable = false;
		/*AlphaBlendEnable = true;
		SrcBlend = srcalpha;
		DestBlend = invsrcalpha;*/

		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_MAIN();
	}	
};
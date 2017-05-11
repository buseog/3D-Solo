
texture		g_AlbedoTexture;

sampler	AlbedoSampler = sampler_state
{
	texture = g_AlbedoTexture;
};

texture		g_ShadeTexture;

sampler	ShadeSampler = sampler_state
{
	texture = g_ShadeTexture;
};

texture		g_SpecularTexture;

sampler	SpecularSampler = sampler_state
{
	texture = g_SpecularTexture;
};

texture		g_BlurTexture;

sampler	BlurSampler = sampler_state
{
	texture = g_BlurTexture;
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

	vector		vAlbedo = tex2D(AlbedoSampler, In.vTexUV);
	vector		vShade = tex2D(ShadeSampler, In.vTexUV);
	vector		vSpecular = tex2D(SpecularSampler, In.vTexUV);
	vector		vBlur = tex2D(BlurSampler, In.vTexUV);

	Out.vColor = vAlbedo * vShade + vSpecular + vBlur;

	return Out;
}


technique Default_Device
{
	pass Default_Rendering
	{
		AlphaBlendEnable = true;
		SrcBlend = srcalpha;
		DestBlend = invsrcalpha;

		ZWriteEnable = false;
		ZEnable = false;

		VertexShader = NULL; 
		PixelShader = compile ps_3_0 PS_MAIN();
	}	
};
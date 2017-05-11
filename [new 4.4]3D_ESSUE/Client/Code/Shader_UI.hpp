#include "Shader_Base.hpp"

float		g_fPercent = 1.f;;	
texture		g_BaseTexture;

sampler	BaseSampler = sampler_state
{
	texture = g_BaseTexture;
};

struct VS_IN
{
	vector	vPosition : POSITION;
	float2	vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	vector	vPosition : POSITION;
	float2	vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out;	

	matrix		matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);
	
	Out.vPosition = mul(vector(In.vPosition.xyz, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}


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
	PS_OUT			Out;

	if (In.vTexUV.x <= g_fPercent)
		Out.vColor = tex2D(BaseSampler, In.vTexUV);		

	return Out;
}


technique Default_Device
{
	pass Default_Rendering
	{
		lighting = false;

		AlphaBlendEnable = true;
		SrcBlend = srcalpha;
		DestBlend = invsrcalpha;

		VertexShader = compile vs_3_0 VS_MAIN(); 
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	
};
#include "Shader_base.hpp"

int			g_iMaxSize = 40;
vector		g_TrailPos[40];
int			g_iCount;

vector		g_Color;

int			g_iY;
int			g_iX;

Texture		g_BaseTexture;

sampler BaseSampler = sampler_state
{
	Texture = g_BaseTexture;
	Minfilter = Linear;
	Magfilter = Linear;
	Mipfilter = Linear;
};

struct VS_IN
{
	vector vPos : POSITION;
	vector vColor : COLOR0;
	float2 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	vector vPos : POSITION;
	vector vColor : COLOR0;
	float2 vTexUV : TEXCOORD0;
};

struct PS_IN
{
	vector vColor : COLOR0;
	float2 vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	vector vColor : COLOR0;
};

VS_OUT VS_TRAIL(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0.f;

	matrix		matVP;
	matVP = mul(g_matView, g_matProj);

	int iX = max(In.vTexUV.x, 0.f);

	int fIndex = (-38.f * (In.vTexUV.y - 1.f)) + iX;

	int iY = fIndex / 2;
	
	float fNewY = 0.f;
	
	if (fIndex < g_iCount)
	{
		int iSize = (g_iCount - 1) / 2;
		fNewY = 1.f - ((1.f / iSize) * iY);
		Out.vPos = mul(g_TrailPos[fIndex], matVP);
		Out.vTexUV.x = In.vTexUV.x;
		Out.vTexUV.y = min(fNewY, 1.f);
	}
	
	return Out;
}

PS_OUT PS_TRAIL(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);
	if (Out.vColor.a != 0)
		Out.vColor += g_Color;

	return Out;
}

PS_OUT PS_POINT(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;

	Out.vColor = tex2D(BaseSampler, In.vTexUV) * In.vColor;

	return Out;
}

technique Default_Device
{
	pass Trail_Rendering
	{
		Lighting = false;
		ZWriteEnable = false;
		ZEnable = false;
		Cullmode = none;

		AlphaBlendEnable = true;
		SrcBlend = srcalpha;
		DestBlend = one;

		VertexShader = compile vs_3_0 VS_TRAIL();
		PixelShader = compile ps_3_0 PS_TRAIL();
	}

	pass Particle_Rendering
	{
		Lighting = false;
		ZWriteEnable = false;
		ZEnable = false;

		AlphaBlendEnable = true;
		SrcBlend = srcalpha;
		DestBlend = one;

		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_POINT();
	}
};
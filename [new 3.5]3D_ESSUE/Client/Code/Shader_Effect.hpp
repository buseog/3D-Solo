#include "Shader_base.hpp"

int			g_iMaxSize = 40;
vector		g_TrailPos[40];
int			g_iCount;

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

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0.f;

	matrix		matWV, matWVP;
	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPos = mul(vector(In.vPos.xyz, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;
	Out.vColor = In.vColor;
	return Out;
}

VS_OUT VS_TRAIL(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0.f;

	matrix		matVP;
	matVP = mul(g_matView, g_matProj);

	int fIndex = (-38.f * (In.vTexUV.y - 1.f)) + In.vTexUV.x;

	int iY = fIndex / 2;
	
	float fNewY = 0.f;
	
	if (fIndex < g_iCount)
	{
		int iSize = (g_iCount - 1) / 2;
		fNewY = 1.f - ((1.f / iSize) * iY);
		Out.vPos = mul(vector(g_TrailPos[fIndex].xyz, 1.f), matVP);
		Out.vTexUV.x = In.vTexUV.x;
		Out.vTexUV.y = min(fNewY, 1.f);
	}
	
	return Out;
}

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);

	return Out;
}

PS_OUT PS_TRAIL(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);
	if (Out.vColor.a != 0)
		Out.vColor.b += 0.3f;

	return Out;
}

PS_OUT PS_POINT(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;

	Out.vColor = tex2D(BaseSampler, In.vTexUV) * In.vColor;

	return Out;
}

PS_OUT PS_SPRITE(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;

	float2 vNewTexUV = 0.f; 
	vNewTexUV.x = (In.vTexUV.x * 0.2f) + (0.2f * g_iX);
	vNewTexUV.y = (In.vTexUV.y * 0.2f) + (0.2f * g_iX);

	Out.vColor = tex2D(BaseSampler, vNewTexUV);

	return Out;
}

technique Default_Device
{
	pass Default_Rendering
	{
		Lighting = false;
		Cullmode = none;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass AlphaBlend_Rendering
	{
		Lighting = false;
		Cullmode = none;

		AlphaBlendEnable = true;
		SrcBlend = srcalpha;
		DestBlend = invsrcalpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass AlphaTest_Rendering
	{
		Lighting = false;
		Cullmode = none;

		AlphaTestEnable = true;
		AlphaFunc = greater;
		AlphaRef = 0x5f;

		AlphaBlendEnable = true;
		SrcBlend = srcalpha;
		DestBlend = invsrcalpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass Trail_Rendering
	{
		Lighting = false;
		Cullmode = none;


		AlphaBlendEnable = true;
		SrcBlend = srcalpha;
		DestBlend = invsrcalpha;

		PointSpriteEnable = false;
		PointScaleenable = false;

		VertexShader = compile vs_3_0 VS_TRAIL();
		PixelShader = compile ps_3_0 PS_TRAIL();
	}

	pass Particle_Rendering
	{
		Lighting = false;

		/*AlphaBlendEnable = true;
		SrcBlend = srcalpha;
		DestBlend = one;*/

		AlphaTestEnable = true;
		AlphaFunc = greater;
		AlphaRef = 0x5f;

		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_POINT();
	}

	pass Sprite_Rendering
	{
		Lighting = false;
		Cullmode = none;

		AlphaTestEnable = true;
		AlphaFunc = greater;
		AlphaRef = 0x5f;

		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_SPRITE();
	}
};
#include "Shader_base.hpp"

vector		g_vColor;

int			g_iX;
int			g_iY;

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
	float2 vTexUV : TEXCOORD0;
	vector vColor : COLOR0;
	vector vRight : TEXCOORD1;
	vector vUp : TEXCOORD2;
	vector vLook : TEXCOORD3;
	vector vMatPos : TEXCOORD4;
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

	matrix matWorld = Matrix(In.vRight, In.vUp, In.vLook, In.vMatPos);

	matrix		matWV, matWVP;
	matWV = mul(matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPos = mul(vector(In.vPos.xyz, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;
	Out.vColor = In.vColor;

	return Out;
}

VS_OUT VS_PARTICLE(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0.f;

	matrix	matWV, matWVP;
	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPos = mul(vector(In.vPos.xyz, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;
	Out.vColor = g_vColor;

	return Out;
}

VS_OUT VS_SPRITE(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0.f;

	matrix	matWV, matWVP;
	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPos = mul(vector(In.vPos.xyz, 1.f), matWVP);
	Out.vTexUV.x = (In.vTexUV.x * 0.2f) + (0.2f * g_iX);
	Out.vTexUV.y = (In.vTexUV.y * 0.2f) + (0.2f * g_iX);
	Out.vColor = g_vColor;

	return Out;
}

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;

	Out.vColor = tex2D(BaseSampler, In.vTexUV) * In.vColor;

	return Out;
}


technique Default_Device
{
	pass AlphaBlend_Rendering
	{
		Lighting = false;
		Cullmode = none;

		ZWriteEnable = false;
		AlphaBlendEnable = true;
		SrcBlend = srcalpha;
		DestBlend = one;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass AlphaBlendone_Rendering
	{
		Lighting = false;
		Cullmode = none;

		ZWriteEnable = false;
		AlphaBlendEnable = true;
		SrcBlend = srcalpha;
		DestBlend = one;

		VertexShader = compile vs_3_0 VS_PARTICLE();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass AlphaBlend_Rendering
	{
		Lighting = false;
		Cullmode = none;

		ZWriteEnable = false;
		AlphaBlendEnable = true;
		SrcBlend = srcalpha;
		DestBlend = invsrcalpha;
	
		VertexShader = compile vs_3_0 VS_PARTICLE();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass AlphaTest_Rendering
	{
		Lighting = false;
		Cullmode = none;

		ZWriteEnable = false;
		AlphaTestEnable = true;
		AlphaFunc = greater;
		AlphaRef = 0x5f;

		VertexShader = compile vs_3_0 VS_PARTICLE();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass Sprite_Rendering
	{
		Lighting = false;
		Cullmode = none;

		ZWriteEnable = false;
		AlphaBlendEnable = true;
		SrcBlend = srcalpha;
		DestBlend = one;

		VertexShader = compile vs_3_0 VS_SPRITE();
		PixelShader = compile ps_3_0 PS_MAIN();
	}
};
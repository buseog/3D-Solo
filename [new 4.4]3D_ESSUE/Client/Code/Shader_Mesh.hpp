#include "Shader_base.hpp"

vector		g_vMaterialDiffuse;
vector		g_vMaterialAmbient;
vector		g_vMaterialSpecular;

float		g_fPower = 50.f;
float		g_fOffset;
float		g_fAlpha = 0.f;

vector		g_vCamPos;
matrix		g_matLight;

vector		g_vColor = vector(0.f, 0.f, 0.f, 0.f);

Texture		g_BaseTexture;

sampler BaseSampler = sampler_state
{
	texture = g_BaseTexture;
	Minfilter = Linear;
	Magfilter = Linear;
	Mipfilter = Linear;
};

Texture		g_ShadowTexture;

sampler ShadowSampler = sampler_state
{
	texture = g_ShadowTexture;
	Minfilter = Linear;
	Magfilter = Linear;
	Mipfilter = Linear;
};

struct VS_IN
{
	vector	vPos : POSITION;
	vector	vNormal : NORMAL;
	float2	vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	vector	vPos : POSITION;
	vector	vNormal : NORMAL;
	float2	vTexUV : TEXCOORD0;
	vector	vWorldPos : TEXCOORD1;
	vector	vProjPos : TEXCOORD2;
	vector	vShadowPos : TEXCOORD3;
};

struct PS_IN
{
	vector	vNormal : NORMAL;
	float2	vTexUV : TEXCOORD0;
	vector	vWorldPos : TEXCOORD1;
	vector	vProjPos : TEXCOORD2;
	vector	vShadowPos : TEXCOORD3;
};

struct PS_OUT
{
	vector vColor : COLOR0;
	vector vNormal : COLOR1;
	vector vDepth : COLOR2;
	vector vBlur : COLOR3;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0.f;

	matrix matWV, matWVP;
	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPos = mul(vector(In.vPos.xyz, 1.f), matWVP);
	Out.vNormal = normalize(mul(vector(In.vNormal.xyz, 0.f), g_matWorld));
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(vector(In.vPos.xyz, 1.f), g_matWorld);
	Out.vProjPos = Out.vPos;

	return Out;
}

VS_OUT VS_SHADOW(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0.f;

	matrix matWV, matWVP;
	matWV = mul(g_matWorld, g_matLight);
	matWVP = mul(matWV, g_matProj);

	Out.vPos = mul(vector(In.vPos.xyz, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPos;

	return Out;
}

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;

	Out.vColor = tex2D(BaseSampler, In.vTexUV) + g_vColor;
	Out.vColor.a = 1.f;

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	Out.vDepth = vector(In.vProjPos.w * 0.001f, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);

	return Out;
}

PS_OUT PS_SHADOW(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;

	vector vDepth = In.vProjPos.z / In.vProjPos.w;
	vDepth.w = 0.f;

	Out.vColor = 1.f - vDepth;

	return Out;
}

PS_OUT PS_RIM(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;

	vector	vTexColor = tex2D(BaseSampler, In.vTexUV);

	vector	vShade = max(dot(normalize(g_vLightDir) * -1.f, In.vNormal), 0.f);
	vector	vReflect = reflect(normalize(g_vLightDir), In.vNormal);
	vector	vLook = In.vWorldPos - g_vCamPos;
	vector	vSpecular = pow(max(dot(normalize(vLook) * -1.f, vReflect), 0.f), g_fPower);

	float	fRim = abs(dot(normalize(vLook) * -1.f, In.vNormal));
	if (fRim < 0.05f)
		Out.vColor = vector(1.f, 1.f, 1.f, 1.f);	
	else
		Out.vColor = (vTexColor * (g_vLightDiffuse * g_vMaterialDiffuse)) * (vShade + (g_vLightAmbient * g_vMaterialAmbient))
		+ vSpecular * (g_vLightSpecular * g_vMaterialSpecular) + g_vColor;

	Out.vColor.a = 1.f;
	Out.vNormal = (vector)1.f;

	return Out;
}

PS_OUT PS_NONERIM(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;

	vector	vTexColor = tex2D(BaseSampler, In.vTexUV);

	vector	vShade = max(dot(normalize(g_vLightDir) * -1.f, In.vNormal), 0.f);
	vector	vReflect = reflect(normalize(g_vLightDir), In.vNormal);
	vector	vLook = In.vWorldPos - g_vCamPos;
	vector	vSpecular = pow(max(dot(normalize(vLook) * -1.f, vReflect), 0.f), g_fPower);

	Out.vColor = (vTexColor * (g_vLightDiffuse * g_vMaterialDiffuse)) * (vShade + (g_vLightAmbient * g_vMaterialAmbient))
		+ vSpecular * (g_vLightSpecular * g_vMaterialSpecular);

	Out.vColor.a = 1.f;
	Out.vNormal = (vector)1.f;

	return Out;
}

PS_OUT PS_BLUR(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	Out.vDepth = vector(In.vProjPos.w * 0.001f, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);

	Out.vBlur = tex2D(BaseSampler, In.vTexUV) * 0.6f;

	return Out;
}

PS_OUT PS_Death(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);
	Out.vColor.a = g_fAlpha;

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	Out.vDepth = vector(In.vProjPos.w * 0.001f, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);

	return Out;
}

technique Default_Device
{
	pass Default_Rendering
	{
		CullMode = ccw;
		Lighting = true;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass Rim_Rendering
	{
		CullMode = ccw;
		Lighting = true;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_RIM();
	}

	pass AlphaBlend_Rendering
	{
		CullMode = ccw;
		Lighting = false;

		AlphaBlendEnable = true;
		SrcBlend = srcalpha;
		DestBlend = one;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass Blur_Rendering
	{
		CullMode = ccw;
		Lighting = true;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_BLUR();
	}

	pass Shadow_Rendering
	{
		ZWriteEnable = false;
		ZEnable = false;

		CullMode = ccw;
		Lighting = true;

		VertexShader = compile vs_3_0 VS_SHADOW();
		PixelShader = compile ps_3_0 PS_SHADOW();
	}

	pass Death_Rendering
	{
		AlphaBlendEnable = true;
		SrcBlend = srcalpha;
		DestBlend = invsrcalpha;

		ZWriteEnable = false;
		ZEnable = false;

		CullMode = ccw;
		Lighting = true;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_Death();
	}

	pass NoneRim_Rendering
	{
		CullMode = ccw;
		Lighting = true;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_NONERIM();
	}
};
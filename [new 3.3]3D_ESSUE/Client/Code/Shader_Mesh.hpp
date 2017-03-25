#include "Shader_base.hpp"

vector		g_vMaterialDiffuse;
vector		g_vMaterialAmbient;
vector		g_vMaterialSpecular;
float		g_fPower = 30.f;

vector		g_vCamPos;

Texture		g_BaseTexture;

sampler BaseSampler = sampler_state
{
	texture = g_BaseTexture;
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
};

struct PS_IN
{
	vector	vNormal : NORMAL;
	float2	vTexUV : TEXCOORD0;
	vector	vWorldPos : TEXCOORD1;
};

struct PS_OUT
{
	vector vColor : COLOR0;
	vector vBlur : COLOR1;
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

	return Out;
}

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;

	vector	vTexColor = tex2D(BaseSampler, In.vTexUV);

	vector	vShade = max(dot(normalize(g_vLightDir) * -1.f, In.vNormal), 0.f);
	vector	vReflect = reflect(normalize(g_vLightDir), In.vNormal);
	vector	vLook = In.vWorldPos - g_vCamPos;
	vector	vSpecular = pow(max(dot(normalize(vLook) * -1.f, vReflect), 0.f), g_fPower);

	Out.vColor = (vTexColor * (g_vLightDiffuse * g_vMaterialDiffuse)) * (vShade + (g_vLightAmbient * g_vMaterialAmbient))
		+ vSpecular * (g_vLightSpecular * g_vMaterialSpecular);	

	return Out;
}

PS_OUT PS_BLUR(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;

	vector	vTexColor = tex2D(BaseSampler, In.vTexUV);

	vector	vShade = max(dot(normalize(g_vLightDir) * -1.f, In.vNormal), 0.f);
	vector	vReflect = reflect(normalize(g_vLightDir), In.vNormal);
	vector	vLook = In.vWorldPos - g_vCamPos;
	vector	vSpecular = pow(max(dot(normalize(vLook) * -1.f, vReflect), 0.f), g_fPower);

	Out.vColor = (vTexColor * (g_vLightDiffuse * g_vMaterialDiffuse)) * (vShade + (g_vLightAmbient * g_vMaterialAmbient))
		+ vSpecular * (g_vLightSpecular * g_vMaterialSpecular);

	Out.vBlur = Out.vColor;

	return Out;
}
PS_OUT PS_ALPHA(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);

	return Out;
}

technique Default_Device
{
	pass Default_Rendering
	{
		Lighting = true;
		AlphaBlendEnable = false;
		AlphaTestEnable = false;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass AlphaBlend_Rendering
	{
		Lighting = false;
		AlphaBlendEnable = true;
		SrcBlend = srcalpha;
		DestBlend = invsrcalpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_ALPHA();
	}

	pass AlphaTest_Rendering
	{
		Lighting = false;
		AlphaTestEnable = true;
		AlphaFunc = greater;
		AlphaRef = 0x5f;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_ALPHA();
	}

	pass Blur_Rendering
	{
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_BLUR();
	}
};
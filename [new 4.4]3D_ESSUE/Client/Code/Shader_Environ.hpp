#include "Shader_base.hpp"

vector		g_vMaterialDiffuse;
vector		g_vMaterialAmbient;
vector		g_vMaterialSpecular;

float		g_fPower = 50.f;
float		g_fOffset;

vector		g_vCamPos;
matrix		g_matLight;

vector		g_vColor;

float		g_fTime;
float		g_fHeight;
float		g_fSpeed;
float		g_fFrequency;

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

Texture		g_NormalTexture;

sampler NormalSampler = sampler_state
{
	texture = g_NormalTexture;
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
	float3	vWorldTangent : TEXCOORD4;
	float3	vWorldBiNormal : TEXCOORD5;
};

struct PS_IN
{
	vector	vNormal : NORMAL;
	float2	vTexUV : TEXCOORD0;
	vector	vWorldPos : TEXCOORD1;
	vector	vProjPos : TEXCOORD2;
	vector	vShadowPos : TEXCOORD3;
	float3	vWorldTangent : TEXCOORD4;
	float3	vWorldBiNormal : TEXCOORD5;
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

VS_OUT VS_BUMP(VS_IN In)
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

	float3 vNormal = float3(Out.vNormal.x, Out.vNormal.y, Out.vNormal.z);
	float3 vBiNormal = float3(Out.vNormal.y, -Out.vNormal.x, 0.f);

	Out.vWorldTangent = cross(vBiNormal, vNormal);
	Out.vWorldBiNormal = vBiNormal;

	return Out;
}

VS_OUT VS_Aura(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0.f;

	matrix matWV, matWVP;
	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPos = mul(vector(In.vPos.xyz, 1.f), matWVP);
	Out.vNormal = normalize(mul(vector(In.vNormal.xyz, 0.f), g_matWorld));
	Out.vTexUV = In.vTexUV;
	Out.vTexUV.y -= g_fOffset;
	Out.vWorldPos = mul(vector(In.vPos.xyz, 1.f), g_matWorld);
	Out.vProjPos = Out.vPos;

	return Out;
}

VS_OUT VS_Fall(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0.f;

	matrix matWV, matWVP;
	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPos = mul(vector(In.vPos.xyz, 1.f), matWVP);
	Out.vNormal = normalize(mul(vector(In.vNormal.xyz, 0.f), g_matWorld));
	Out.vTexUV = In.vTexUV;
	Out.vTexUV.y -= g_fOffset;
	Out.vWorldPos = mul(vector(In.vPos.xyz, 1.f), g_matWorld);
	Out.vProjPos = Out.vPos;

	return Out;
}

VS_OUT VS_Flow(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0.f;

	float cosTime = g_fHeight * cos(g_fTime * g_fSpeed + In.vTexUV.x * g_fFrequency + In.vTexUV.y * g_fFrequency);

	vector vPos = In.vPos;
	vPos.y += cosTime;

	matrix matWV, matWVP;
	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPos = mul(vector(vPos.xyz, 1.f), matWVP);
	Out.vNormal = normalize(mul(vector(In.vNormal.xyz, 0.f), g_matWorld));
	Out.vTexUV = In.vTexUV * 0.3f - g_fOffset;
	Out.vWorldPos = mul(vector(In.vPos.xyz, 1.f), g_matWorld);
	Out.vProjPos = Out.vPos;

	return Out;
}

VS_OUT VS_DRAWSHADOW(VS_IN In)
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

	matrix matLightWV, matLightWVP;
	matLightWV = mul(g_matWorld, g_matLight);
	matLightWVP = mul(matLightWV, g_matProj);
	Out.vShadowPos = mul(vector(In.vPos.xyz, 1.f), matLightWVP);

	return Out;
}

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;

	Out.vColor = tex2D(BaseSampler, In.vTexUV) + g_vColor;
	//Out.vColor.a = 1.f;

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	Out.vDepth = vector(In.vProjPos.w * 0.001f, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);

	return Out;
}

PS_OUT PS_BLEND(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;

	Out.vColor = tex2D(BaseSampler, In.vTexUV) + g_vColor;

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	Out.vDepth = vector(In.vProjPos.w * 0.001f, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);

	return Out;
}


PS_OUT PS_AURA(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);
	Out.vColor.a += 0.3f;

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	Out.vDepth = vector(In.vProjPos.w * 0.001f, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);

	return Out;
}

PS_OUT PS_DRAWSHADOW(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;

	float2 ShadowUV = float2(In.vShadowPos.x / In.vShadowPos.w, In.vShadowPos.y / In.vShadowPos.w);
	ShadowUV.x = ShadowUV.x * 0.5 + 0.5;
	ShadowUV.y = ShadowUV.y * -0.5 + 0.5;

	Out.vColor = tex2D(BaseSampler, In.vTexUV) * max(tex2D(ShadowSampler, ShadowUV), 0.3f);

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	Out.vDepth = vector(In.vProjPos.w * 0.001f, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);

	return Out;
}

PS_OUT PS_BUMP(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;

	vector vNormal = tex2D(NormalSampler, In.vTexUV);
	vNormal = normalize(vNormal * 2.f - 1.f);

	float3x3 matTranspose = float3x3(normalize(In.vWorldTangent), normalize(In.vWorldBiNormal), normalize(In.vNormal.xyz));
	matTranspose = transpose(matTranspose);
	vector vWorldNormal = vector(mul(vNormal, matTranspose), 0.f);

	vector	vTexColor = tex2D(BaseSampler, In.vTexUV);

	vector	vShade = max(dot(normalize(g_vLightDir) * -1.f, vWorldNormal), 0.f);
	vector	vReflect = reflect(normalize(g_vLightDir), vWorldNormal);
	vector	vLook = In.vWorldPos - g_vCamPos;
	vector	vSpecular = pow(max(dot(normalize(vLook) * -1.f, vReflect), 0.f), g_fPower);

	Out.vColor = (vTexColor * (g_vLightDiffuse * g_vMaterialDiffuse)) * (vShade + (g_vLightAmbient * g_vMaterialAmbient))
		+ vSpecular * (g_vLightSpecular * g_vMaterialSpecular);
	Out.vNormal = (vector)1.f;

	/*Out.vColor = tex2D(BaseSampler, In.vTexUV);

	Out.vNormal = vector(vWorldNormal.xyz * 0.5f + 0.5f, 0.f);

	Out.vDepth = vector(In.vProjPos.w * 0.001f, In.vProjPos.z / In.vProjPos.w, 0.f, 0.f);*/

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

	pass Pillar_Rendering
	{
		CullMode = ccw;
		Lighting = false;

		AlphaBlendEnable = true;
		SrcBlend = srcalpha;
		DestBlend = one;

		VertexShader = compile vs_3_0 VS_Fall();
		PixelShader = compile ps_3_0 PS_BLEND();
	}

	pass Fall_Rendering
	{
		Lighting = false;
		CullMode = none;

		AlphaBlendEnable = true;
		SrcBlend = srcalpha;
		DestBlend = invsrcalpha;

		VertexShader = compile vs_3_0 VS_Fall();
		PixelShader = compile ps_3_0 PS_BLEND();
	}

	pass Flow_Rendering
	{
		Lighting = false;
		CullMode = none;

		AlphaBlendEnable = true;
		SrcBlend = srcalpha;
		DestBlend = one;

		VertexShader = compile vs_3_0 VS_Flow();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass ShadowDraw_Rendering
	{
		CullMode = ccw;
		Lighting = true;

		VertexShader = compile vs_3_0 VS_DRAWSHADOW();
		PixelShader = compile ps_3_0 PS_DRAWSHADOW();
	}

	pass Aura_Rendering
	{
		Lighting = false;
		CullMode = none;

		AlphaBlendEnable = true;
		SrcBlend = srcalpha;
		DestBlend = one;

		VertexShader = compile vs_3_0 VS_Aura();
		PixelShader = compile ps_3_0 PS_AURA();
	}

	pass Bump_Rendering
	{
		Lighting = false;
		CullMode = none;

		VertexShader = compile vs_3_0 VS_BUMP();
		PixelShader = compile ps_3_0 PS_BUMP();
	}
}
#include "Shader_base.hpp"

vector		g_vMaterialDiffuse;
vector		g_vMaterialAmbient;
vector		g_vMaterialSpecular;
float		g_fPower = 10.f;

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
	vector	vShade : COLOR0;
	vector	vSpecular : COLOR1;
	float2	vTexUV : TEXCOORD0;
};

struct PS_IN
{
	vector	vShade : COLOR0;
	vector	vSpecular : COLOR1;
	float2	vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	vector vColor : COLOR0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0.f;

	matrix matWV, matWVP;
	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPos = mul(vector(In.vPos.xyz, 1.f), matWVP);

	vector vWorldNormal = mul(vector(In.vNormal.xyz, 0.f), g_matWorld);
	Out.vShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(vWorldNormal)), 0);

	vector vWorldPos = mul(vector(In.vPos.xyz, 1.f), g_matWorld);
	vector vReflect = reflect(normalize(g_vLightDir), normalize(vWorldNormal));
	vector vLook = vWorldPos - g_vCamPos;

	Out.vSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0), g_fPower);

	Out.vTexUV = In.vTexUV;

	return Out;
}

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;

	vector	vTexColor = tex2D(BaseSampler, In.vTexUV);

	vector vDiffuse = vTexColor * g_vLightDiffuse * g_vMaterialDiffuse;
	vector vAmbient = In.vShade + (g_vLightAmbient * g_vMaterialAmbient);
	vector vSpecular = In.vSpecular * (g_vLightSpecular * g_vMaterialSpecular);

	Out.vColor = (vDiffuse * vAmbient) + vSpecular;


	return Out;
}

technique Default_Device
{
	pass Default_Rendering
	{
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}
};
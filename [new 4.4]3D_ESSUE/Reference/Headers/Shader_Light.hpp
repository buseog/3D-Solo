
vector		g_vLightDir;

vector		g_vLightPos;
float		g_fRange;


vector		g_vLightDiffuse;
vector		g_vLightAmbient;
vector		g_vLightSpecular;

vector		g_vMtrDiffuse = (vector)1.f;
vector		g_vMtrAmbient = (vector)1.f;
vector		g_vMtrSpecular = (vector)1.f;

vector		g_vCamPos;
vector		g_fPower = 50.f;

matrix		g_matInvView;
matrix		g_matInvProj;

texture		g_NormalTexture;

sampler NormalSampler = sampler_state
{
	texture = g_NormalTexture;
};

texture		g_DepthTexture;

sampler DepthSampler = sampler_state
{
	texture = g_DepthTexture;
};

struct PS_IN
{
	float2 vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	vector vShade : COLOR0;
	vector vSpecular : COLOR1;
};

PS_OUT PS_DIRECTION(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0.f;

	vector	vNormal = tex2D(NormalSampler, In.vTexUV);
	if (vNormal.w == 1.f)
	{
		Out.vShade = (vector)1.f;
		return Out;
	}

	vNormal = vector(vNormal.xyz * 2.f - 1.f, 0.f);

	vector	vShade = max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f);

	Out.vShade = (vShade + (g_vLightAmbient * g_vMtrAmbient)) * (g_vLightDiffuse * g_vMtrDiffuse);

	vector	vReflect = reflect(normalize(g_vLightDir), vNormal);

	vector	vWorldPos;

	vector	vDepth = tex2D(DepthSampler, In.vTexUV);

	float	fViewZ = vDepth.x * 1000.f;

	vWorldPos.x = (In.vTexUV.x * 2.f - 1.f) * fViewZ;
	vWorldPos.y = (In.vTexUV.y * -2.f + 1.f) * fViewZ;
	vWorldPos.z = vDepth.y * fViewZ;
	vWorldPos.w = fViewZ;

	vWorldPos = mul(vWorldPos, g_matInvProj);
	vWorldPos = mul(vWorldPos, g_matInvView);

	vector		vLook = vWorldPos - g_vCamPos;

	Out.vSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), g_fPower);

	return Out;
}

PS_OUT PS_POINT(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0.f;

	vector	vNormal = tex2D(NormalSampler, In.vTexUV);
	if (vNormal.w == 1.f)
	{
		Out.vShade = (vector)1.f;
		return Out;
	}

	vNormal = vector(vNormal.xyz * 2.f - 1.f, 0.f);

	vector	vDepth = tex2D(DepthSampler, In.vTexUV);

	vector	vWorldPos;

	float	fViewZ = vDepth.x * 1000.f;

	vWorldPos.x = (In.vTexUV.x * 2.f - 1.f) * fViewZ;
	vWorldPos.y = (In.vTexUV.y * -2.f + 1.f) * fViewZ;
	vWorldPos.z = vDepth.y * fViewZ;
	vWorldPos.w = fViewZ;

	vWorldPos = mul(vWorldPos, g_matInvProj);
	vWorldPos = mul(vWorldPos, g_matInvView);

	vector		vLightDir = vWorldPos - g_vLightPos;

	float		fDistance = length(vLightDir);

	float		fAtt = max((g_fRange - fDistance) / g_fRange, 0.f);

	vector		vShade = max(dot(normalize(vLightDir) * -1.f, vNormal), 0.f) * fAtt;	

	Out.vShade = (vShade + (g_vLightAmbient * g_vMtrAmbient)) * (g_vLightDiffuse * g_vMtrDiffuse);

	vector		vReflect = reflect(normalize(vLightDir), vNormal);

	vector		vLook = vWorldPos - g_vCamPos;

	Out.vSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), g_fPower) * (g_vLightSpecular * g_vMtrSpecular)  * fAtt;

	return Out;
}

technique Default_Device
{
	pass Direction_Rendering
	{
		AlphaBlendEnable = true;
		SrcBlend = one;
		DestBlend = one;

		ZEnable = false;
		ZWriteEnable = false;

		VertexShader = NULL; 
		PixelShader = compile ps_3_0 PS_DIRECTION();
	}	

	pass Point_Rendering
	{
		AlphaBlendEnable = true;
		SrcBlend = one;
		DestBlend = one;

		ZEnable = false;
		ZWriteEnable = false;

		VertexShader = NULL; 
		PixelShader = compile ps_3_0 PS_POINT();
	}	
};
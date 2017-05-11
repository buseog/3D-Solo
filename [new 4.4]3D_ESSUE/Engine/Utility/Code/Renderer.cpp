#include "Renderer.h"
#include "GameObject.h"
#include "Transform.h"
#include "..\Reference\Headers\Shader.h"
#include "..\Reference\Headers\ShaderBuffer.h"
#include "TargetMgr.h"
#include "Target.h"
#include "LightMgr.h"

USING(Engine)

Engine::CRenderer::CRenderer(LPDIRECT3DDEVICE9 pDevice)
: m_pDevice(pDevice)
, m_pShaderBuffer(NULL)
, m_pShader_Gaussian(NULL)
, m_pShader_Refraction(NULL)
, m_pShader_Light(NULL)
, m_pShader_Blend(NULL)
, m_bGaussian(false)
, m_bDebug(false)
{
	m_pDevice->AddRef();
}

Engine::CRenderer::~CRenderer(void)
{

}

HRESULT CRenderer::Initialize(void)
{
	m_pShader_Gaussian = CShader::Create(m_pDevice, L"../../Reference/Headers/Shader_Gaussian.hpp");
	if (m_pShader_Gaussian == NULL)
		return E_FAIL;

	m_pShader_Refraction = CShader::Create(m_pDevice, L"../../Reference/Headers/Shader_Refraction.hpp");
	if (m_pShader_Gaussian == NULL)
		return E_FAIL;

	m_pShader_Light = CShader::Create(m_pDevice, L"../../Reference/Headers/Shader_Light.hpp");
	if (m_pShader_Gaussian == NULL)
		return E_FAIL;

	m_pShader_Blend = CShader::Create(m_pDevice, L"../../Reference/Headers/Shader_Blend.hpp");
	if (m_pShader_Gaussian == NULL)
		return E_FAIL;

	D3DVIEWPORT9	ViewPort;
	m_pDevice->GetViewport(&ViewPort);

	if (FAILED(Engine::CTargetMgr::GetInstance()->Init_Target(m_pDevice, L"Target_Albedo", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(Engine::CTargetMgr::GetInstance()->Init_Debug(L"Target_Albedo", 0.f, 0.f, 200.f, 200.f)))
		return E_FAIL;
	
	if (FAILED(Engine::CTargetMgr::GetInstance()->Init_Target(m_pDevice, L"Target_Normal", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(Engine::CTargetMgr::GetInstance()->Init_Debug(L"Target_Normal", 0.f, 200.f, 200.f, 200.f)))
		return E_FAIL;
	
	if (FAILED(Engine::CTargetMgr::GetInstance()->Init_Target(m_pDevice, L"Target_Depth", ViewPort.Width, ViewPort.Height, D3DFMT_A32B32G32R32F, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(Engine::CTargetMgr::GetInstance()->Init_Debug(L"Target_Depth", 0.f, 400.f, 200.f, 200.f)))
		return E_FAIL;

	if (FAILED(Engine::CTargetMgr::GetInstance()->Init_Target(m_pDevice, L"Target_Shadow", ViewPort.Width, ViewPort.Height, D3DFMT_A32B32G32R32F, D3DXCOLOR(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(Engine::CTargetMgr::GetInstance()->Init_Debug(L"Target_Shadow", 200.f, 400.f, 200.f, 200.f)))
		return E_FAIL;


	if (FAILED(Engine::CTargetMgr::GetInstance()->Init_Target(m_pDevice, L"Target_Shade", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(Engine::CTargetMgr::GetInstance()->Init_Debug(L"Target_Shade", 200.f, 0.f, 200.f, 200.f)))
		return E_FAIL;

	if (FAILED(Engine::CTargetMgr::GetInstance()->Init_Target(m_pDevice, L"Target_Specular", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(Engine::CTargetMgr::GetInstance()->Init_Debug(L"Target_Specular", 200.f, 200.f, 200.f, 200.f)))
		return E_FAIL;

	if (FAILED(Engine::CTargetMgr::GetInstance()->Init_Target(m_pDevice, L"Target_Gaussian", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(Engine::CTargetMgr::GetInstance()->Init_Debug(L"Target_Gaussian", 600.f, 0.f, 200.f, 200.f)))
		return E_FAIL;

	if (FAILED(Engine::CTargetMgr::GetInstance()->Init_Target(m_pDevice, L"Target_Blur", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(Engine::CTargetMgr::GetInstance()->Init_Debug(L"Target_Blur", 600.f, 200.f, 200.f, 200.f)))
		return E_FAIL;


	if (FAILED(Engine::CTargetMgr::GetInstance()->Init_Target(m_pDevice, L"Target_Effect", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(Engine::CTargetMgr::GetInstance()->Init_Debug(L"Target_Effect", 400.f, 0.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(Engine::CTargetMgr::GetInstance()->Init_Target(m_pDevice, L"Target_Refraction", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(Engine::CTargetMgr::GetInstance()->Init_Debug(L"Target_Refraction", 400.f, 200.f, 200.f, 200.f)))
		return E_FAIL;


	if (FAILED(Engine::CTargetMgr::GetInstance()->Init_MRT(L"MRT_Deferred", L"Target_Albedo")))
		return E_FAIL;
	if(FAILED(Engine::CTargetMgr::GetInstance()->Init_MRT(L"MRT_Deferred", L"Target_Normal")))
		return E_FAIL;
	if(FAILED(Engine::CTargetMgr::GetInstance()->Init_MRT(L"MRT_Deferred", L"Target_Depth")))
		return E_FAIL;

	if (FAILED(Engine::CTargetMgr::GetInstance()->Init_MRT(L"MRT_Light", L"Target_Shade")))
		return E_FAIL;
	if(FAILED(Engine::CTargetMgr::GetInstance()->Init_MRT(L"MRT_Light", L"Target_Specular")))
		return E_FAIL;

	if (FAILED(Engine::CTargetMgr::GetInstance()->Init_MRT(L"MRT_Gaussian", L"Target_Gaussian")))
		return E_FAIL;

	if (FAILED(Engine::CTargetMgr::GetInstance()->Init_MRT(L"MRT_Refraction", L"Target_Effect")))
		return E_FAIL;
	if (FAILED(Engine::CTargetMgr::GetInstance()->Init_MRT(L"MRT_Refraction", L"Target_Refraction")))
		return E_FAIL;



	m_pShaderBuffer = CShaderBuffer::Create(m_pDevice, ViewPort.Width, ViewPort.Height);
	if (m_pShaderBuffer == NULL)
		return E_FAIL;

	return S_OK;
}

void Engine::CRenderer::Render(void)
{
	if (CTargetMgr::GetInstance()->GetTarget(L"Target_Blur")->GetExcept() == false)
		CTargetMgr::GetInstance()->GetTarget(L"Target_Blur")->ClearTarget();

	Render_Priority();

	Render_Deferred();

	Render_Light();

	Render_Blend();
	
	//Render_Alpha();
	Render_Refraction();
	Render_UI();
	
	if (m_bDebug == true)
	{
		CTargetMgr::GetInstance()->Render_Debug(L"Target_Albedo");
		CTargetMgr::GetInstance()->Render_Debug(L"Target_Normal");
		CTargetMgr::GetInstance()->Render_Debug(L"Target_Depth");

		CTargetMgr::GetInstance()->Render_Debug(L"Target_Shade");
		CTargetMgr::GetInstance()->Render_Debug(L"Target_Specular");
		CTargetMgr::GetInstance()->Render_Debug(L"Target_Shadow");

		CTargetMgr::GetInstance()->Render_Debug(L"Target_Gaussian");
		CTargetMgr::GetInstance()->Render_Debug(L"Target_Blur");

		CTargetMgr::GetInstance()->Render_Debug(L"Target_Effect");
		CTargetMgr::GetInstance()->Render_Debug(L"Target_Refraction");
	}

	Clear();
}

void Engine::CRenderer::Render_Priority(void)
{
	RENDERLIST::iterator iter = m_RenderList[RENDER_PRIORITY].begin();
	RENDERLIST::iterator iter_end = m_RenderList[RENDER_PRIORITY].end();

	for ( ; iter != iter_end; ++iter)
	{
		(*iter)->Render();
	}
}

void Engine::CRenderer::Render_NoneAlpha(void)
{
	RENDERLIST::iterator iter = m_RenderList[RENDER_NONEALPHA].begin();
	RENDERLIST::iterator iter_end = m_RenderList[RENDER_NONEALPHA].end();

	for ( ; iter != iter_end; ++iter)
	{
		(*iter)->Render();
	}
}

_bool Compare(CGameObject* pSrc, CGameObject* pDest)
{
	return ((CTransform*)pSrc->GetComponent(L"Com_Transform"))->GetViewZ() > ((CTransform*)pDest->GetComponent(L"Com_Transform"))->GetViewZ();
}

void Engine::CRenderer::Render_Alpha(void)
{
	m_RenderList[RENDER_NONEALPHA].sort(Compare);

	RENDERLIST::iterator iter = m_RenderList[RENDER_ALPHA].begin();
	RENDERLIST::iterator iter_end = m_RenderList[RENDER_ALPHA].end();

	for ( ; iter != iter_end; ++iter)
	{
		(*iter)->Render();
	}
}

void Engine::CRenderer::Render_UI(void)
{
	RENDERLIST::iterator iter = m_RenderList[RENDER_UI].begin();
	RENDERLIST::iterator iter_end = m_RenderList[RENDER_UI].end();

	for ( ; iter != iter_end; ++iter)
	{
		(*iter)->Render();
	}
}

void Engine::CRenderer::Render_Deferred(void)
{
	CTargetMgr::GetInstance()->BeginMRT(L"MRT_Deferred");

	Render_NoneAlpha();

	CTargetMgr::GetInstance()->EndMRT(L"MRT_Deferred");
}

void Engine::CRenderer::Render_Light(void)
{
	CTargetMgr::GetInstance()->BeginMRT(L"MRT_Light");

	LPD3DXEFFECT pEffect = m_pShader_Light->GetEffect();
	if (pEffect == NULL)
		return;
	pEffect->AddRef();

	CTargetMgr::GetInstance()->SetUpShader(pEffect, L"Target_Normal", "g_NormalTexture");
	CTargetMgr::GetInstance()->SetUpShader(pEffect, L"Target_Depth", "g_DepthTexture");

	pEffect->Begin(NULL, 0);
	pEffect->BeginPass(0);

	CLightMgr::GetInstance()->Render(pEffect);

	pEffect->EndPass();
	pEffect->End();

	Engine::Safe_Release(pEffect);
	CTargetMgr::GetInstance()->EndMRT(L"MRT_Light");
}

void Engine::CRenderer::Render_Blend(void)
{
	CTargetMgr::GetInstance()->BeginMRT(L"MRT_Refraction");

	if (m_bGaussian == true)
		CTargetMgr::GetInstance()->BeginMRT(L"MRT_Gaussian");

	LPD3DXEFFECT pEffect = m_pShader_Blend->GetEffect();
	if (pEffect == NULL)
		return;
	pEffect->AddRef();

	CTargetMgr::GetInstance()->SetUpShader(pEffect, L"Target_Albedo", "g_AlbedoTexture");
	CTargetMgr::GetInstance()->SetUpShader(pEffect, L"Target_Shade", "g_ShadeTexture");
	CTargetMgr::GetInstance()->SetUpShader(pEffect, L"Target_Specular", "g_SpecularTexture");
	CTargetMgr::GetInstance()->SetUpShader(pEffect, L"Target_Blur", "g_BlurTexture");

	pEffect->Begin(NULL, 0);
	pEffect->BeginPass(0);

	m_pShaderBuffer->Render();

	pEffect->EndPass();
	pEffect->End();

	Engine::Safe_Release(pEffect);

	if (m_bGaussian == true)
		Render_Gaussian();
}

void Engine::CRenderer::Render_Refraction(void)
{
	Render_Alpha();

	CTargetMgr::GetInstance()->EndMRT(L"MRT_Refraction");

	LPD3DXEFFECT pEffect = m_pShader_Refraction->GetEffect();
	if(NULL == pEffect)
		return;
	pEffect->AddRef();

	CTargetMgr::GetInstance()->SetUpShader(pEffect, L"Target_Effect", "g_BaseTexture");
	CTargetMgr::GetInstance()->SetUpShader(pEffect, L"Target_Refraction", "g_RefractionTexture");

	pEffect->Begin(0, NULL);
	pEffect->BeginPass(0);

	m_pShaderBuffer->Render();

	pEffect->EndPass();
	pEffect->End();

	Engine::Safe_Release(pEffect);
}

void Engine::CRenderer::Render_Gaussian( void )
{
	CTargetMgr::GetInstance()->EndMRT(L"MRT_Gaussian");

	LPD3DXEFFECT pEffect = m_pShader_Gaussian->GetEffect();
	if(NULL == pEffect)
		return;
	pEffect->AddRef();

	CTargetMgr::GetInstance()->SetUpShader(pEffect, L"Target_Gaussian", "g_GaussianTexture");

	pEffect->Begin(0, NULL);
	pEffect->BeginPass(0);

	m_pShaderBuffer->Render();

	pEffect->EndPass();
	pEffect->End();

	Engine::Safe_Release(pEffect);
}

void Engine::CRenderer::Clear(void)
{
	for (_int i = 0; i < RENDER_END; ++i)
	{
		for_each(m_RenderList[i].begin(), m_RenderList[i].end(), CReleaseSingle());
		m_RenderList[i].clear();
	}
}

HRESULT Engine::CRenderer::AddRenderList(RENDERTYPE eTpye, CGameObject* pGameObject)
{
	if (pGameObject == NULL)
		return E_FAIL;

	m_RenderList[eTpye].push_back(pGameObject);

	pGameObject->AddRef();

	return S_OK;
}

void CRenderer::Free(void)
{
	Engine::Safe_Release(m_pShader_Gaussian);
	Engine::Safe_Release(m_pShader_Refraction);
	Engine::Safe_Release(m_pShader_Light);
	Engine::Safe_Release(m_pShader_Blend);
	Engine::Safe_Release(m_pShaderBuffer);

	for (_int i = 0; i < RENDER_END; ++i)
	{
		for_each(m_RenderList[i].begin(), m_RenderList[i].end(), CReleaseSingle());
		m_RenderList[i].clear();
	}

	Engine::Safe_Release(m_pDevice);
}

CComponent* CRenderer::Clone(void)
{
	AddRef();

	return this;
}

CRenderer* CRenderer::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CRenderer* pRenderer =  new CRenderer(pDevice);

	if (FAILED(pRenderer->Initialize()))
	{
		MSG_BOX("Renderer Created Failed");
		Engine::Safe_Release(pRenderer);
	}

	return pRenderer;
}

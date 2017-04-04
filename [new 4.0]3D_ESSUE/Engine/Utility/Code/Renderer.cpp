#include "Renderer.h"
#include "GameObject.h"
#include "Transform.h"
#include "..\Reference\Headers\Shader.h"
#include "..\Reference\Headers\ShaderBuffer.h"

USING(Engine)

Engine::CRenderer::CRenderer(LPDIRECT3DDEVICE9 pDevice)
: m_pDevice(pDevice)
, m_pShaderBuffer(NULL)
, m_pShader(NULL)
{
	m_pDevice->AddRef();
}

Engine::CRenderer::~CRenderer(void)
{

}

HRESULT CRenderer::Initialize(void)
{
	m_pShader = CShader::Create(m_pDevice, L"../../Reference/Headers/Shader_Gaussian.hpp");
	if (m_pShader == NULL)
		return E_FAIL;

	D3DVIEWPORT9	ViewPort;
	m_pDevice->GetViewport(&ViewPort);

	m_pShaderBuffer = CShaderBuffer::Create(m_pDevice, ViewPort.Width, ViewPort.Height);
	if (m_pShaderBuffer == NULL)
		return E_FAIL;

	if(FAILED(D3DXCreateTexture(m_pDevice, ViewPort.Width, ViewPort.Height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pTargetTexture[0])))
		return E_FAIL;
	m_pTargetTexture[0]->GetSurfaceLevel(0, &m_pTargetSurface[0]);

	if(FAILED(D3DXCreateTexture(m_pDevice, ViewPort.Width, ViewPort.Height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pTargetTexture[1])))
		return E_FAIL;
	m_pTargetTexture[1]->GetSurfaceLevel(0, &m_pTargetSurface[1]);

	return S_OK;
}

void Engine::CRenderer::Render(void)
{
	/*LPDIRECT3DSURFACE9 pBackBuffer = NULL;
	if (GetAsyncKeyState(VK_SPACE))
	{
	m_pDevice->GetRenderTarget(0, &pBackBuffer);

	m_pDevice->SetRenderTarget(0, m_pTargetSurface[0]);
	m_pDevice->SetRenderTarget(1, m_pTargetSurface[1]);

	m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DXCOLOR(0.f, 0.f, 0.f, 1.f), 1.f, 0);
	}*/
	Render_Priority();
	Render_NoneAlpha();

	/*if (GetAsyncKeyState(VK_SPACE))
	{
	m_pDevice->SetRenderTarget(0, pBackBuffer);

	LPD3DXEFFECT	pEffect = m_pShader->GetEffect();
	if(NULL == pEffect)
		return;

	pEffect->SetTexture("g_BaseTexture", m_pTargetTexture[0]); 
	pEffect->SetTexture("g_GaussianTexture", m_pTargetTexture[1]); 

	pEffect->Begin(0, NULL);
	pEffect->BeginPass(0);

	m_pShaderBuffer->Render();

	pEffect->EndPass();
	pEffect->End();
	}*/

	//Engine::Safe_Release(pEffect);

	Render_Alpha();
	Render_UI();

	//Engine::Safe_Release(m_pDevice);
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
	Engine::Safe_Release(m_pShader);
	Engine::Safe_Release(m_pShaderBuffer);

	for (_int i = 0; i < 2; ++i)
	{
		Engine::Safe_Release(m_pTargetTexture[i]);
		Engine::Safe_Release(m_pTargetSurface[i]);
	}

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

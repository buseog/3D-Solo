#include "Renderer.h"
#include "GameObject.h"

USING(Engine)

Engine::CRenderer::CRenderer(LPDIRECT3DDEVICE9 pDevice)
: m_pDevice(pDevice)
{
	m_pDevice->AddRef();
}

Engine::CRenderer::~CRenderer(void)
{

}

HRESULT CRenderer::Initialize(void)
{
	return S_OK;
}

void Engine::CRenderer::Render(void)
{
	Render_Priority();
	Render_NoneAlpha();
	Render_Alpha();
	Render_UI();

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

void Engine::CRenderer::Render_Alpha(void)
{
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

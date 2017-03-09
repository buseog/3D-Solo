#include "Management.h"
#include "Scene.h"
#include "ComponentMgr.h"
#include "Renderer.h"

USING(Engine)
IMPLEMENT_SINGLETON(CManagement)

Engine::CManagement::CManagement(void)
: m_pScene(NULL)
, m_pRenderer(NULL)
{

}

Engine::CManagement::~CManagement(void)
{

}

HRESULT Engine::CManagement::InitManagement(void)
{
	m_pRenderer = (Engine::CRenderer*)CComponentMgr::GetInstance()->CloneComponent(0, L"Component_Renderer");
	if(NULL == m_pRenderer)
		return E_FAIL;

	return S_OK;
}

_int Engine::CManagement::Update(const _float& fDelta)
{
	if (m_pScene == NULL)
		return -1;

	return m_pScene->Update(fDelta);
}

void Engine::CManagement::Render(void)
{
	m_pRenderer->Render();

	if (m_pScene == NULL)
		return;

	m_pScene->Render();
}

HRESULT Engine::CManagement::SetScene(CScene* pScene)
{
	if (pScene == NULL)
		return E_FAIL;

	if (m_pScene != NULL)
	{
		if (Engine::Safe_Release(m_pScene) != 0)
			MSG_BOX("Scene Release Failed");
	}

	m_pScene = pScene;

	return S_OK;
}

void Engine::CManagement::Free(void)
{
	Engine::Safe_Release(m_pRenderer);

	Engine::Safe_Release(m_pScene);
}

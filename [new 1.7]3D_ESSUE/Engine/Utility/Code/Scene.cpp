#include "Scene.h"
#include "Layer.h"

USING(Engine)

Engine::CScene::CScene(LPDIRECT3DDEVICE9 pDevice)
: m_pDevice(pDevice)
{
	m_pDevice->AddRef();
}

Engine::CScene::~CScene(void)
{

}

HRESULT Engine::CScene::Initialize(void)
{
	return S_OK;
}

_int Engine::CScene::Update(const _float& fDelta)
{
	MAPLAYER::iterator iter = m_mapLayer.begin();
	MAPLAYER::iterator iter_end = m_mapLayer.end();

	_int iExit = 0;

	for ( ; iter != iter_end; ++iter)
	{
		iExit = iter->second->Update(fDelta);

		if (iExit < 0)
			return iExit;
	}

	return 0;
}

CLayer* Engine::CScene::FindLayer(const _tchar* pLayerKey)
{
	MAPLAYER::iterator iter = find_if(m_mapLayer.begin(), m_mapLayer.end(), CTagFinder(pLayerKey));

	if (iter == m_mapLayer.end())
		return NULL;

	return iter->second;
}

HRESULT Engine::CScene::AddLayer(const _tchar* pLayerKey, CLayer* pLayer)
{
	CLayer* pTemp = FindLayer(pLayerKey);

	if (pTemp != NULL)
		return E_FAIL;

 	m_mapLayer.insert(MAPLAYER::value_type(pLayerKey, pLayer));

	return S_OK;
}

void Engine::CScene::Free(void)
{
	for_each(m_mapLayer.begin(), m_mapLayer.end(), CReleasePair());
	m_mapLayer.clear();

	Engine::Safe_Release(m_pDevice);
}

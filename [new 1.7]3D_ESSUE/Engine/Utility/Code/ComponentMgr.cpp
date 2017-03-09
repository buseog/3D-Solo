#include "ComponentMgr.h"
#include "Component.h"

USING(Engine)
IMPLEMENT_SINGLETON(CComponentMgr)

Engine::CComponentMgr::CComponentMgr(void)
: m_pMapComponent(NULL)
, m_iContainerSize(0)
{

}

Engine::CComponentMgr::~CComponentMgr(void)
{

}

HRESULT Engine::CComponentMgr::ReserveComponentMgr(const _uint& iSize)
{
	if (m_pMapComponent != NULL)
	{
		MSG_BOX("");
		return E_FAIL;
	}

	m_pMapComponent = new MAPCOMPONENT[iSize];
	m_iContainerSize = iSize;

	return S_OK;
}

CComponent* Engine::CComponentMgr::FindComponent(const _uint& iIndex, const _tchar* Componentkey)
{
	MAPCOMPONENT::iterator iter = find_if(m_pMapComponent[iIndex].begin(), m_pMapComponent[iIndex].end(), CTagFinder(Componentkey));

	if (iter == m_pMapComponent[iIndex].end())
		return NULL;

	return iter->second;
}

HRESULT Engine::CComponentMgr::AddComponent(const _uint& iIndex, const _tchar* ComponentKey, CComponent* pComponent)
{
	CComponent* pTemp = FindComponent(iIndex, ComponentKey);

	if (pTemp != NULL)
		return E_FAIL;

	m_pMapComponent[iIndex].insert(MAPCOMPONENT::value_type(ComponentKey, pComponent));

	return S_OK;
}


Engine::CComponent* Engine::CComponentMgr::CloneComponent(const _uint& iIndex, const _tchar* ComponentKey)
{
	CComponent* pTemp = FindComponent(iIndex, ComponentKey);

	if (pTemp == NULL)
		return NULL;

	return pTemp->Clone();
}

void Engine::CComponentMgr::Clear( const _uint& iIndex )
{
	for_each(m_pMapComponent[iIndex].begin(), m_pMapComponent[iIndex].end(), CReleasePair());
	m_pMapComponent[iIndex].clear();
}

void Engine::CComponentMgr::Free(void)
{
	for (_uint i = 0; i < m_iContainerSize; ++i)
	{
		for_each(m_pMapComponent[i].begin(), m_pMapComponent[i].end(), CReleasePair());
		m_pMapComponent[i].clear();
	}

	Engine::Safe_DeleteArray(m_pMapComponent);
}

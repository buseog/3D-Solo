#include "GameObject.h"
#include "Component.h"

USING(Engine)

Engine::CGameObject::CGameObject( LPDIRECT3DDEVICE9 pDevice )
: m_pDevice(pDevice)
, m_bDestroy(false)
{
	m_pDevice->AddRef();
}

Engine::CGameObject::~CGameObject( void )
{

}

HRESULT Engine::CGameObject::Initialize( void )
{
	return S_OK;
}

_int Engine::CGameObject::Update( const _float& fDelta )
{
	for (_int i = 0; i < UPDATE_END; ++i)
	{
		MAPCOMPONENT::iterator iter = m_mapComponent[i].begin();
		MAPCOMPONENT::iterator iter_end = m_mapComponent[i].end();

		for ( ; iter != iter_end; ++iter)
		{
			iter->second->Update();
		}
	}

	return 0;
}

_int Engine::CGameObject::LastUpdate( const _float& fDelta )
{
	return 0;
}

void Engine::CGameObject::Render( void )
{

}

CComponent* Engine::CGameObject::FindComponent(UPDATETYPE eType, const _tchar* ComponentKey)
{
	MAPCOMPONENT::iterator iter = find_if(m_mapComponent[eType].begin(), m_mapComponent[eType].end(), CTagFinder(ComponentKey));

	if (iter == m_mapComponent[eType].end())
		return NULL;

	return iter->second;
}

HRESULT Engine::CGameObject::AddComponent(UPDATETYPE eType, const _tchar* ComponentKey, CComponent* pComponent)
{
	CComponent* pTemp = FindComponent(eType, ComponentKey);

	if (pTemp != NULL)
		return E_FAIL;

	m_mapComponent[eType].insert(MAPCOMPONENT::value_type(ComponentKey, pComponent));
	pComponent->AddRef();

	return S_OK;
}


void Engine::CGameObject::Free( void )
{
	for (_int i = 0; i < UPDATE_END; ++i)
	{
		for_each(m_mapComponent[i].begin(), m_mapComponent[i].end(), CReleasePair());
		m_mapComponent[i].clear();
	}

	Engine::Safe_Release(m_pDevice);
}

CComponent* Engine::CGameObject::GetComponent( const _tchar* CompoKey )
{
	CComponent* pFind = NULL;

	for (_uint i = 0; i < UPDATE_END; ++i)
	{
		pFind = FindComponent(UPDATETYPE(i), CompoKey);

		if (pFind != NULL)
			return pFind;
	}
	return NULL;
}

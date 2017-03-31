#include "Layer.h"
#include "GameObject.h"

USING(Engine)

Engine::CLayer::CLayer(void)
{

}

Engine::CLayer::~CLayer(void)
{

}

HRESULT Engine::CLayer::Initialize( void )
{
	return S_OK;
}

_int Engine::CLayer::Update( const _float& fDelta )
{
	OBJECTLIST::iterator iter = m_ObjectList.begin();
	OBJECTLIST::iterator iter_end = m_ObjectList.end();

	_int iExit = 0;

	for ( ; iter != iter_end; ++iter)
	{
		iExit = (*iter)->Update(fDelta);

		if (iExit < 0)
			return iExit;
	}

	return 0;
}

_int Engine::CLayer::LastUpdate( const _float& fDelta )
{
	OBJECTLIST::iterator iter = m_ObjectList.begin();
	OBJECTLIST::iterator iter_end = m_ObjectList.end();

	_int iExit = 0;

	for ( ; iter != iter_end;)
	{
		iExit = (*iter)->LastUpdate(fDelta);

		if ((*iter)->GetDestroy() == true)
		{
			Engine::Safe_Release(*iter);
			iter = m_ObjectList.erase(iter);
		}
		else
			++iter;

		if (iExit < 0)
			return iExit;
	}

	return 0;
}

void Engine::CLayer::AddGameObject(CGameObject* pGameObject)
{
	m_ObjectList.push_back(pGameObject);
}

CGameObject* Engine::CLayer::GetObject( const _uint& iIndex )
{
	OBJECTLIST::iterator iter = m_ObjectList.begin();

	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	if (iter == m_ObjectList.end())
		return NULL;

	return (*iter);
}

void Engine::CLayer::Free(void)
{
	for_each(m_ObjectList.begin(), m_ObjectList.end(), CReleaseSingle());
	m_ObjectList.clear();
}

CLayer* Engine::CLayer::Create(void)
{
	CLayer* pLayer = new CLayer;

	if (FAILED(pLayer->Initialize()))
	{
		MSG_BOX("CLayer Created Failed");
		Engine::Safe_Release(pLayer);
	}

	return pLayer;
}

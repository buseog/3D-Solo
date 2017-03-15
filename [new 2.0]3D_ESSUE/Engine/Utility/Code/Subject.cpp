#include "Subject.h"
#include "Observer.h"

USING(Engine)

Engine::CSubject::CSubject(void)
{
}

Engine::CSubject::~CSubject(void)
{
}

void Engine::CSubject::Subscribe( CObserver* pObserver )
{
	m_ObserverList.push_back(pObserver);
}

void Engine::CSubject::UnSubscribe( CObserver* pObserver )
{
	if (m_ObserverList.empty())
		return;
}

void Engine::CSubject::Notify( _uint iMessage, void* pData )
{
	OBSERVERLIST::iterator		iter		= m_ObserverList.begin();
	OBSERVERLIST::iterator		iter_end	= m_ObserverList.end();

	for( ; iter != iter_end; ++iter)
	{
		(*iter)->Update(iMessage, pData);
	}
}

void Engine::CSubject::Free( void )
{
	m_ObserverList.clear();
}

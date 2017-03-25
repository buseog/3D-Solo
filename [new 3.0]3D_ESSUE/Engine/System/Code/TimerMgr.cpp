#include "TimerMgr.h"
#include "Timer.h"

USING(Engine)
IMPLEMENT_SINGLETON(CTimerMgr)

Engine::CTimerMgr::CTimerMgr(void)
{

}

Engine::CTimerMgr::~CTimerMgr(void)
{

}

HRESULT Engine::CTimerMgr::InitTimerMgr(const _tchar* pTimerTag)
{
	CTimer*	pTimer = FindTimer(pTimerTag);
	if (pTimer != NULL)
		return E_FAIL;

	pTimer = CTimer::Create();
	if (pTimer == NULL)
		return E_FAIL;

	m_mapTimer.insert(MAPTIMER::value_type(pTimerTag, pTimer));

	return S_OK;
}


CTimer* Engine::CTimerMgr::FindTimer(const _tchar* pTimerTag)
{
	MAPTIMER::iterator iter = find_if(m_mapTimer.begin(), m_mapTimer.end(), CTagFinder(pTimerTag));

	if (iter == m_mapTimer.end())
		return NULL;

	return iter->second;
}

void Engine::CTimerMgr::SetUpTimer(const _tchar* pTimerTag)
{
	CTimer*	pTimer = FindTimer(pTimerTag);

	if (pTimer == NULL)
		return;

	pTimer->SetUpTimer();
}

_float Engine::CTimerMgr::GetTimeDelta(const _tchar* pTimerTag)
{
	CTimer*	pTimer = FindTimer(pTimerTag);

	if (pTimer == NULL)
		return 0.f;

	return pTimer->GetTimeDelta();
}

void Engine::CTimerMgr::Free(void)
{
	for_each(m_mapTimer.begin(), m_mapTimer.end(), CReleasePair());
	m_mapTimer.clear();
}
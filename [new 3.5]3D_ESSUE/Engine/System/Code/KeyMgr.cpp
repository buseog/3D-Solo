#include "KeyMgr.h"

USING(Engine)
IMPLEMENT_SINGLETON(CKeyMgr)

Engine::CKeyMgr::CKeyMgr(void)
{
	ZeroMemory(m_bKeyDown, sizeof(KEYMAX));
	ZeroMemory(m_bKeyUp, sizeof(KEYMAX));
}

Engine::CKeyMgr::~CKeyMgr(void)
{

}

bool	Engine::CKeyMgr::StayKeyDown(int nKey)
{
	if (GetAsyncKeyState(nKey) & 0x8000)
		return true;

	return false;
}

bool	Engine::CKeyMgr::KeyDown(int nKey, int iIndex)
{
	if (GetAsyncKeyState(nKey) & 0x8000)
	{
		if (m_bKeyDown[iIndex][nKey] == false)
		{
			m_bKeyDown[iIndex][nKey] = true;

			return true;
		}
	}
	else
	{
		m_bKeyDown[iIndex][nKey] = false;
	}

	return false;
}

bool	Engine::CKeyMgr::KeyUp(int nKey, int iIndex)
{
	if (GetAsyncKeyState(nKey) & 0x8000)
	{
		m_bKeyUp[iIndex][nKey] = true;
	}
	else if (m_bKeyUp[iIndex][nKey] == true)
	{
		m_bKeyUp[iIndex][nKey] = false;

		return true;
	}

	return false;
}

void Engine::CKeyMgr::Free(void)
{

}

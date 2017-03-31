#include "FrameMgr.h"
#include "Frame.h"

USING(Engine)
IMPLEMENT_SINGLETON(CFrameMgr)


Engine::CFrameMgr::CFrameMgr( void )
{

}

Engine::CFrameMgr::~CFrameMgr( void )
{

}

HRESULT Engine::CFrameMgr::InitFrameMgr(const _tchar* pFrameTag, const _float& fCallCnt)
{
	CFrame* pFrame = FindFrame(pFrameTag);
	if (pFrame != NULL)
		return E_FAIL;

	pFrame = CFrame::Create(fCallCnt);
	if (pFrame == NULL)
		return E_FAIL;

	m_mapFrame.insert(MAPFRAME::value_type(pFrameTag, pFrame));

	return S_OK;
}

bool Engine::CFrameMgr::PermitCall(const _tchar* pFrameTag, const _float& fDeltaTime)
{
	CFrame* pFrame = FindFrame(pFrameTag);

	if (pFrame == NULL)
		return false;

	return pFrame->PermitCall(fDeltaTime);
}

CFrame* Engine::CFrameMgr::FindFrame(const _tchar* pFrameTag)
{
	MAPFRAME::iterator iter = find_if(m_mapFrame.begin(), m_mapFrame.end(), CTagFinder(pFrameTag));

	if (iter == m_mapFrame.end())
		return NULL;

	return iter->second;
}

void Engine::CFrameMgr::Free( void )
{
	for_each(m_mapFrame.begin(), m_mapFrame.end(), CReleasePair());
	m_mapFrame.clear();
}

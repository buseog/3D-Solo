#include "Frame.h"

USING(Engine)

Engine::CFrame::CFrame(void)
: m_fCPS(0.f)
, m_fTimeAcc(0.f)
{

}

Engine::CFrame::~CFrame(void)
{

}

HRESULT Engine::CFrame::InitFrame(const _float& fCallCnt)
{
	m_fCPS = 1.f / fCallCnt;

	return S_OK;
}

bool Engine::CFrame::PermitCall(const _float& fDeltaTime)
{
	m_fTimeAcc += fDeltaTime;

	if (m_fTimeAcc > m_fCPS)
	{
		m_fTimeAcc = 0.f;
		return true;
	}

	return false;
}


void Engine::CFrame::Free(void)
{

}

CFrame* Engine::CFrame::Create(const _float& fCallCnt)
{
	CFrame* pFrame = new CFrame;

	if (FAILED(pFrame->InitFrame(fCallCnt)))
	{
		MSG_BOX("CFrame Created Failed");
		Engine::Safe_Release(pFrame);
	}

	return pFrame;
}

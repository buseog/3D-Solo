#include "Timer.h"

USING(Engine)

Engine::CTimer::CTimer(void)
:m_fDeltaTime(0.f)
{
	ZeroMemory(&m_FrameTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_FixTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_LastTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_CpuTick, sizeof(LARGE_INTEGER));
}

Engine::CTimer::~CTimer(void)
{
}

HRESULT Engine::CTimer::InitTimer(void)
{
	QueryPerformanceCounter(&m_FrameTime);
	QueryPerformanceCounter(&m_FixTime);
	QueryPerformanceCounter(&m_LastTime);
	QueryPerformanceFrequency(&m_CpuTick);

	return S_OK;
}

void Engine::CTimer::SetUpTimer(void)
{
	QueryPerformanceCounter(&m_FrameTime);

	if (m_FrameTime.QuadPart - m_LastTime.QuadPart >= m_CpuTick.QuadPart)
	{
		QueryPerformanceFrequency(&m_CpuTick);
		m_LastTime = m_FrameTime;
	}

	m_fDeltaTime = _float(m_FrameTime.QuadPart - m_FixTime.QuadPart) / m_CpuTick.QuadPart;

	m_FixTime = m_FrameTime;
}

_float Engine::CTimer::GetTimeDelta(void)
{
	return m_fDeltaTime;
}


void Engine::CTimer::Free(void)
{

}

Engine::CTimer* Engine::CTimer::Create(void)
{
	CTimer* pTimer = new CTimer;

	if (FAILED(pTimer->InitTimer()))
	{
		MSG_BOX("CTimer Create Failed");
		Engine::Safe_Release(pTimer);
	}

	return pTimer;
}


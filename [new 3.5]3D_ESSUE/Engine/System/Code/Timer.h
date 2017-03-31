#ifndef Engine_Timer_h__
#define Engine_Timer_h__

#include "Engine_BigHeaders.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CTimer :public CBase
{
private:
	LARGE_INTEGER	m_FrameTime;
	LARGE_INTEGER	m_FixTime;
	LARGE_INTEGER	m_LastTime;
	LARGE_INTEGER	m_CpuTick;

	_float			m_fDeltaTime;

public:
	HRESULT	InitTimer(void);
	void	SetUpTimer(void);
	_float	GetTimeDelta(void);

private:
	virtual void Free(void);

public:
	static CTimer* Create(void);

private:
	explicit CTimer(void);
	virtual ~CTimer(void);
};

END
#endif // Timer_h__
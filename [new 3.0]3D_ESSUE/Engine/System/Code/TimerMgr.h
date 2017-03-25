#ifndef TimerMgr_h__
#define TimerMgr_h__

#include "Engine_BigHeaders.h"
#include "Base.h"

BEGIN(Engine)

class CTimer;
class ENGINE_DLL CTimerMgr :public CBase
{
	DECLARE_SINGLETON(CTimerMgr)

private:
	typedef map<const _tchar*, CTimer*>		MAPTIMER;
	MAPTIMER								m_mapTimer;

private:
	CTimer* FindTimer(const _tchar* pTimerTag);

public:
	HRESULT	InitTimerMgr(const _tchar* pTimerTag);
	void	SetUpTimer(const _tchar* pTimerTag);

public:
	_float	GetTimeDelta(const _tchar* pTimerTag);

public:
	virtual void Free(void);

private:
	explicit CTimerMgr(void);
	virtual ~CTimerMgr(void);
};

END
#endif // TimerMgr_h__
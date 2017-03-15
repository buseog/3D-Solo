#ifndef KeyMgr_h__
#define KeyMgr_h__

#include "Engine_BigHeaders.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CKeyMgr :public CBase
{
	DECLARE_SINGLETON(CKeyMgr)

private:
	bool	m_bKeyDown[KEYPORT][KEYMAX];
	bool	m_bKeyUp[KEYPORT][KEYMAX];

public:
	bool	StayKeyDown(int nKey);
	bool	KeyDown(int nKey, int iIndex = 0);
	bool	KeyUp(int nKey, int iIndex = 0);

public:
	virtual void Free(void);

private:
	explicit CKeyMgr(void);
	virtual ~CKeyMgr(void);
};

END
#endif // KeyMgr_h__
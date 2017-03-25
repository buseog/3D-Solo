#ifndef FrameMgr_h__
#define FrameMgr_h__

#include "Engine_BigHeaders.h"
#include "Base.h"

BEGIN(Engine)

class CFrame;
class ENGINE_DLL CFrameMgr :public CBase
{
	DECLARE_SINGLETON(CFrameMgr)

private:
	typedef map<const _tchar*, CFrame*>		MAPFRAME;
	MAPFRAME								m_mapFrame;

private:
	CFrame*	FindFrame(const _tchar* pFrameTag);

public:
	HRESULT InitFrameMgr(const _tchar* pFrameTag, const _float& fCallCnt);
	bool	PermitCall(const _tchar* pFrameTag, const _float& fDeltaTime);

public:
	virtual void Free(void);

private:
	explicit CFrameMgr(void);
	virtual ~CFrameMgr(void);
};

END
#endif // FrameMgr_h__
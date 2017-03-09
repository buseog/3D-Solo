#ifndef Frame_h__
#define Frame_h__

#include "Engine_BigHeaders.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CFrame :public CBase
{
private:
	_float		m_fCPS;
	_float		m_fTimeAcc;

public:
	HRESULT InitFrame(const _float& fCallCnt);
	bool	PermitCall(const _float& fDeltaTime);

public:
	virtual void Free(void);

public:
	static CFrame* Create(const _float& fCallCnt);

private:
	explicit CFrame(void);
	virtual ~CFrame(void);
};

END
#endif // Frame_h__
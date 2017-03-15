#ifndef Observer_h__
#define Observer_h__

#include "Engine_BigHeaders.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CObserver :public CBase
{
public:
	virtual _int Update(_int iMessage, void* pData) PURE;
	
public:
	virtual void Free(void);

protected:
	explicit CObserver(void);
	virtual ~CObserver(void);
};

END
#endif // Observer_h__
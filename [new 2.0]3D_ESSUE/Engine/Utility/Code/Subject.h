#ifndef Subject_h__
#define Subject_h__

#include "Engine_BigHeaders.h"
#include "Base.h"

BEGIN(Engine)

class CObserver;
class ENGINE_DLL CSubject :public CBase
{
protected:
	typedef list<CObserver*>	OBSERVERLIST;
	OBSERVERLIST				m_ObserverList;

public:
	virtual void Subscribe(CObserver* pObserver);
	virtual void UnSubscribe(CObserver* pObserver);
	virtual void Notify(_uint iMessage, void* pData);

public:
	virtual void Free(void);

protected:
	explicit CSubject(void);
	virtual ~CSubject(void);
};

END
#endif // Subject_h__
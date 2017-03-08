#ifndef Component_h__
#define Component_h__

#include "Engine_BigHeaders.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CComponent :public CBase
{
public:
	virtual HRESULT Initialize(void);
	virtual void	Update(void);

public:
	virtual void Free(void);
	virtual CComponent* Clone(void) PURE;
	
protected:
	explicit CComponent(void);
	virtual ~CComponent(void);
};

END
#endif // Component_h__
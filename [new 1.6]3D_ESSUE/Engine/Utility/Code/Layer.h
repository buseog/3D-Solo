#ifndef Layer_h__
#define Layer_h__

#include "Engine_BigHeaders.h"
#include "Base.h"

BEGIN(Engine)

class CGameObject;
class ENGINE_DLL CLayer :public CBase
{
private:
	typedef list<CGameObject*>	OBJECTLIST;
	OBJECTLIST					m_ObjectList;

public:
	HRESULT	Initialize(void);
	_int	Update(const _float& fDelta);
	void	AddGameObject(CGameObject* pGameObject);

public:
	virtual void Free(void);

public:
	static CLayer* Create(void);

private:
	explicit CLayer(void);
	virtual ~CLayer(void);
};

END
#endif // Layer_h__
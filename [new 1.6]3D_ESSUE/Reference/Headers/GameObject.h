#ifndef GameObject_h__
#define GameObject_h__

#include "Engine_BigHeaders.h"
#include "Base.h"

BEGIN(Engine)

class CComponent;
class ENGINE_DLL CGameObject :public CBase
{
public:
	enum UPDATETYPE { UPDATE_NONE, UPDATE_DO, UPDATE_END };

protected:
	LPDIRECT3DDEVICE9	m_pDevice;

protected:
	typedef map<const _tchar*, CComponent*>		MAPCOMPONENT;
	MAPCOMPONENT		m_mapComponent[UPDATE_END];

protected:
	CComponent* FindComponent(UPDATETYPE eType, const _tchar* ComponentKey);
	HRESULT	AddComponent(UPDATETYPE eType, const _tchar* ComponentKey, CComponent* pComponent);

public:
	virtual HRESULT Initialize(void);
	virtual	_int	Update(const _float& fDelta);
	virtual void	Render(void);

public:
	virtual void	Free(void);

protected:
	explicit CGameObject(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CGameObject(void);
};

END
#endif // GameObject_h__
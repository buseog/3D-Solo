#ifndef ComponentMgr_h__
#define ComponentMgr_h__

#include "Engine_BigHeaders.h"
#include "Base.h"

BEGIN(Engine)

class CComponent;
class ENGINE_DLL CComponentMgr :public CBase
{
	DECLARE_SINGLETON(CComponentMgr)

private:
	typedef map<const _tchar*, CComponent*> MAPCOMPONENT;
	MAPCOMPONENT*	m_pMapComponent;

	_uint			m_iContainerSize;

private:
	CComponent* FindComponent(const _uint& iIndex, const _tchar* Componentkey);

public:
	HRESULT	ReserveComponentMgr(const _uint& iSize);
	HRESULT AddComponent(const _uint& iIndex, const _tchar* ComponentKey, CComponent* pComponent);
	CComponent* CloneComponent(const _uint& iIndex, const _tchar* ComponentKey);
	void	Clear(const _uint& iIndex);

public:
	virtual void Free(void);

private:
	explicit CComponentMgr(void);
	virtual ~CComponentMgr(void);
};

END
#endif // ComponentMgr_h__
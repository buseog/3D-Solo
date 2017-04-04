#ifndef EffectMgr_h__
#define EffectMgr_h__

#include "Export_Engine.h"
#include "BigHeaders.h"
#include "Base.h"

class CEffectMgr :public CBase
{
	DECLARE_SINGLETON(CEffectMgr)

private:
	LPDIRECT3DDEVICE9 m_pDevice;

public:
	HRESULT Initialize(LPDIRECT3DDEVICE9 pDevice);
	_int	Update(const _float& fDelta);
	void	Render(void);

public:
	virtual void Free(void);

private:
	explicit CEffectMgr(void);
	virtual ~CEffectMgr(void);
};

#endif // EffectMgr_h__
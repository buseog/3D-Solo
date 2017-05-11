#ifndef LightMgr_h__
#define LightMgr_h__

#include "Engine_BigHeaders.h"
#include "Base.h"

BEGIN(Engine)

class CLight;
class ENGINE_DLL CLightMgr :public CBase
{
	DECLARE_SINGLETON(CLightMgr)

private:
	typedef list<CLight*>	LIGHTLIST;
	LIGHTLIST				m_LightList;

public:
	HRESULT Init_Light(LPDIRECT3DDEVICE9 pDevice, const D3DLIGHT9* pLightInfo);
	void	Render(LPD3DXEFFECT pEffect);

private:
	virtual void Free(void);

private:
	explicit CLightMgr(void);
	virtual ~CLightMgr(void);
};

END
#endif // LightMgr_h__
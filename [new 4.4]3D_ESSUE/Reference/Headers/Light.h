#ifndef Light_h__
#define Light_h__

#include "Engine_BigHeaders.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CLight :public CBase
{
private:
	LPDIRECT3DDEVICE9			m_pDevice;
	LPDIRECT3DVERTEXBUFFER9		m_pVB;
	LPDIRECT3DINDEXBUFFER9		m_pIB;

private:
	D3DLIGHT9			m_tLightInfo;

public:
	HRESULT Initialize(const D3DLIGHT9* pLightInfo);
	void	Render(LPD3DXEFFECT pEffect);

public:
	virtual void Free(void);

public:
	static CLight* Create(LPDIRECT3DDEVICE9 pDevice, const D3DLIGHT9* pLightInfo);

private:
	explicit CLight(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CLight(void);
};

END
#endif // Light_h__
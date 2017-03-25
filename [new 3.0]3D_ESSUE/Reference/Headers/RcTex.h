#ifndef RcTex_h__
#define RcTex_h__

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CRcTex :public CVIBuffer
{
public:
	virtual HRESULT Initialize(void);
	virtual void Render(void);

private:
	virtual void Free(void);
	virtual CComponent* Clone(void);

public:
	static CRcTex* Create(LPDIRECT3DDEVICE9 pDevice);

private:
	explicit CRcTex(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CRcTex(const CRcTex& rhs);
	virtual ~CRcTex(void);
};

END
#endif // RcTex_h__
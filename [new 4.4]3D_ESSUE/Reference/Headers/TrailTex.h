#ifndef TrailTex_h__
#define TrailTex_h__

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CTrailTex :public CVIBuffer
{
public:
	virtual HRESULT Initialize(void);
	virtual void Render(void);

private:
	virtual void Free(void);
	virtual CComponent* Clone(void);

public:
	static CTrailTex* Create(LPDIRECT3DDEVICE9 pDevice);

private:
	explicit CTrailTex(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTrailTex(const CTrailTex& rhs);
	virtual ~CTrailTex(void);
};

END
#endif // TrailTex_h__
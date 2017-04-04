#ifndef TrainTex_h__
#define TrainTex_h__

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CTrainTex :public CVIBuffer
{
public:
	virtual HRESULT Initialize(void);
	virtual void Render(void);

private:
	virtual void Free(void);
	virtual CComponent* Clone(void);

public:
	static CTrainTex* Create(LPDIRECT3DDEVICE9 pDevice);

private:
	explicit CTrainTex(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTrainTex(const CTrainTex& rhs);
	virtual ~CTrainTex(void);
};

END
#endif // TrainTex_h__
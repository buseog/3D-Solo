#ifndef ParticleBuffer_h__
#define ParticleBuffer_h__

#include "ViBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CParticleBuffer :public CVIBuffer
{
private:
	IDirect3DVertexBuffer9* m_pVBInstance;

public:
	virtual HRESULT Initialize(void);
	virtual void Render(void);
	virtual void Render(LPD3DXEFFECT pEffect);

private:
	virtual void Free(void);
	virtual CComponent* Clone(void);

public:
	static CParticleBuffer* Create(LPDIRECT3DDEVICE9 pDevice);

private:
	explicit CParticleBuffer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CParticleBuffer(const CParticleBuffer& rhs);
	virtual ~CParticleBuffer(void);
};

END
#endif // ParticleBuffer_h__
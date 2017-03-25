#ifndef VIBuffer_h__
#define VIBuffer_h__

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer :public CComponent
{
protected:
	LPDIRECT3DDEVICE9			m_pDevice;

protected:
	LPDIRECT3DVERTEXBUFFER9				m_pVB;
	_ulong								m_dwVtxCnt;
	_ulong								m_dwVtxFVF;
	_ulong								m_dwVtxSize;

protected:
	LPDIRECT3DINDEXBUFFER9				m_pIB;
	_ulong								m_dwIdxSize;
	_ulong								m_dwTriCnt;
	D3DFORMAT							m_IndexFmt;

public:
	_ulong GetTriCount(void);

public:
	virtual HRESULT Initialize(void);
	virtual void	Render(void);

public:
	virtual CComponent* Clone(void) PURE;
	virtual void Free(void);

protected:
	explicit CVIBuffer(LPDIRECT3DDEVICE9 pDevice);
	explicit CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer(void);
};

END
#endif // VIBuffer_h__
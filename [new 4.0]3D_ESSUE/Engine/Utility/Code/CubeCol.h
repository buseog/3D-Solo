#ifndef CubeCol_h__
#define CubeCol_h__

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CCubeCol :public CVIBuffer
{
public:
	void SetColor(D3DXCOLOR vColor);

public:
	virtual HRESULT Initialize(void);
	virtual void	Render(void);

public:
	virtual void Free(void);
	virtual CComponent* Clone(void);

public:
	static CCubeCol* Create(LPDIRECT3DDEVICE9 pDevice);

private:
	explicit CCubeCol(LPDIRECT3DDEVICE9 pDevice);
	explicit CCubeCol(const CCubeCol& rhs);
	virtual ~CCubeCol(void);
};

END
#endif // CubeCol_h__
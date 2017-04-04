#ifndef TerrainTex_h__
#define TerrainTex_h__

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CTerrainTex :public CVIBuffer
{
private:
	HANDLE				m_hFile;
	BITMAPFILEHEADER	m_fh;
	BITMAPINFOHEADER	m_ih;
	MyVec3*				m_pPos;
	_bool				m_isClone;

public:
	const MyVec3* GetVtxPos(void);

public:
	virtual HRESULT Initialize(const _ulong& dwCntX, const _ulong& dwCntZ, const _ulong& dwItv);
	virtual void	Render(void);

public:
	virtual void Free(void);
	virtual CComponent* Clone(void);

public:
	static CTerrainTex* Create(LPDIRECT3DDEVICE9 pDevice, const _ulong& dwCntX, const _ulong& dwCntZ, const _ulong& dwItv);

private:
	explicit CTerrainTex(LPDIRECT3DDEVICE9 pDevice);
	explicit CTerrainTex(const CTerrainTex& rhs);
	virtual ~CTerrainTex(void);
};

END
#endif // TerrainTex_h__
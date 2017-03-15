#ifndef StaticMesh_h__
#define StaticMesh_h__

#include "Mesh.h"

BEGIN(Engine)

class ENGINE_DLL CStaticMesh :public CMesh
{
private:
	LPD3DXMESH					m_pOriMesh;	
	LPD3DXMESH					m_pMesh;
	LPD3DXBUFFER				m_pAdjacency;
	LPD3DXBUFFER				m_pSubSetBuffer;
	D3DXMATERIAL*				m_pSubSets;
	_ulong						m_dwSubSetCnt;
	LPDIRECT3DTEXTURE9*			m_ppTextures;

public:
	void ComputeMesh(MyVec3* pMin, MyVec3* pMax);

public:
	virtual HRESULT Initialize(const _tchar* FilePath, const _tchar* FileName);
	virtual void	Render(void);

public:
	virtual void Free(void);
	virtual CComponent* Clone(void);

public:
	static CStaticMesh* Create(LPDIRECT3DDEVICE9 pDevice, const _tchar* FilePath, const _tchar* FileName);

private:
	explicit CStaticMesh(LPDIRECT3DDEVICE9 pDevice);
	explicit CStaticMesh(const CStaticMesh& rhs);
	virtual ~CStaticMesh(void);
};

END
#endif // StaticMesh_h__
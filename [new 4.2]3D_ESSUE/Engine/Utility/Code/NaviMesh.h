#ifndef NaviMesh_h__
#define NaviMesh_h__

#include "Engine_BigHeaders.h"
#include "Base.h"

BEGIN(Engine)

class CLine2D;
class CNaviMesh :public CBase
{
public:
	enum POINTID { POINT_A, POINT_B, POINT_C, POINT_END };
	enum NEIGHBORID { NEIGHBOR_AB, NEIGHBOR_BC, NEIGHBOR_CA, NEIGHBOR_END };
	enum LINEID { LINE_AB, LINE_BC, LINE_CA, LINE_END };

private:
	LPDIRECT3DDEVICE9		m_pDevice;
	CNaviMesh*				m_pNeighbor[NEIGHBOR_END];
	MyVec3					m_vPoint[POINT_END];
	_ulong					m_dwIndex;

	LPD3DXLINE				m_pLine;
	CLine2D*				m_pLine2D[LINE_END];

	_bool					m_bPicking;

public:
	void SetPicking(_bool bTF) { m_bPicking = bTF; }

public:
	HRESULT	Initialize(const MyVec3* pPointA, const MyVec3* pPointB, const MyVec3* pPointC, const _ulong& dwIndex);
	void	Render(void);

	bool	Compare(const MyVec3* pFirst, const MyVec3* pSecond, CNaviMesh* pNeighbor);
	bool	CheckPass(const MyVec3* pStart, const MyVec3* pDir, NEIGHBORID* pNeighbor);

public:
	const MyVec3* GetPoint(POINTID ePoint) { return &m_vPoint[ePoint]; }
	const CNaviMesh* GetNeighbor(NEIGHBORID eNeighbor) { return m_pNeighbor[eNeighbor]; }
	_ulong	GetIndex(void) const { return m_dwIndex; }
	void	SetNeighbor(NEIGHBORID eNeighbor, CNaviMesh* pNeighbor) { m_pNeighbor[eNeighbor] = pNeighbor; }

public:
	virtual void Free(void);

public:
	static CNaviMesh* Create(LPDIRECT3DDEVICE9 pDevice, const MyVec3* pPointA, const MyVec3* pPointB, const MyVec3* pPointC, const _ulong& dwIndex);

private:
	explicit CNaviMesh(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CNaviMesh(void);
};

END
#endif // NaviMesh_h__
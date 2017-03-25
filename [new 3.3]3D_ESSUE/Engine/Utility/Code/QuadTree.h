#ifndef QuadTree_h__
#define QuadTree_h__

#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class CFrustum;
class CTerrain_Buffer;
class ENGINE_DLL CQuadTree : public CBase
{
public:
	enum CORNER {CORNER_LT, CORNER_RT, CORNER_LB, CORNER_RB, CORNER_END};
	enum CHILD {CHILD_LT, CHILD_RT, CHILD_LB, CHILD_RB, CHILD_END};
	enum NEIGHBOR {NEIGHBOR_LEFT, NEIGHBOR_TOP, NEIGHBOR_RIGHT, NEIGHBOR_BOTTOM, NEIGHBOR_END};
private:
	explicit CQuadTree(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CQuadTree(void);
public:
	HRESULT Ready_QuadTree(const _ulong& wCntX, const _ulong& wCntZ, const _float& fItv);
	void Culling_ByQuadTree(const CFrustum* pFrustum, const CTerrain_Buffer* pTerrainBufferCom, INDEX32* pIndices, _ulong* pTriCnt);
	void Culling_ByQuadTree(const CFrustum* pFrustum, const _vec3* pVtxPos, INDEX32* pIndices, _ulong* pTriCnt);
	_bool isDraw_ByLOD(const _vec3* pVtxPos);
private:
	LPDIRECT3DDEVICE9	m_pGraphicDev;
	_ulong				m_dwCorner[CORNER_END];
	_ulong				m_dwCenter;
	_float				m_fRadius;
	CQuadTree*			m_pChild[CHILD_END];
	CQuadTree*			m_pNeighbor[NEIGHBOR_END];
private:
	void Make_Child(const _ulong& wCntX, const _ulong& wCntZ, const _float& fItv);
	void Make_Neighbor(void);
	void SetUp_ChildCorner(CHILD eID, _ulong* pChildCorner);
	void SetUp_NeighborCorner(NEIGHBOR eID, _ulong* pLT, _ulong* pRT, _ulong* pLB, _ulong* pRB, _ulong* pCenter);	
public:
	static CQuadTree* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CQuadTree* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _ulong* pCorner
		, const _ulong& dwCntX, const _ulong& dwCntZ, const _float& fItv);
private:
	virtual void Free(void);
};


END

#endif // QuadTree_h__

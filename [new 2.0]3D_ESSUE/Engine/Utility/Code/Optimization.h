#ifndef Optimization_h__
#define Optimization_h__

#include "Engine_Defines.h"
#include "Component.h"

BEGIN(Engine)

class CTerrain_Buffer;
class CFrustum;
class CQuadTree;
class ENGINE_DLL COptimization : public CComponent
{
private:
	explicit COptimization(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit COptimization(const COptimization& rhs);
	virtual	~COptimization(void);	
public:
	HRESULT Ready_Optimization(void);
	void Update_Component(void);
	HRESULT Ready_QuadTree(const _ulong& dwCntX, const _ulong& dwCntZ, const _float& fItv);
	_bool Culling_ByFrustum(const _vec3* pWorldPosition);
	_bool Culling_ByFrustum(const _vec3* pWorldPosition, const _float& fRadius);
	void Culling_ByFrustum(const CTerrain_Buffer* pTerrainBufferCom
		, INDEX32* pIndices, _ulong* pTriCnt);

	void Culling_ByQuadTree(const _vec3* pVtxPos
		, INDEX32* pIndices, _ulong* pTriCnt);

	void Culling_ByQuadTree(const CTerrain_Buffer* pTerrainBufferCom
		, INDEX32* pIndices, _ulong* pTriCnt);
private:
	LPDIRECT3DDEVICE9			m_pGraphicDev;	
	CFrustum*					m_pFrustum;	
	CQuadTree*					m_pQuadTree;
public:
	static COptimization* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone(void);
private:
	virtual void Free(void);

};

END

#endif // Optimization_h__

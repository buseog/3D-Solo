#ifndef Frustum_h__
#define Frustum_h__

#include "Engine_Defines.h"
#include "Base.h"
#include "Terrain_Buffer.h"

BEGIN(Engine)

class ENGINE_DLL CFrustum : public CBase
{
private:
	explicit CFrustum(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CFrustum(void);
public:
	HRESULT Ready_Frustum(void);
	_bool Culling_ByFrustum(const _vec3* pWorldPosition);
	_bool Culling_ByFrustum(const _vec3* pWorldPosition, const _float& fRadius);
	 void Culling_ByFrustum(const CTerrain_Buffer* pTerrainBufferCom
		, INDEX32* pIndices, _ulong* pTriCnt);
	 void SetUp_Frustum();
	_bool isIn_Frustum(const _vec3* pPosition) const;
	_bool isIn_Frustum(const _vec3* pPosition, const _float& fRadius) const;
private:
	LPDIRECT3DDEVICE9			m_pGraphicDev;
	_vec3						m_vPoint[8];
	D3DXPLANE					m_Plane[6];
	_bool						m_isSetUpPlane;
private:
	
public:
	static CFrustum* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);

};


END

#endif // Frustum_h__

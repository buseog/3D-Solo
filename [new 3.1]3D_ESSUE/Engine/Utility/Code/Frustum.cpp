#include "Frustum.h"
#include "Terrain_Buffer.h"

USING(Engine)

Engine::CFrustum::CFrustum(LPDIRECT3DDEVICE9 pGraphicDev)
: m_pGraphicDev(pGraphicDev)
, m_isSetUpPlane(false)
{
	m_pGraphicDev->AddRef();
}

Engine::CFrustum::~CFrustum(void)
{

}

HRESULT Engine::CFrustum::Ready_Frustum(void)
{
	m_vPoint[0] = _vec3(-1.f, 1.f, 0.f);
	m_vPoint[1] = _vec3(1.f, 1.f, 0.f);
	m_vPoint[2] = _vec3(1.f, -1.f, 0.f);
	m_vPoint[3] = _vec3(-1.f, -1.f, 0.f);

	m_vPoint[4] = _vec3(-1.f, 1.f, 1.f);
	m_vPoint[5] = _vec3(1.f, 1.f, 1.f);
	m_vPoint[6] = _vec3(1.f, -1.f, 1.f);
	m_vPoint[7] = _vec3(-1.f, -1.f, 1.f);

	return S_OK;
}

_bool Engine::CFrustum::Culling_ByFrustum(const _vec3* pWorldPosition)
{
	return isIn_Frustum(pWorldPosition);	
}

_bool Engine::CFrustum::Culling_ByFrustum(const _vec3* pWorldPosition, const _float& fRadius)
{
	return isIn_Frustum(pWorldPosition, fRadius);	
}

void Engine::CFrustum::Culling_ByFrustum(const CTerrain_Buffer* pTerrainBufferCom
					   , INDEX32* pIndices, _ulong* pTriCnt)
{
	const _vec3* pVtxPosition = pTerrainBufferCom->Get_VtxPosition();

	_ulong dwNumVtxX = pTerrainBufferCom->Get_VtxCntX();
	_ulong dwNumVtxZ = pTerrainBufferCom->Get_VtxCntZ();

	_bool			isIn[3] = {false};

	*pTriCnt = 0;

	for (_ulong i = 0; i < dwNumVtxZ - 1; ++i)
	{
		for (_ulong j = 0; j < dwNumVtxX - 1; ++j)
		{
			_ulong			dwIndex = i * dwNumVtxZ + j;

			isIn[0] = isIn_Frustum(&pVtxPosition[dwIndex + dwNumVtxX]);
			isIn[1] = isIn_Frustum(&pVtxPosition[dwIndex + dwNumVtxX + 1]);
			isIn[2] = isIn_Frustum(&pVtxPosition[dwIndex + 1]);

			if(true == isIn[0]
			|| true == isIn[1]
			|| true == isIn[2])
			{
				pIndices[*pTriCnt]._1 = dwIndex + dwNumVtxX;
				pIndices[*pTriCnt]._2 = dwIndex + dwNumVtxX + 1;
				pIndices[*pTriCnt]._3 = dwIndex + 1;
				++(*pTriCnt);
			}

			isIn[0] = isIn_Frustum(&pVtxPosition[dwIndex + dwNumVtxX]);
			isIn[1] = isIn_Frustum(&pVtxPosition[dwIndex + 1]);
			isIn[2] = isIn_Frustum(&pVtxPosition[dwIndex]);

			if(true == isIn[0]
			|| true == isIn[1]
			|| true == isIn[2])
			{
				pIndices[*pTriCnt]._1 = dwIndex + dwNumVtxX;
				pIndices[*pTriCnt]._2 = dwIndex + 1;
				pIndices[*pTriCnt]._3 = dwIndex;
				++(*pTriCnt);
			}
		}	
	}
}

void Engine::CFrustum::SetUp_Frustum()
{
	
	Ready_Frustum();

	// To.ViewSpace
	_matrix			matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, NULL, &matProj);

	for (_uint i = 0; i < 8; ++i)
	{
		D3DXVec3TransformCoord(&m_vPoint[i], &m_vPoint[i], &matProj);
	}

	// To.WorldSpace
	_matrix			matView;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, NULL, &matView);

	for (_uint i = 0; i < 8; ++i)
	{
		D3DXVec3TransformCoord(&m_vPoint[i], &m_vPoint[i], &matView);
	}

	// 평면상의 점(x, y, z)이 있었다면 ax + by + cz + d = 0을 만족한다.
	// 점이 평면 위에 있었더라면 ax + by + cz + d > 0을 만족하고
	// 점이 평면 아래에 있었더라면 ax + by + cz + d < 0을 만족하고
	// x	
	D3DXPlaneFromPoints(&m_Plane[0], &m_vPoint[1], &m_vPoint[5], &m_vPoint[6]);
	D3DXPlaneFromPoints(&m_Plane[1], &m_vPoint[4], &m_vPoint[0], &m_vPoint[3]);

	// Y
	D3DXPlaneFromPoints(&m_Plane[2], &m_vPoint[4], &m_vPoint[5], &m_vPoint[1]);
	D3DXPlaneFromPoints(&m_Plane[3], &m_vPoint[3], &m_vPoint[2], &m_vPoint[6]);

	// z	
	D3DXPlaneFromPoints(&m_Plane[4], &m_vPoint[7], &m_vPoint[6], &m_vPoint[5]);
	D3DXPlaneFromPoints(&m_Plane[5], &m_vPoint[0], &m_vPoint[1], &m_vPoint[2]);

	m_isSetUpPlane = true;
}

_bool Engine::CFrustum::isIn_Frustum(const _vec3* pPosition) const 
{
	for (_uint i = 0; i < 6; ++i)
	{
		if(0.0f < D3DXPlaneDotCoord(&m_Plane[i], pPosition))
			return false;
	}		
	return true;
}

_bool Engine::CFrustum::isIn_Frustum(const _vec3* pPosition, const _float& fRadius) const 
{
	for (_uint i = 0; i < 6; ++i)
	{
		if(fRadius < D3DXPlaneDotCoord(&m_Plane[i], pPosition))
			return false;
	}		
	return true;
}

CFrustum* Engine::CFrustum::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CFrustum*	pInstance = new CFrustum(pGraphicDev);

	if(FAILED(pInstance->Ready_Frustum()))
	{
		MSG_BOX(L"CFrustum Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

void Engine::CFrustum::Free(void)
{
	Engine::Safe_Release(m_pGraphicDev);	

}


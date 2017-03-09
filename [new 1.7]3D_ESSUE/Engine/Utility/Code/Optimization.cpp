#include "Optimization.h"
#include "Frustum.h"
#include "QuadTree.h"

USING(Engine)

Engine::COptimization::COptimization(LPDIRECT3DDEVICE9 pGraphicDev)
: m_pGraphicDev(pGraphicDev)
, m_pFrustum(NULL)
, m_pQuadTree(NULL)
{
	m_pGraphicDev->AddRef();
}

Engine::COptimization::COptimization(const COptimization& rhs)
: m_pGraphicDev(rhs.m_pGraphicDev)
, m_pFrustum(rhs.m_pFrustum)
, m_pQuadTree(rhs.m_pQuadTree)
{
	m_pGraphicDev->AddRef();
	m_pFrustum->AddRef();
	m_pQuadTree->AddRef();
}

Engine::COptimization::~COptimization(void)
{

}

HRESULT Engine::COptimization::Ready_Optimization(void)
{
	m_pFrustum = CFrustum::Create(m_pGraphicDev);
	if(NULL == m_pFrustum)
		return E_FAIL;

	m_pQuadTree = CQuadTree::Create(m_pGraphicDev);
	if(NULL == m_pQuadTree)
		return E_FAIL;
	
	return S_OK;
}

void Engine::COptimization::Update_Component(void)
{	
	m_pFrustum->SetUp_Frustum();
}

HRESULT Engine::COptimization::Ready_QuadTree(const _ulong& dwCntX, const _ulong& dwCntZ, const _float& fItv)
{
	if(FAILED(m_pQuadTree->Ready_QuadTree(dwCntX, dwCntZ, fItv)))
		return E_FAIL;

	return S_OK;
}

_bool Engine::COptimization::Culling_ByFrustum(const _vec3* pWorldPosition)
{
	return m_pFrustum->Culling_ByFrustum(pWorldPosition);
}


_bool Engine::COptimization::Culling_ByFrustum(const _vec3* pWorldPosition, const _float& fRadius)
{
	return m_pFrustum->Culling_ByFrustum(pWorldPosition, fRadius);
}

void Engine::COptimization::Culling_ByFrustum(const CTerrain_Buffer* pTerrainBufferCom
												 , INDEX32* pIndices, _ulong* pTriCnt)
{	
	m_pFrustum->Culling_ByFrustum(pTerrainBufferCom, pIndices, pTriCnt);	
}

void Engine::COptimization::Culling_ByQuadTree(const _vec3* pVtxPos
						, INDEX32* pIndices, _ulong* pTriCnt)
{
	*pTriCnt = 0;
	m_pQuadTree->Culling_ByQuadTree(m_pFrustum, pVtxPos, pIndices, pTriCnt);
}



void Engine::COptimization::Culling_ByQuadTree(const CTerrain_Buffer* pTerrainBufferCom
						, INDEX32* pIndices, _ulong* pTriCnt)
{
	*pTriCnt = 0;
	m_pQuadTree->Culling_ByQuadTree(m_pFrustum, pTerrainBufferCom, pIndices, pTriCnt);
}

COptimization* Engine::COptimization::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	COptimization*		pInstance = new COptimization(pGraphicDev);

	if(FAILED(pInstance->Ready_Optimization()))
	{
		MSG_BOX(L"COptimization Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* Engine::COptimization::Clone(void)
{
	return new COptimization(*this);
}

void Engine::COptimization::Free(void)
{
	Engine::Safe_Release(m_pFrustum);
	Engine::Safe_Release(m_pQuadTree);

	Engine::Safe_Release(m_pGraphicDev);		
}


#include "QuadTree.h"
#include "Frustum.h"
#include "Terrain_Buffer.h"

USING(Engine)

Engine::CQuadTree::CQuadTree(LPDIRECT3DDEVICE9 pGraphicDev)
: m_pGraphicDev(pGraphicDev)
, m_dwCenter(0)
{
	m_pGraphicDev->AddRef();
	ZeroMemory(m_pChild, sizeof(CQuadTree*) * CHILD_END);
	ZeroMemory(m_pNeighbor, sizeof(CQuadTree*) * NEIGHBOR_END);
}

Engine::CQuadTree::~CQuadTree(void)
{

}

HRESULT Engine::CQuadTree::Ready_QuadTree(const _ulong& dwCntX, const _ulong& dwCntZ, const _float& fItv)
{
	// 가장 큰 쿼드트리의 정보를 채운다.
	m_dwCorner[CORNER_LT] = dwCntX * dwCntZ - dwCntX;
	m_dwCorner[CORNER_RT] = dwCntX * dwCntZ - 1;
	m_dwCorner[CORNER_LB] = 0;
	m_dwCorner[CORNER_RB] = dwCntX - 1;

	for (_ulong i = 0; i < CORNER_END; ++i)
		m_dwCenter += m_dwCorner[i];
	m_dwCenter = (_ulong)(m_dwCenter * 0.25f);

	m_fRadius = sqrtf(pow((m_dwCorner[CORNER_RB] - m_dwCorner[CORNER_LB]) * fItv, 2)
		+ pow((m_dwCorner[CORNER_RT] - m_dwCorner[CORNER_RB]) / dwCntZ * fItv, 2)) * 0.5f;

	Make_Child(dwCntX, dwCntZ, fItv);

	Make_Neighbor();

	return S_OK;
}

void Engine::CQuadTree::Culling_ByQuadTree(const CFrustum* pFrustum, const CTerrain_Buffer* pTerrainBufferCom, INDEX32* pIndices, _ulong* pTriCnt)
{
	// 쿼드트리에 외접하는 원과 절두체가 서로 겹치는지 확인하자.
	const _vec3* pVtxPos = pTerrainBufferCom->Get_VtxPosition();

	if(true == isDraw_ByLOD(pVtxPos))
	{
		_bool		isDrawNeighbor[NEIGHBOR_END] = {true, true, true, true};

		for (_ulong i = 0; i < NEIGHBOR_END; ++i)
		{
			if(NULL != m_pNeighbor[i])
				isDrawNeighbor[i] = m_pNeighbor[i]->isDraw_ByLOD(pVtxPos);
		}

		_ulong		dwLC, dwTC, dwRC, dwBC;
		dwLC = (m_dwCorner[CORNER_LT] + m_dwCorner[CORNER_LB]) * 0.5f;
		dwTC = (m_dwCorner[CORNER_LT] + m_dwCorner[CORNER_RT]) * 0.5f;
		dwRC = (m_dwCorner[CORNER_RT] + m_dwCorner[CORNER_RB]) * 0.5f;
		dwBC = (m_dwCorner[CORNER_LB] + m_dwCorner[CORNER_RB]) * 0.5f;

		_bool		isIn[4] = {false};

		isIn[0] = pFrustum->isIn_Frustum(&pVtxPos[m_dwCorner[CORNER_LT]]);
		isIn[1] = pFrustum->isIn_Frustum(&pVtxPos[m_dwCorner[CORNER_RT]]);
		isIn[2] = pFrustum->isIn_Frustum(&pVtxPos[m_dwCorner[CORNER_RB]]);
		isIn[3] = pFrustum->isIn_Frustum(&pVtxPos[m_dwCorner[CORNER_LB]]);

		if(true == isDrawNeighbor[NEIGHBOR_LEFT]
			&& true == isDrawNeighbor[NEIGHBOR_TOP]
			&& true == isDrawNeighbor[NEIGHBOR_RIGHT]
			&& true == isDrawNeighbor[NEIGHBOR_BOTTOM])
		{
			if(isIn[0] || isIn[1] || isIn[2])
			{
				pIndices[*pTriCnt]._1 = m_dwCorner[CORNER_LT];
				pIndices[*pTriCnt]._2 = m_dwCorner[CORNER_RT];
				pIndices[*pTriCnt]._3 = m_dwCorner[CORNER_RB];
				++(*pTriCnt);
			}

			if(isIn[0] || isIn[2] || isIn[3])
			{
				pIndices[*pTriCnt]._1 = m_dwCorner[CORNER_LT];
				pIndices[*pTriCnt]._2 = m_dwCorner[CORNER_RB];
				pIndices[*pTriCnt]._3 = m_dwCorner[CORNER_LB];
				++(*pTriCnt);
			}	
			return;			
		}

		if(true == isDrawNeighbor[NEIGHBOR_LEFT]
			&& true == isDrawNeighbor[NEIGHBOR_BOTTOM])
		{
			if(isIn[0] || isIn[2] || isIn[3])
			{
				pIndices[*pTriCnt]._1 = m_dwCorner[CORNER_LT];
				pIndices[*pTriCnt]._2 = m_dwCorner[CORNER_RB];
				pIndices[*pTriCnt]._3 = m_dwCorner[CORNER_LB];
				++(*pTriCnt);
			}
		}
		else
		{
			if(false == isDrawNeighbor[NEIGHBOR_LEFT])
			{
				pIndices[*pTriCnt]._1 = m_dwCorner[CORNER_LT];
				pIndices[*pTriCnt]._2 = m_dwCenter;
				pIndices[*pTriCnt]._3 = dwLC;
				++(*pTriCnt);

				pIndices[*pTriCnt]._1 = dwLC;
				pIndices[*pTriCnt]._2 = m_dwCenter;
				pIndices[*pTriCnt]._3 = m_dwCorner[CORNER_LB];
				++(*pTriCnt);

			}
			else
			{
				pIndices[*pTriCnt]._1 = m_dwCorner[CORNER_LT];
				pIndices[*pTriCnt]._2 = m_dwCenter;
				pIndices[*pTriCnt]._3 = m_dwCorner[CORNER_LB];
				++(*pTriCnt);
			}

			if(false == isDrawNeighbor[NEIGHBOR_BOTTOM])
			{
				pIndices[*pTriCnt]._1 = m_dwCorner[CORNER_LB];
				pIndices[*pTriCnt]._2 = m_dwCenter;
				pIndices[*pTriCnt]._3 = dwBC;
				++(*pTriCnt);

				pIndices[*pTriCnt]._1 = dwBC;
				pIndices[*pTriCnt]._2 = m_dwCenter;
				pIndices[*pTriCnt]._3 = m_dwCorner[CORNER_RB];
				++(*pTriCnt);
			}
			else
			{
				pIndices[*pTriCnt]._1 = m_dwCorner[CORNER_LB];
				pIndices[*pTriCnt]._2 = m_dwCenter;
				pIndices[*pTriCnt]._3 = m_dwCorner[CORNER_RB];
				++(*pTriCnt);
			}
		}

		if(true == isDrawNeighbor[NEIGHBOR_TOP]
			&& true == isDrawNeighbor[NEIGHBOR_RIGHT])
		{
			if(isIn[0] || isIn[1] || isIn[2])
			{
				pIndices[*pTriCnt]._1 = m_dwCorner[CORNER_LT];
				pIndices[*pTriCnt]._2 = m_dwCorner[CORNER_RT];
				pIndices[*pTriCnt]._3 = m_dwCorner[CORNER_RB];
				++(*pTriCnt);
			}
		}
		else
		{
			if(false == isDrawNeighbor[NEIGHBOR_TOP])
			{
				pIndices[*pTriCnt]._1 = m_dwCorner[CORNER_LT];
				pIndices[*pTriCnt]._2 = dwTC;
				pIndices[*pTriCnt]._3 = m_dwCenter;
				++(*pTriCnt);

				pIndices[*pTriCnt]._1 = m_dwCenter;
				pIndices[*pTriCnt]._2 = dwTC;
				pIndices[*pTriCnt]._3 = m_dwCorner[CORNER_RT];
				++(*pTriCnt);
			}
			else
			{
				pIndices[*pTriCnt]._1 = m_dwCorner[CORNER_LT];
				pIndices[*pTriCnt]._2 = m_dwCorner[CORNER_RT];
				pIndices[*pTriCnt]._3 = m_dwCenter;
				++(*pTriCnt);
			}

			if(false == isDrawNeighbor[NEIGHBOR_RIGHT])
			{
				pIndices[*pTriCnt]._1 = m_dwCorner[CORNER_RT];
				pIndices[*pTriCnt]._2 = dwRC;
				pIndices[*pTriCnt]._3 = m_dwCenter;
				++(*pTriCnt);

				pIndices[*pTriCnt]._1 = m_dwCenter;
				pIndices[*pTriCnt]._2 = dwRC;
				pIndices[*pTriCnt]._3 = m_dwCorner[CORNER_RB];
				++(*pTriCnt);
			}
			else
			{
				pIndices[*pTriCnt]._1 = m_dwCenter;
				pIndices[*pTriCnt]._2 = m_dwCorner[CORNER_RT];
				pIndices[*pTriCnt]._3 = m_dwCorner[CORNER_RB];
				++(*pTriCnt);
			}
		}		
	}

	if(true == pFrustum->isIn_Frustum(&pVtxPos[m_dwCenter], m_fRadius))
	{
		for (_ulong i = 0; i < CHILD_END; ++i)
		{
			if(NULL != m_pChild[i])
				m_pChild[i]->Culling_ByQuadTree(pFrustum, pTerrainBufferCom, pIndices, pTriCnt);
		}
	}
}

void CQuadTree::Culling_ByQuadTree(const CFrustum* pFrustum, const _vec3* pVtxPos, INDEX32* pIndices, _ulong* pTriCnt)
{

	// 쿼드트리에 외접하는 원과 절두체가 서로 겹치는지 확인하자.
	//if(NULL == m_pChild[CHILD_LT])
	if(true == isDraw_ByLOD(pVtxPos))
	{
		_bool		isDrawNeighbor[NEIGHBOR_END] = {true, true, true, true};

		for (_ulong i = 0; i < NEIGHBOR_END; ++i)
		{
			if(NULL != m_pNeighbor[i])
				isDrawNeighbor[i] = m_pNeighbor[i]->isDraw_ByLOD(pVtxPos);
		}

		_ulong		dwLC, dwTC, dwRC, dwBC;
		dwLC = (m_dwCorner[CORNER_LT] + m_dwCorner[CORNER_LB]) * 0.5f;
		dwTC = (m_dwCorner[CORNER_LT] + m_dwCorner[CORNER_RT]) * 0.5f;
		dwRC = (m_dwCorner[CORNER_RT] + m_dwCorner[CORNER_RB]) * 0.5f;
		dwBC = (m_dwCorner[CORNER_LB] + m_dwCorner[CORNER_RB]) * 0.5f;

		_bool		isIn[4] = {false};

		isIn[0] = pFrustum->isIn_Frustum(&pVtxPos[m_dwCorner[CORNER_LT]]);
		isIn[1] = pFrustum->isIn_Frustum(&pVtxPos[m_dwCorner[CORNER_RT]]);
		isIn[2] = pFrustum->isIn_Frustum(&pVtxPos[m_dwCorner[CORNER_RB]]);
		isIn[3] = pFrustum->isIn_Frustum(&pVtxPos[m_dwCorner[CORNER_LB]]);

		if(true == isDrawNeighbor[NEIGHBOR_LEFT]
		&& true == isDrawNeighbor[NEIGHBOR_TOP]
		&& true == isDrawNeighbor[NEIGHBOR_RIGHT]
		&& true == isDrawNeighbor[NEIGHBOR_BOTTOM])
		{
			if(isIn[0] || isIn[1] || isIn[2])
			{
				pIndices[*pTriCnt]._1 = m_dwCorner[CORNER_LT];
				pIndices[*pTriCnt]._2 = m_dwCorner[CORNER_RT];
				pIndices[*pTriCnt]._3 = m_dwCorner[CORNER_RB];
				++(*pTriCnt);
			}

			if(isIn[0] || isIn[2] || isIn[3])
			{
				pIndices[*pTriCnt]._1 = m_dwCorner[CORNER_LT];
				pIndices[*pTriCnt]._2 = m_dwCorner[CORNER_RB];
				pIndices[*pTriCnt]._3 = m_dwCorner[CORNER_LB];
				++(*pTriCnt);
			}	
			return;			
		}

	
		if(false == isDrawNeighbor[NEIGHBOR_LEFT])
		{
			pIndices[*pTriCnt]._1 = m_dwCorner[CORNER_LT];
			pIndices[*pTriCnt]._2 = m_dwCenter;
			pIndices[*pTriCnt]._3 = dwLC;
			++(*pTriCnt);

			pIndices[*pTriCnt]._1 = dwLC;
			pIndices[*pTriCnt]._2 = m_dwCenter;
			pIndices[*pTriCnt]._3 = m_dwCorner[CORNER_LB];
			++(*pTriCnt);

		}
		else
		{
			pIndices[*pTriCnt]._1 = m_dwCorner[CORNER_LT];
			pIndices[*pTriCnt]._2 = m_dwCenter;
			pIndices[*pTriCnt]._3 = m_dwCorner[CORNER_LB];
			++(*pTriCnt);
		}

		if(false == isDrawNeighbor[NEIGHBOR_BOTTOM])
		{
			pIndices[*pTriCnt]._1 = m_dwCorner[CORNER_LB];
			pIndices[*pTriCnt]._2 = m_dwCenter;
			pIndices[*pTriCnt]._3 = dwBC;
			++(*pTriCnt);

			pIndices[*pTriCnt]._1 = dwBC;
			pIndices[*pTriCnt]._2 = m_dwCenter;
			pIndices[*pTriCnt]._3 = m_dwCorner[CORNER_RB];
			++(*pTriCnt);
		}
		else
		{
			pIndices[*pTriCnt]._1 = m_dwCorner[CORNER_LB];
			pIndices[*pTriCnt]._2 = m_dwCenter;
			pIndices[*pTriCnt]._3 = m_dwCorner[CORNER_RB];
			++(*pTriCnt);
		}
	

	
	
		if(false == isDrawNeighbor[NEIGHBOR_TOP])
		{
			pIndices[*pTriCnt]._1 = m_dwCorner[CORNER_LT];
			pIndices[*pTriCnt]._2 = dwTC;
			pIndices[*pTriCnt]._3 = m_dwCenter;
			++(*pTriCnt);

			pIndices[*pTriCnt]._1 = m_dwCenter;
			pIndices[*pTriCnt]._2 = dwTC;
			pIndices[*pTriCnt]._3 = m_dwCorner[CORNER_RT];
			++(*pTriCnt);
		}
		else
		{
			pIndices[*pTriCnt]._1 = m_dwCorner[CORNER_LT];
			pIndices[*pTriCnt]._2 = m_dwCorner[CORNER_RT];
			pIndices[*pTriCnt]._3 = m_dwCenter;
			++(*pTriCnt);
		}

		if(false == isDrawNeighbor[NEIGHBOR_RIGHT])
		{
			pIndices[*pTriCnt]._1 = m_dwCorner[CORNER_RT];
			pIndices[*pTriCnt]._2 = dwRC;
			pIndices[*pTriCnt]._3 = m_dwCenter;
			++(*pTriCnt);

			pIndices[*pTriCnt]._1 = m_dwCenter;
			pIndices[*pTriCnt]._2 = dwRC;
			pIndices[*pTriCnt]._3 = m_dwCorner[CORNER_RB];
			++(*pTriCnt);
		}
		else
		{
			pIndices[*pTriCnt]._1 = m_dwCenter;
			pIndices[*pTriCnt]._2 = m_dwCorner[CORNER_RT];
			pIndices[*pTriCnt]._3 = m_dwCorner[CORNER_RB];
			++(*pTriCnt);
		}
		
		return;
	}

	if(true == pFrustum->isIn_Frustum(&pVtxPos[m_dwCenter], m_fRadius))
	{
		for (_ulong i = 0; i < CHILD_END; ++i)
		{
			if(NULL != m_pChild[i])
				m_pChild[i]->Culling_ByQuadTree(pFrustum, pVtxPos, pIndices, pTriCnt);
		}
	}
}

_bool CQuadTree::isDraw_ByLOD(const _vec3* pVtxPos)
{
	_matrix			matView;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, NULL, &matView);

	_vec3			vCamPos;
	memcpy(&vCamPos, &matView.m[3][0], sizeof(_vec3));

	_float		fDistance = D3DXVec3Length(&(vCamPos - pVtxPos[m_dwCenter]));

	_float		fWidth = D3DXVec3Length(&(pVtxPos[m_dwCorner[CORNER_RT]] - pVtxPos[m_dwCorner[CORNER_LT]]));

	if(fDistance * 0.1f > fWidth)
		return true;

	return false;
}

void CQuadTree::Make_Child(const _ulong& dwCntX, const _ulong& dwCntZ, const _float& fItv)
{
	// 더이상 나눌 수 없다면.나가
	if(m_dwCorner[CORNER_RT] - m_dwCorner[CORNER_LT] == 1)
		return;

	for (_ulong i = 0; i < CHILD_END; ++i)
	{
		_ulong		dwChildCorner[CORNER_END];

		SetUp_ChildCorner(CHILD(i), dwChildCorner);

		m_pChild[i] = CQuadTree::Create(m_pGraphicDev, dwChildCorner, dwCntX, dwCntZ, fItv);
		if(NULL == m_pChild[i])
			return;

		m_pChild[i]->Make_Child(dwCntX, dwCntZ, fItv);
	}
}

void CQuadTree::Make_Neighbor(void)
{
	if(NULL == m_pChild[CHILD_LT])
		return;
	
	m_pChild[CHILD_LT]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pChild[CHILD_RT];
	m_pChild[CHILD_LT]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pChild[CHILD_LB];

	m_pChild[CHILD_RT]->m_pNeighbor[NEIGHBOR_LEFT] = m_pChild[CHILD_LT];
	m_pChild[CHILD_RT]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pChild[CHILD_RB];

	m_pChild[CHILD_LB]->m_pNeighbor[NEIGHBOR_TOP] = m_pChild[CHILD_LT];
	m_pChild[CHILD_LB]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pChild[CHILD_RB];

	m_pChild[CHILD_RB]->m_pNeighbor[NEIGHBOR_TOP] = m_pChild[CHILD_RT];
	m_pChild[CHILD_RB]->m_pNeighbor[NEIGHBOR_LEFT] = m_pChild[CHILD_LB];

	if(NULL != m_pNeighbor[NEIGHBOR_LEFT])
	{
		m_pChild[CHILD_LT]->m_pNeighbor[NEIGHBOR_LEFT] = m_pNeighbor[NEIGHBOR_LEFT]->m_pChild[CHILD_RT];
		m_pChild[CHILD_LB]->m_pNeighbor[NEIGHBOR_LEFT] = m_pNeighbor[NEIGHBOR_LEFT]->m_pChild[CHILD_RB];
	}

	if(NULL != m_pNeighbor[NEIGHBOR_TOP])
	{
		m_pChild[CHILD_LT]->m_pNeighbor[NEIGHBOR_TOP] = m_pNeighbor[NEIGHBOR_TOP]->m_pChild[CHILD_LB];
		m_pChild[CHILD_RT]->m_pNeighbor[NEIGHBOR_TOP] = m_pNeighbor[NEIGHBOR_TOP]->m_pChild[CHILD_RB];
	}

	if(NULL != m_pNeighbor[NEIGHBOR_RIGHT])
	{
		m_pChild[CHILD_RT]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pNeighbor[NEIGHBOR_RIGHT]->m_pChild[CHILD_LT];
		m_pChild[CHILD_RB]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pNeighbor[NEIGHBOR_RIGHT]->m_pChild[CHILD_LB];
	}

	if(NULL != m_pNeighbor[NEIGHBOR_BOTTOM])
	{
		m_pChild[CHILD_LB]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pNeighbor[NEIGHBOR_BOTTOM]->m_pChild[CHILD_LT];
		m_pChild[CHILD_RB]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pNeighbor[NEIGHBOR_BOTTOM]->m_pChild[CHILD_RT];
	}

	for (_ulong i = 0; i < CHILD_END; ++i)
	{
		if(NULL != m_pChild[i])
			m_pChild[i]->Make_Neighbor();
	}

}

void CQuadTree::SetUp_ChildCorner(CHILD eID, _ulong* pChildCorner)
{
	_ulong			dwTC, dwRC, dwBC, dwLC;

	dwTC = (_ulong)((m_dwCorner[CORNER_RT] + m_dwCorner[CORNER_LT]) * 0.5f);
	dwRC = (_ulong)((m_dwCorner[CORNER_RT] + m_dwCorner[CORNER_RB]) * 0.5f);
	dwBC = (_ulong)((m_dwCorner[CORNER_LB] + m_dwCorner[CORNER_RB]) * 0.5f);
	dwLC = (_ulong)((m_dwCorner[CORNER_LT] + m_dwCorner[CORNER_LB]) * 0.5f);

	switch(eID)
	{
	case CHILD_LT:
		pChildCorner[CORNER_LT] = m_dwCorner[CORNER_LT];
		pChildCorner[CORNER_RT] = dwTC;
		pChildCorner[CORNER_LB] = dwLC;
		pChildCorner[CORNER_RB] = m_dwCenter;		
		break;
	case CHILD_RT:
		pChildCorner[CORNER_LT] = dwTC;
		pChildCorner[CORNER_RT] = m_dwCorner[CORNER_RT];
		pChildCorner[CORNER_LB] = m_dwCenter;
		pChildCorner[CORNER_RB] = dwRC;		
		break;
	case CHILD_LB:
		pChildCorner[CORNER_LT] = dwLC;
		pChildCorner[CORNER_RT] = m_dwCenter;
		pChildCorner[CORNER_LB] = m_dwCorner[CORNER_LB];
		pChildCorner[CORNER_RB] = dwBC;		
		break;
	case CHILD_RB:
		pChildCorner[CORNER_LT] = m_dwCenter;
		pChildCorner[CORNER_RT] = dwRC;
		pChildCorner[CORNER_LB] = dwBC;
		pChildCorner[CORNER_RB] = m_dwCorner[CORNER_RB];		
		break;
	}
}

void Engine::CQuadTree::SetUp_NeighborCorner(NEIGHBOR eID, _ulong* pLT, _ulong* pRT, _ulong* pLB, _ulong* pRB, _ulong* pCenter)
{
	_ulong	dwWidth = m_dwCorner[CORNER_RT] - m_dwCorner[CORNER_LT];
	_ulong	dwHeight = m_dwCorner[CORNER_RT] - m_dwCorner[CORNER_RB];

	switch(eID)
	{
	case NEIGHBOR_LEFT:
		*pLT = m_dwCorner[CORNER_LT] - dwWidth;
		*pRT = m_dwCorner[CORNER_LT];
		*pLB = m_dwCorner[CORNER_LB] - dwWidth;
		*pRB = m_dwCorner[CORNER_LB];
		break;
	case NEIGHBOR_TOP:
		*pLT = m_dwCorner[CORNER_LT] + dwHeight;
		*pRT = m_dwCorner[CORNER_RT] + dwHeight;
		*pLB = m_dwCorner[CORNER_LT];
		*pRB = m_dwCorner[CORNER_RT];
		break;
	case NEIGHBOR_RIGHT:
		*pLT = m_dwCorner[CORNER_RT];
		*pRT = m_dwCorner[CORNER_RT] + dwWidth;
		*pLB = m_dwCorner[CORNER_RB];
		*pRB = m_dwCorner[CORNER_RB] + dwWidth;
		break;
	case NEIGHBOR_BOTTOM:
		*pLT = m_dwCorner[CORNER_LB];
		*pRT = m_dwCorner[CORNER_RB];
		*pLB = m_dwCorner[CORNER_LB] - dwHeight;
		*pRB = m_dwCorner[CORNER_RB] - dwHeight;
		break;
	}

	*pCenter = (*pLT + *pRT + *pLB + *pRB) * 0.25f;
}

CQuadTree* Engine::CQuadTree::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CQuadTree*	pInstance = new CQuadTree(pGraphicDev);
	
	return pInstance;
}

CQuadTree* Engine::CQuadTree::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _ulong* pCorner
									 , const _ulong& dwCntX, const _ulong& dwCntZ, const _float& fItv)
{
	CQuadTree*	pInstance = new CQuadTree(pGraphicDev);

	pInstance->m_dwCorner[CORNER_LT] = pCorner[CORNER_LT];
	pInstance->m_dwCorner[CORNER_RT] = pCorner[CORNER_RT];
	pInstance->m_dwCorner[CORNER_LB] = pCorner[CORNER_LB];
	pInstance->m_dwCorner[CORNER_RB] = pCorner[CORNER_RB];

	for (_ulong i = 0; i < CORNER_END; ++i)
		pInstance->m_dwCenter += pInstance->m_dwCorner[i];
	pInstance->m_dwCenter = (_ulong)(pInstance->m_dwCenter * 0.25f);

	pInstance->m_fRadius = sqrtf(pow((pInstance->m_dwCorner[CORNER_RB] - pInstance->m_dwCorner[CORNER_LB]) * fItv, 2)
		+ pow((pInstance->m_dwCorner[CORNER_RT] - pInstance->m_dwCorner[CORNER_RB]) / dwCntZ * fItv, 2)) * 0.5f;

	return pInstance;
}

void Engine::CQuadTree::Free(void)
{
	for (_ulong i = 0; i < CHILD_END; ++i)
		Engine::Safe_Release(m_pChild[i]);
	

	Engine::Safe_Release(m_pGraphicDev);
}


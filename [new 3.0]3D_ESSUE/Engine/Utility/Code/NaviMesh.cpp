#include "NaviMesh.h"
#include "Line2D.h"

USING(Engine)

Engine::CNaviMesh::CNaviMesh( LPDIRECT3DDEVICE9 pDevice )
: m_pDevice(pDevice)
, m_pLine(NULL)
, m_dwIndex(0)
{
	ZeroMemory(m_pNeighbor, sizeof(CNaviMesh*) * NEIGHBOR_END);
	ZeroMemory(m_vPoint, sizeof(MyVec3) * POINT_END);
	ZeroMemory(m_pLine2D, sizeof(CLine2D*) * LINE_END);
}

Engine::CNaviMesh::~CNaviMesh( void )
{

}

HRESULT Engine::CNaviMesh::Initialize( const MyVec3* pPointA, const MyVec3* pPointB, const MyVec3* pPointC, const _ulong& dwIndex )
{
	m_dwIndex = dwIndex;

	m_vPoint[POINT_A] = *pPointA;
	m_vPoint[POINT_B] = *pPointB;
	m_vPoint[POINT_C] = *pPointC;

	m_pLine2D[LINE_AB] = Engine::CLine2D::Create(pPointA, pPointB);
	m_pLine2D[LINE_BC] = Engine::CLine2D::Create(pPointB, pPointC);
	m_pLine2D[LINE_CA] = Engine::CLine2D::Create(pPointC, pPointA);

	if (FAILED(D3DXCreateLine(m_pDevice, &m_pLine)))
		return E_FAIL;

	return S_OK;
}

void Engine::CNaviMesh::Render( void )
{
	MyMat matView, matProj;

	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	MyVec3 vPoint[4];

	vPoint[0] = m_vPoint[0];
	vPoint[1] = m_vPoint[1];
	vPoint[2] = m_vPoint[2];
	vPoint[3] = m_vPoint[0];

	for (_uint i = 0; i < POINT_END; ++i)
	{
		D3DXVec3TransformCoord(&vPoint[i], &vPoint[i], &matView);

		if (vPoint[i].z < 0.f)
			vPoint[i].z = 0.f;

		D3DXVec3TransformCoord(&vPoint[i], &vPoint[i], &matProj);
	}

	MyMat matIden;
	D3DXMatrixIdentity(&matIden);

	m_pLine->SetWidth(5.f);
	m_pLine->Begin();

	m_pLine->DrawTransform(vPoint, POINT_END + 1, &matIden, D3DXCOLOR(0.f, 1.f, 0.f, 1.f));

	m_pLine->End();
}

bool Engine::CNaviMesh::Compare( const MyVec3* pFirst, const MyVec3* pSecond, CNaviMesh* pNeighbor )
{
	if (m_vPoint[POINT_A] == *pFirst)
	{
		if (m_vPoint[POINT_B] == *pSecond)
		{
			m_pNeighbor[NEIGHBOR_AB] = pNeighbor;
			return true;
		}
		else if (m_vPoint[POINT_C] == *pSecond)
		{
			m_pNeighbor[NEIGHBOR_CA] = pNeighbor;
			return true;
		}
	}

	if (m_vPoint[POINT_B] == *pFirst)
	{
		if (m_vPoint[POINT_A] == *pSecond)
		{
			m_pNeighbor[NEIGHBOR_AB] = pNeighbor;
			return true;
		}
		else if (m_vPoint[POINT_C] == *pSecond)
		{
			m_pNeighbor[NEIGHBOR_BC] = pNeighbor;
			return true;
		}
	}

	if (m_vPoint[POINT_C] == *pFirst)
	{
		if (m_vPoint[POINT_A] == *pSecond)
		{
			m_pNeighbor[NEIGHBOR_CA] = pNeighbor;
			return true;
		}
		else if (m_vPoint[POINT_B] == *pSecond)
		{
			m_pNeighbor[NEIGHBOR_BC] = pNeighbor;
			return true;
		}
	}

	return false;
}

bool Engine::CNaviMesh::CheckPass( const MyVec3* pStart, const MyVec3* pDir, NEIGHBORID* pNeighbor )
{
	MyVec2 vTemp = MyVec2(pStart->x + pDir->x, pStart->z + pDir->z);
	for (_uint i = 0; i < LINE_END; ++i)
	{
		if (m_pLine2D[i]->CheckLinePass(&vTemp))
		{
			*pNeighbor = NEIGHBORID(i);
			return true;
		}
	}
	return false;
}

void Engine::CNaviMesh::Free( void )
{
	for (_uint i = 0; i < LINE_END; ++i)
	{
		Engine::Safe_Release(m_pLine2D[i]);
	}
	Engine::Safe_Release(m_pLine);
}

CNaviMesh* Engine::CNaviMesh::Create( LPDIRECT3DDEVICE9 pDevice, const MyVec3* pPointA, const MyVec3* pPointB, const MyVec3* pPointC, const _ulong& dwIndex )
{
	CNaviMesh* pNaviMesh = new CNaviMesh(pDevice);

	if (FAILED(pNaviMesh->Initialize(pPointA, pPointB, pPointC, dwIndex)))
	{
		MSG_BOX("NaviMesh Created Failed");
		Engine::Safe_Release(pNaviMesh);
	}

	return pNaviMesh;
}


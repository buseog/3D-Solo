#include "CollisionBox.h"

USING(Engine)

Engine::CCollisionBox::CCollisionBox( LPDIRECT3DDEVICE9 pDevice )
: m_pDevice(pDevice)
, m_pVB(NULL)
, m_pIB(NULL)
, m_bCollision(false)
, m_bRender(false)
{
	m_pDevice->AddRef();
}

Engine::CCollisionBox::CCollisionBox( const CCollisionBox& rhs )
: m_pDevice(rhs.m_pDevice)
, m_pVB(rhs.m_pVB)
, m_pIB(rhs.m_pIB)
, m_MatLocal(rhs.m_MatLocal)
, m_MatParent(rhs.m_MatParent)
, m_tOBB(rhs.m_tOBB)
, m_bCollision(rhs.m_bCollision)
, m_MatBonus(rhs.m_MatBonus)
, m_bRender(rhs.m_bRender)
{
	m_pDevice->AddRef();
	m_pVB->AddRef();
	m_pIB->AddRef();

	for (_uint i = 0; i < COLL_END; ++i)
	{
		m_pTexture[i] = rhs.m_pTexture[i];
		m_pTexture[i]->AddRef();
	}
}

Engine::CCollisionBox::~CCollisionBox( void )
{

}

HRESULT Engine::CCollisionBox::Initialize( void )
{
	D3DXMatrixIdentity(&m_MatLocal);
	D3DXMatrixIdentity(&m_MatParent);
	D3DXMatrixIdentity(&m_MatBonus);

	for (_uint i = 0; i < COLL_END; ++i)
		D3DXCreateTexture(m_pDevice, 1, 1, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pTexture[i]);

	D3DLOCKED_RECT	LockRect;
	ZeroMemory(&LockRect, sizeof(D3DLOCKED_RECT));

	m_pTexture[COLL_FALSE]->LockRect(0, &LockRect, NULL, NULL);
	((_ulong*)LockRect.pBits)[0] = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);
	m_pTexture[COLL_FALSE]->UnlockRect(0);

	ZeroMemory(&LockRect, sizeof(D3DLOCKED_RECT));

	m_pTexture[COLL_TRUE]->LockRect(0, &LockRect, NULL, NULL);
	((_ulong*)LockRect.pBits)[0] = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
	m_pTexture[COLL_TRUE]->UnlockRect(0);

	if (FAILED(m_pDevice->CreateVertexBuffer(sizeof(VTXCUBETEX) * 8, 0, D3DFVF_VTXCUBETEX, D3DPOOL_MANAGED, &m_pVB, NULL)))
		return E_FAIL;

	VTXCUBETEX*		pVertex = NULL;
	
	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	pVertex[0].vPos = MyVec3(-0.5f, 0.5f, -0.5f);
	pVertex[0].vTexUV = pVertex[0].vPos;

	pVertex[1].vPos = MyVec3(0.5f, 0.5f, -0.5f);
	pVertex[1].vTexUV = pVertex[1].vPos;

	pVertex[2].vPos = MyVec3(0.5f, -0.5f, -0.5f);
	pVertex[2].vTexUV = pVertex[2].vPos;

	pVertex[3].vPos = MyVec3(-0.5f, -0.5f, -0.5f);
	pVertex[3].vTexUV = pVertex[3].vPos;

	pVertex[4].vPos = MyVec3(-0.5f, 0.5f, 0.5f);
	pVertex[4].vTexUV = pVertex[4].vPos;

	pVertex[5].vPos = MyVec3(0.5f, 0.5f, 0.5f);
	pVertex[5].vTexUV = pVertex[5].vPos;

	pVertex[6].vPos = MyVec3(0.5f, -0.5f, 0.5f);
	pVertex[6].vTexUV = pVertex[6].vPos;

	pVertex[7].vPos = MyVec3(-0.5f, -0.5f, 0.5f);
	pVertex[7].vTexUV = pVertex[7].vPos;

	m_pVB->Unlock();

	if(FAILED(m_pDevice->CreateIndexBuffer(sizeof(INDEX16) * 12, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL)))
		return E_FAIL;

	INDEX16*	pIndex = NULL;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	// +x
	pIndex[0]._0 = 1; pIndex[0]._1 = 6; pIndex[0]._2 = 2;
	pIndex[1]._0 = 1; pIndex[1]._1 = 5; pIndex[1]._2 = 6;

	// -x									   
	pIndex[2]._0 = 4; pIndex[2]._1 = 0; pIndex[2]._2 = 3;
	pIndex[3]._0 = 4; pIndex[3]._1 = 3; pIndex[3]._2 = 7;

	// +y									   
	pIndex[4]._0 = 4; pIndex[4]._1 = 5; pIndex[4]._2 = 1;
	pIndex[5]._0 = 4; pIndex[5]._1 = 1; pIndex[5]._2 = 0;

	// -y									   
	pIndex[6]._0 = 3; pIndex[6]._1 = 2; pIndex[6]._2 = 6;
	pIndex[7]._0 = 3; pIndex[7]._1 = 6; pIndex[7]._2 = 7;

	// +z									   
	pIndex[8]._0 = 7; pIndex[8]._1 = 6; pIndex[8]._2 = 5;
	pIndex[9]._0 = 7; pIndex[9]._1 = 5; pIndex[9]._2 = 4;

	// -z									   
	pIndex[10]._0 = 0; pIndex[10]._1 = 1; pIndex[10]._2 = 2;
	pIndex[11]._0 = 0; pIndex[11]._1 = 2; pIndex[11]._2 = 3;

	m_pIB->Unlock();


	return S_OK;
}

void Engine::CCollisionBox::Render( COLLTYPE eType, const MyMat* pMatWorld )
{
	if (eType == COLL_AABB)
	{
		MyMat matScale, matTrans;

		D3DXMatrixScaling(&matScale, D3DXVec3Length((MyVec3*)&pMatWorld->m[0][0]), D3DXVec3Length((MyVec3*)&pMatWorld->m[1][0]), D3DXVec3Length((MyVec3*)&pMatWorld->m[2][0]));
		D3DXMatrixTranslation(&matTrans, pMatWorld->m[3][0], pMatWorld->m[3][1], pMatWorld->m[3][2]);

		m_MatParent = matScale * matTrans;
	}
	else
		m_MatParent = *pMatWorld;

	if(m_bCollision == false)
		m_pDevice->SetTexture(0, m_pTexture[COLL_FALSE]);
	else
		m_pDevice->SetTexture(0, m_pTexture[COLL_TRUE]);
	
	m_pDevice->SetTransform(D3DTS_WORLD, &(m_MatLocal * m_MatParent));

	if (m_bRender == false)
		return;

	m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(VTXCUBETEX));
	m_pDevice->SetFVF(D3DFVF_VTXCUBETEX);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);

	m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

HRESULT Engine::CCollisionBox::ComputeBox( CStaticMesh* pMesh )
{
	pMesh->ComputeMesh(&m_vMin, &m_vMax);

	MyMat	matScale, matTrans;

	D3DXMatrixScaling(&matScale, m_vMax.x - m_vMin.x, m_vMax.y - m_vMin.y, m_vMax.z - m_vMin.z);
	D3DXMatrixTranslation(&matTrans, (m_vMax.x + m_vMin.x) / 2.f, (m_vMax.y + m_vMin.y) / 2.f, (m_vMax.z + m_vMin.z) / 2.f);

	m_MatLocal = matScale * matTrans;

	return S_OK;
}

HRESULT Engine::CCollisionBox::ComputeBox( CDynamicMesh* pMesh, const _uint& iIndex )
{
	pMesh->ComputeMesh(iIndex, &m_vMin, &m_vMax);

	MyMat	matScale, matTrans;

	D3DXMatrixScaling(&matScale, m_vMax.x - m_vMin.x, m_vMax.y - m_vMin.y, m_vMax.z - m_vMin.z);
	D3DXMatrixTranslation(&matTrans, (m_vMax.x + m_vMin.x) / 2.f, (m_vMax.y + m_vMin.y) / 2.f, (m_vMax.z + m_vMin.z) / 2.f);

	m_MatLocal = matScale * matTrans;

	return S_OK;
}

_bool Engine::CCollisionBox::CollisionAABB( CCollisionBox* pTargetBox )
{
	MyVec3	vWorldMin, vWorldMax;
	MyVec3	vTargetMin, vTargetMax;

	D3DXVec3TransformCoord(&vWorldMin, &vWorldMin, &m_MatParent);
	D3DXVec3TransformCoord(&vWorldMax, &vWorldMax, &m_MatParent);

	D3DXVec3TransformCoord(&vTargetMin, &vTargetMin, &pTargetBox->m_MatParent);
	D3DXVec3TransformCoord(&vTargetMax, &vTargetMax, &pTargetBox->m_MatParent);

	if(max(vWorldMin.x, vTargetMin.x) > min(vWorldMax.x, vTargetMax.x))
		return m_bCollision = false;

	if(max(vWorldMin.y, vTargetMin.y) > min(vWorldMax.y, vTargetMax.y))
		return m_bCollision = false;

	if(max(vWorldMin.z, vTargetMin.z) > min(vWorldMax.z, vTargetMax.z))
		return m_bCollision = false;

	return m_bCollision = true;
}

_bool Engine::CCollisionBox::CollisionOBB( CCollisionBox* pTargetBox )
{
	SetOBB();
	pTargetBox->SetOBB();

	MyVec3 vDis = pTargetBox->m_tOBB.vCenter - m_tOBB.vCenter;

	for (_uint i = 0; i < 3; ++i)
	{
		/////// 박스축
		float fDis = abs(D3DXVec3Dot(&vDis, &m_tOBB.vAxis[i]));
		float fSum = 0.f;

		for (_uint j = 0; j < 3; ++j)
		{
			fSum += abs(D3DXVec3Dot(&m_tOBB.vPoint[j], &m_tOBB.vAxis[i]));
			fSum += abs(D3DXVec3Dot(&pTargetBox->m_tOBB.vPoint[j], &m_tOBB.vAxis[i]));
		}
		if (fDis > fSum)
			return m_bCollision = false;
		else
			fSum = 0.f;

		////// 타겟박스축
		fDis = abs(D3DXVec3Dot(&vDis, &pTargetBox->m_tOBB.vAxis[i]));

		for (_uint j = 0; j < 3; ++j)
		{
			fSum += abs(D3DXVec3Dot(&m_tOBB.vPoint[j], &pTargetBox->m_tOBB.vAxis[i]));
			fSum += abs(D3DXVec3Dot(&pTargetBox->m_tOBB.vPoint[j], &pTargetBox->m_tOBB.vAxis[i]));
		}
		if (fDis > fSum)
			return m_bCollision = false;
		else
			fSum = 0.f;
	}

	return m_bCollision = true;
}

void Engine::CCollisionBox::SetOBB( void )
{
	MyVec3 vMin, vMax;
	D3DXVec3TransformCoord(&vMin, &m_vMin, &MyMat(m_MatBonus * m_MatParent));
	D3DXVec3TransformCoord(&vMax, &m_vMax, &MyMat(m_MatBonus * m_MatParent));

	m_tOBB.vCenter = (vMax + vMin) / 2.f;

	m_tOBB.vAxis[0] = vMax - MyVec3(vMin.x, vMax.y, vMax.z);
	m_tOBB.vAxis[1] = vMax - MyVec3(vMax.x, vMin.y, vMax.z);
	m_tOBB.vAxis[2] = vMax - MyVec3(vMax.x, vMax.y, vMin.z);

	for (_uint i = 0; i < 3; ++i)
		D3DXVec3Normalize(&m_tOBB.vAxis[i], &m_tOBB.vAxis[i]);

	m_tOBB.vPoint[0] = (vMax + MyVec3(vMax.x, vMin.y, vMin.z)) / 2.f - m_tOBB.vCenter;
	m_tOBB.vPoint[1] = (vMax + MyVec3(vMin.x, vMax.y, vMin.z)) / 2.f - m_tOBB.vCenter;
	m_tOBB.vPoint[2] = (vMax + MyVec3(vMin.x, vMin.y, vMax.z)) / 2.f - m_tOBB.vCenter;
}	


void Engine::CCollisionBox::SetCollision( _bool bTF )
{
	m_bCollision = bTF;
}

void Engine::CCollisionBox::Free( void )
{
	for (_uint i = 0; i < COLL_END; ++i)		
		Engine::Safe_Release(m_pTexture[i]);	

	Engine::Safe_Release(m_pDevice);
	Engine::Safe_Release(m_pVB);
	Engine::Safe_Release(m_pIB);
}

CComponent* Engine::CCollisionBox::Clone( void )
{
	return new CCollisionBox(*this);
}

CCollisionBox* Engine::CCollisionBox::Create( LPDIRECT3DDEVICE9 pDevice )
{
	CCollisionBox* pBox = new CCollisionBox(pDevice);

	if (FAILED(pBox->Initialize()))
	{
		MSG_BOX("CollisionBox Created Failed");
		Engine::Safe_Release(pBox);
	}
	return pBox;
}

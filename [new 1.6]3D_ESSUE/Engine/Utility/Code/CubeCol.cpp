#include "CubeCol.h"

USING(Engine)

Engine::CCubeCol::CCubeCol(LPDIRECT3DDEVICE9 pDevice)
: CVIBuffer(pDevice)
{

}

Engine::CCubeCol::CCubeCol(const CCubeCol& rhs)
: CVIBuffer(rhs)
{

}

Engine::CCubeCol::~CCubeCol(void)
{

}

HRESULT Engine::CCubeCol::Initialize(void)
{
	m_dwVtxCnt = 8;
	m_dwVtxSize = sizeof(VTXCOL);
	m_dwVtxFVF = D3DFVF_VTXCOL;

	m_dwTriCnt = 12;
	m_dwIdxSize = sizeof(INDEX32) * m_dwTriCnt;
	m_IndexFmt = D3DFMT_INDEX32;

	if(FAILED(Engine::CVIBuffer::Initialize()))
		return E_FAIL;

	// 할당된 메모리에 접근을 하자.
	VTXCOL*				pVertex = NULL;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	pVertex[0].vPos = MyVec3(-1.f, 1.f, -1.f);
	pVertex[0].vColor = D3DCOLOR_ARGB(255, 0, 255, 0);

	pVertex[1].vPos = MyVec3(1.f, 1.f, -1.f);
	pVertex[1].vColor = D3DCOLOR_ARGB(255, 0, 255, 0);

	pVertex[2].vPos = MyVec3(1.f, -1.f, -1.f);
	pVertex[2].vColor = D3DCOLOR_ARGB(255, 0, 255, 0);

	pVertex[3].vPos = MyVec3(-1.f, -1.f, -1.f);
	pVertex[3].vColor = D3DCOLOR_ARGB(255, 0, 255, 0);

	pVertex[4].vPos = MyVec3(-1.f, 1.f, 1.f);
	pVertex[4].vColor = D3DCOLOR_ARGB(255, 0, 255, 0);

	pVertex[5].vPos = MyVec3(1.f, 1.f, 1.f);
	pVertex[5].vColor = D3DCOLOR_ARGB(255, 0, 255, 0);

	pVertex[6].vPos = MyVec3(1.f, -1.f, 1.f);
	pVertex[6].vColor = D3DCOLOR_ARGB(255, 0, 255, 0);

	pVertex[7].vPos = MyVec3(-1.f, -1.f, 1.f);
	pVertex[7].vColor = D3DCOLOR_ARGB(255, 0, 255, 0);

	m_pVB->Unlock();

	INDEX32*			pIndex = NULL;

	_int iIndex = 0;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	//// x ===========
	// x+
	pIndex[iIndex]._0 = 1;
	pIndex[iIndex]._1 = 5;
	pIndex[iIndex]._2 = 6;
	++iIndex;

	pIndex[iIndex]._0 = 1;
	pIndex[iIndex]._1 = 6;
	pIndex[iIndex]._2 = 2;
	++iIndex;

	// x-
	pIndex[iIndex]._0 = 4;
	pIndex[iIndex]._1 = 0;
	pIndex[iIndex]._2 = 3;
	++iIndex;

	pIndex[iIndex]._0 = 4;
	pIndex[iIndex]._1 = 3;
	pIndex[iIndex]._2 = 7;
	++iIndex;

	//// y ===========
	// y+
	pIndex[iIndex]._0 = 4;
	pIndex[iIndex]._1 = 5;
	pIndex[iIndex]._2 = 1;
	++iIndex;

	pIndex[iIndex]._0 = 4;
	pIndex[iIndex]._1 = 1;
	pIndex[iIndex]._2 = 0;
	++iIndex;

	// y-
	pIndex[iIndex]._0 = 3;
	pIndex[iIndex]._1 = 2;
	pIndex[iIndex]._2 = 6;
	++iIndex;

	pIndex[iIndex]._0 = 3;
	pIndex[iIndex]._1 = 6;
	pIndex[iIndex]._2 = 7;
	++iIndex;

	//// z ===========
	// z+
	pIndex[iIndex]._0 = 7;
	pIndex[iIndex]._1 = 6;
	pIndex[iIndex]._2 = 5;
	++iIndex;

	pIndex[iIndex]._0 = 7;
	pIndex[iIndex]._1 = 5;
	pIndex[iIndex]._2 = 4;
	++iIndex;

	// z-
	pIndex[iIndex]._0 = 0;
	pIndex[iIndex]._1 = 1;
	pIndex[iIndex]._2 = 2;
	++iIndex;

	pIndex[iIndex]._0 = 0;
	pIndex[iIndex]._1 = 2;
	pIndex[iIndex]._2 = 3;

	m_pIB->Unlock();

	return S_OK;
}

void Engine::CCubeCol::Render(void)
{
	m_pDevice->SetStreamSource(0, m_pVB, 0, m_dwVtxSize);
	m_pDevice->SetFVF(m_dwVtxFVF);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_dwVtxCnt, 0, m_dwTriCnt);
}

void Engine::CCubeCol::Free(void)
{
	CVIBuffer::Free();
}

CComponent* Engine::CCubeCol::Clone(void)
{
	return new CCubeCol(*this);
}

CCubeCol* Engine::CCubeCol::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CCubeCol* pCube = new CCubeCol(pDevice);

	if (FAILED(pCube->Initialize()))
	{
		MSG_BOX("CubeTex Created Failed");
		Engine::Safe_Release(pCube);
	}

	return pCube;
}

#include "CubeTex.h"

USING(Engine)

Engine::CCubeTex::CCubeTex(LPDIRECT3DDEVICE9 pDevice)
: CVIBuffer(pDevice)
{

}

Engine::CCubeTex::CCubeTex(const CCubeTex& rhs)
: CVIBuffer(rhs)
{

}

Engine::CCubeTex::~CCubeTex(void)
{

}

HRESULT Engine::CCubeTex::Initialize(void)
{
	m_dwVtxCnt = 8;
	m_dwVtxSize = sizeof(VTXCUBETEX);
	m_dwVtxFVF = D3DFVF_VTXCUBETEX;

	m_dwTriCnt = 12;
	m_dwIdxSize = sizeof(INDEX32) * m_dwTriCnt;
	m_IndexFmt = D3DFMT_INDEX32;

	if(FAILED(Engine::CVIBuffer::Initialize()))
		return E_FAIL;

	// 할당된 메모리에 접근을 하자.
	VTXCUBETEX*				pVertex = NULL;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	pVertex[0].vPos = MyVec3(-1.f, 1.f, -1.f);
	pVertex[0].vTexUV = pVertex[0].vPos;

	pVertex[1].vPos = MyVec3(1.f, 1.f, -1.f);
	pVertex[1].vTexUV = pVertex[1].vPos;

	pVertex[2].vPos = MyVec3(1.f, -1.f, -1.f);
	pVertex[2].vTexUV = pVertex[2].vPos;

	pVertex[3].vPos = MyVec3(-1.f, -1.f, -1.f);
	pVertex[3].vTexUV = pVertex[3].vPos;

	pVertex[4].vPos = MyVec3(-1.f, 1.f, 1.f);
	pVertex[4].vTexUV = pVertex[4].vPos;

	pVertex[5].vPos = MyVec3(1.f, 1.f, 1.f);
	pVertex[5].vTexUV = pVertex[5].vPos;

	pVertex[6].vPos = MyVec3(1.f, -1.f, 1.f);
	pVertex[6].vTexUV = pVertex[6].vPos;

	pVertex[7].vPos = MyVec3(-1.f, -1.f, 1.f);
	pVertex[7].vTexUV = pVertex[7].vPos;

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

void Engine::CCubeTex::Render(void)
{
	m_pDevice->SetStreamSource(0, m_pVB, 0, m_dwVtxSize);
	m_pDevice->SetFVF(m_dwVtxFVF);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_dwVtxCnt, 0, m_dwTriCnt);
}

void Engine::CCubeTex::Free(void)
{
	CVIBuffer::Free();
}

CComponent* Engine::CCubeTex::Clone(void)
{
	return new CCubeTex(*this);
}

CCubeTex* Engine::CCubeTex::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CCubeTex* pCube = new CCubeTex(pDevice);

	if (FAILED(pCube->Initialize()))
	{
		MSG_BOX("CubeTex Created Failed");
		Engine::Safe_Release(pCube);
	}

	return pCube;
}

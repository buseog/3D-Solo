#include "RcTex.h"

USING(Engine)

Engine::CRcTex::CRcTex(LPDIRECT3DDEVICE9 pGraphicDev)
: CVIBuffer(pGraphicDev)
{
	
}

Engine::CRcTex::CRcTex(const CRcTex& rhs)
: CVIBuffer(rhs)
{
	
}

Engine::CRcTex::~CRcTex(void)
{

}

HRESULT Engine::CRcTex::Initialize(void)
{
	m_dwVtxCnt = 4;
	m_dwVtxSize = sizeof(VTXTEX);
	m_dwVtxFVF = D3DFVF_VTXTEX;

	m_dwTriCnt = 2;
	m_dwIdxSize = sizeof(INDEX16) * m_dwTriCnt;
	m_IndexFmt = D3DFMT_INDEX16;

	if(FAILED(Engine::CVIBuffer::Initialize()))
		return E_FAIL;

	// 할당된 메모리에 접근을 하자.
	VTXTEX*				pVertex = NULL;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	pVertex[0].vPos = MyVec3(-0.5f, 0.5f, 0.f);
	pVertex[0].vTexUV = MyVec2(0.f, 0.f);

	pVertex[1].vPos = MyVec3(0.5f, 0.5f, 0.f);
	pVertex[1].vTexUV = MyVec2(1.f, 0.f);

	pVertex[2].vPos = MyVec3(0.5f, -0.5f, 0.f);
	pVertex[2].vTexUV = MyVec2(1.f, 1.f);

	pVertex[3].vPos = MyVec3(-0.5f, -0.5f, 0.f);
	pVertex[3].vTexUV = MyVec2(0.0f, 1.0f);

	m_pVB->Unlock();

	INDEX16*			pIndex = NULL;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	pIndex[0]._0 = 0;
	pIndex[0]._1 = 1;
	pIndex[0]._2 = 2;

	pIndex[1]._0 = 0;
	pIndex[1]._1 = 2;
	pIndex[1]._2 = 3;

	m_pIB->Unlock();

	return S_OK;
}

void Engine::CRcTex::Render(void)
{
	m_pDevice->SetStreamSource(0, m_pVB, 0, m_dwVtxSize);
	m_pDevice->SetFVF(m_dwVtxFVF);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_dwVtxCnt, 0, m_dwTriCnt);
}

void Engine::CRcTex::Free(void)
{
	Engine::CVIBuffer::Free();
}

CComponent* Engine::CRcTex::Clone(void)
{
	return new CRcTex(*this);
}

CRcTex* Engine::CRcTex::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CRcTex* pRcTex = new CRcTex(pDevice);

	if (FAILED(pRcTex->Initialize()))
	{
		MSG_BOX("RcTex Created Failed");
		Engine::Safe_Release(pRcTex);
	}

	return pRcTex;
}

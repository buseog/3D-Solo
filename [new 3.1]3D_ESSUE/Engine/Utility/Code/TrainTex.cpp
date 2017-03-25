#include "TrainTex.h"

USING(Engine)

Engine::CTrainTex::CTrainTex(LPDIRECT3DDEVICE9 pGraphicDev)
: CVIBuffer(pGraphicDev)
{
	
}

Engine::CTrainTex::CTrainTex(const CTrainTex& rhs)
: CVIBuffer(rhs)
{
	
}

Engine::CTrainTex::~CTrainTex(void)
{

}

HRESULT Engine::CTrainTex::Initialize(void)
{
	m_dwVtxCnt = 40;
	m_dwVtxSize = sizeof(VTXTEX);
	m_dwVtxFVF = D3DFVF_VTXTEX;

	m_dwTriCnt = m_dwVtxCnt - 1;
	m_dwIdxSize = sizeof(INDEX16) * m_dwTriCnt;
	m_IndexFmt = D3DFMT_INDEX16;

	if(FAILED(Engine::CVIBuffer::Initialize()))
		return E_FAIL;

	VTXTEX*				pVertex = NULL;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	_uint SizeY = (m_dwVtxCnt - 1) / 2;
	for (_uint i = 0; i < m_dwVtxCnt; ++i)
	{
		_uint iX = i % 2;
		_uint iY = i / 2;

		if (iX == 0)
		{
			pVertex[i].vPos = MyVec3(-1.f, (_float)iY, 0.f);
			pVertex[i].vTexUV = MyVec2(0.f, 1.f - ((1.f / SizeY) * iY));
		}
		else
		{
			pVertex[i].vPos = MyVec3(1.f, (_float)iY, 0.f);
			pVertex[i].vTexUV = MyVec2(1.f, 1.f - ((1.f / SizeY) * iY));
		}
	}

	m_pVB->Unlock();

	INDEX16*			pIndex = NULL;

	_ulong		dwTriIdx = 0;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	for (_uint i = 0; i < m_dwVtxCnt - 2; i += 2)
	{
		pIndex[dwTriIdx]._0 = i + 2;
		pIndex[dwTriIdx]._1 = i + 1;
		pIndex[dwTriIdx]._2 = i;
		++dwTriIdx;

		pIndex[dwTriIdx]._0 = i + 2;
		pIndex[dwTriIdx]._1 = i + 3;
		pIndex[dwTriIdx]._2 = i + 1;
		++dwTriIdx;
	}
	m_pIB->Unlock();

	return S_OK;
}

void Engine::CTrainTex::Render(void)
{
	m_pDevice->SetStreamSource(0, m_pVB, 0, m_dwVtxSize);
	m_pDevice->SetFVF(m_dwVtxFVF);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_dwVtxCnt, 0, m_dwTriCnt);
}

void Engine::CTrainTex::Free(void)
{
	Engine::CVIBuffer::Free();
}

CComponent* Engine::CTrainTex::Clone(void)
{
	return new CTrainTex(*this);
}

CTrainTex* Engine::CTrainTex::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CTrainTex* pTrainTex = new CTrainTex(pDevice);

	if (FAILED(pTrainTex->Initialize()))
	{
		MSG_BOX("TrainTex Created Failed");
		Engine::Safe_Release(pTrainTex);
	}

	return pTrainTex;
}

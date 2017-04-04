#include "ShaderBuffer.h"

USING(Engine)

Engine::CShaderBuffer::CShaderBuffer(LPDIRECT3DDEVICE9 pGraphicDev)
: CVIBuffer(pGraphicDev)
{

}

Engine::CShaderBuffer::CShaderBuffer(const CShaderBuffer& rhs)
: CVIBuffer(rhs)
{

}

Engine::CShaderBuffer::~CShaderBuffer(void)
{

}

HRESULT Engine::CShaderBuffer::Initialize(const _uint& iSizeX, const _uint& iSizeY)
{
	m_dwVtxCnt = 4;
	m_dwVtxSize = sizeof(VTXSCREENTEX);
	m_dwVtxFVF = D3DFVF_VTXSCREENTEX;

	m_dwTriCnt = 2;
	m_dwIdxSize = sizeof(INDEX16) * m_dwTriCnt;
	m_IndexFmt = D3DFMT_INDEX16;

	if(FAILED(Engine::CVIBuffer::Initialize()))
		return E_FAIL;

	VTXSCREENTEX*			pVertex = NULL;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	pVertex[0].vPosition = MyVec4(0.0f, 0.0f, 0.0f, 1.f);
	pVertex[0].vTexUV = MyVec2(0.f, 0.f);

	pVertex[1].vPosition = MyVec4((_float)iSizeX, 0.0f, 0.0f, 1.f);
	pVertex[1].vTexUV = MyVec2(1.f, 0.f);

	pVertex[2].vPosition = MyVec4((_float)iSizeX, (_float)iSizeY, 0.0f, 1.f);
	pVertex[2].vTexUV = MyVec2(1.f, 1.f);

	pVertex[3].vPosition = MyVec4(0.0f, (_float)iSizeY, 0.0f, 1.f);
	pVertex[3].vTexUV = MyVec2(0.f, 1.f);

	m_pVB->Unlock();

	Engine::INDEX16*	pIndex = NULL;

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

void Engine::CShaderBuffer::Render(void)
{
	m_pDevice->SetStreamSource(0, m_pVB, 0, m_dwVtxSize);
	m_pDevice->SetFVF(m_dwVtxFVF);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_dwVtxCnt, 0, m_dwTriCnt);
}

void Engine::CShaderBuffer::Free(void)
{
	Engine::CVIBuffer::Free();
}

CComponent* Engine::CShaderBuffer::Clone(void)
{
	return new CShaderBuffer(*this);
}

CShaderBuffer* Engine::CShaderBuffer::Create(LPDIRECT3DDEVICE9 pDevice, const _uint& iSizeX, const _uint& iSizeY)
{
	CShaderBuffer* pShaderBuffer = new CShaderBuffer(pDevice);

	if (FAILED(pShaderBuffer->Initialize(iSizeX, iSizeY)))
	{
		MSG_BOX("ShaderBuffer Created Failed");
		Engine::Safe_Release(pShaderBuffer);
	}

	return pShaderBuffer;
}

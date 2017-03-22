#include "VIBuffer.h"

USING(Engine)

Engine::CVIBuffer::CVIBuffer( LPDIRECT3DDEVICE9 pDevice )
: m_pDevice(pDevice)
, m_pVB(NULL)
, m_pIB(NULL)
, m_dwVtxCnt(0)
, m_dwVtxSize(0)
, m_dwVtxFVF(0)
{
	m_pDevice->AddRef();
}

Engine::CVIBuffer::CVIBuffer( const CVIBuffer& rhs )
: m_pDevice(rhs.m_pDevice)
, m_pVB(rhs.m_pVB)
, m_pIB(rhs.m_pIB)
, m_dwVtxCnt(rhs.m_dwVtxCnt)
, m_dwVtxSize(rhs.m_dwVtxSize)
, m_dwVtxFVF(rhs.m_dwVtxFVF)
, m_dwIdxSize(rhs.m_dwIdxSize)
, m_dwTriCnt(rhs.m_dwTriCnt)
, m_IndexFmt(rhs.m_IndexFmt)
{
	m_pDevice->AddRef();
	m_pVB->AddRef();
	m_pIB->AddRef();
}

Engine::CVIBuffer::~CVIBuffer( void )
{

}

HRESULT Engine::CVIBuffer::Initialize( void )
{

	if(FAILED(m_pDevice->CreateVertexBuffer(m_dwVtxSize * m_dwVtxCnt, 0, m_dwVtxFVF, D3DPOOL_MANAGED, &m_pVB, NULL)))
		return E_FAIL;

	if(FAILED(m_pDevice->CreateIndexBuffer(m_dwIdxSize, 0, m_IndexFmt, D3DPOOL_MANAGED, &m_pIB, NULL)))
		return E_FAIL;

	return S_OK;
}

void Engine::CVIBuffer::Render( void )
{

}

_ulong Engine::CVIBuffer::GetTriCount( void )
{
	return m_dwTriCnt;
}

void Engine::CVIBuffer::Free( void )
{
	Engine::CComponent::Free();

	Engine::Safe_Release(m_pDevice);
	Engine::Safe_Release(m_pVB);
	Engine::Safe_Release(m_pIB);
}


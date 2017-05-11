#include "Target.h"

USING(Engine)

Engine::CTarget::CTarget( LPDIRECT3DDEVICE9 pDevice )
: m_pDevice(pDevice)
, m_pTargetTexture(NULL)
, m_pTargetSurface(NULL)
, m_bExcept(false)
{
	m_pDevice->AddRef();
}

Engine::CTarget::~CTarget( void )
{

}

HRESULT Engine::CTarget::Initialize( const _uint& iSizeX, const _uint& iSizeY, D3DFORMAT Format, D3DXCOLOR Color )
{
	if (FAILED(D3DXCreateTexture(m_pDevice, iSizeX, iSizeY, 1, D3DUSAGE_RENDERTARGET, Format, D3DPOOL_DEFAULT, &m_pTargetTexture)))
		return E_FAIL;

	m_pTargetTexture->GetSurfaceLevel(0, &m_pTargetSurface);
	Engine::Safe_Release(m_pTargetTexture);
	
	m_Color = Color;

	return S_OK;
}

HRESULT Engine::CTarget::Initialize_Debug( const _float& fX, const _float& fY, const _float& fSizeX, const _float& fSizeY )
{
	if (FAILED(m_pDevice->CreateVertexBuffer(sizeof(VTXSCREENTEX) * 4, 0, D3DFVF_VTXSCREENTEX, D3DPOOL_MANAGED, &m_pVB, NULL)))
		return E_FAIL;
	
	if (FAILED(m_pDevice->CreateIndexBuffer(sizeof(INDEX16) * 2, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL)))
		return E_FAIL;

	VTXSCREENTEX*		pVertex = NULL;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	pVertex[0].vPosition = MyVec4(fX, fY, 0.f, 1.f);
	pVertex[0].vTexUV = MyVec2(0.f, 0.f);

	pVertex[1].vPosition = MyVec4(fX + fSizeX, fY, 0.f, 1.f);
	pVertex[1].vTexUV = MyVec2(1.f, 0.f);

	pVertex[2].vPosition = MyVec4(fX + fSizeX, fY + fSizeY, 0.f, 1.f);
	pVertex[2].vTexUV = MyVec2(1.f, 1.f);

	pVertex[3].vPosition = MyVec4(fX, fY + fSizeY, 0.f, 1.f);
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

void Engine::CTarget::Render_Debug( void )
{
	m_pDevice->SetTexture(0, m_pTargetTexture);
	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(VTXSCREENTEX));
	m_pDevice->SetFVF(D3DFVF_VTXSCREENTEX);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);
}

void Engine::CTarget::ClearTarget( void )
{
	if (m_bExcept == true)
		return;

	m_pDevice->GetRenderTarget(0, &m_pOldSurface);
	m_pDevice->SetRenderTarget(0, m_pTargetSurface);
	m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET, m_Color, 1.f, 0);
	m_pDevice->SetRenderTarget(0, m_pOldSurface);
	Engine::Safe_Release(m_pOldSurface);
}

void Engine::CTarget::SetUpDevice( const _uint& iIndex )
{
	m_pDevice->GetRenderTarget(iIndex, &m_pOldSurface);
	m_pDevice->SetRenderTarget(iIndex, m_pTargetSurface);
}

void Engine::CTarget::ResetDevice( const _uint& iIndex )
{
	m_pDevice->SetRenderTarget(iIndex, m_pOldSurface);
	Engine::Safe_Release(m_pOldSurface);
}


void Engine::CTarget::SetUpShader( LPD3DXEFFECT pEffect, const char* pTexKey )
{
	pEffect->SetTexture(pTexKey, m_pTargetTexture);
}

void Engine::CTarget::Free( void )
{
	Engine::Safe_Release(m_pDevice);
	Engine::Safe_Release(m_pTargetTexture);
	Engine::Safe_Release(m_pVB);
	Engine::Safe_Release(m_pIB);
}

CTarget* Engine::CTarget::Create( LPDIRECT3DDEVICE9 pDevice, const _uint& iSizeX, const _uint& iSizeY, D3DFORMAT Format, D3DXCOLOR Color )
{
	CTarget* pTarget = new CTarget(pDevice);

	if (FAILED(pTarget->Initialize(iSizeX, iSizeY,	Format, Color)))
	{
		MSG_BOX("Target Created Failed");
		Engine::Safe_Release(pTarget);
	}
	return pTarget;
}


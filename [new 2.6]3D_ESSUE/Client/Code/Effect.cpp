#include "StdAfx.h"
#include "Effect.h"

CEffect::CEffect( LPDIRECT3DDEVICE9 pDevice)
: Engine::CGameObject(pDevice)
, m_pRendererCom(NULL)
, m_pTransformCom(NULL)
, m_pTextureCom(NULL)
, m_pBufferCom(NULL)
, m_pShaderCom(NULL)
{

}

CEffect::~CEffect( void )
{

}

HRESULT CEffect::Initialize( const _tchar* pName )
{
	if (FAILED(Ready_Component(pName)))
		return E_FAIL;

	return S_OK;
}

_int CEffect::Update( const _float& fDelta )
{
	return 0;
}

void CEffect::Render( void )
{

}

HRESULT CEffect::Ready_Component( const _tchar* pName )
{
	return S_OK;
}

void CEffect::Free( void )
{

}

CEffect* CEffect::Create( LPDIRECT3DDEVICE9 pDevice, const _tchar* pName )
{
	CEffect* pEffect = new CEffect(pDevice);

	if (FAILED(pEffect->Initialize(pName)))
	{
		MSG_BOX("Effect Created Failed");
		Engine::Safe_Release(pEffect);
	}
	return pEffect;
}

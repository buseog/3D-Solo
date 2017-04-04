#include "StdAfx.h"
#include "EffectMgr.h"

IMPLEMENT_SINGLETON(CEffectMgr)

CEffectMgr::CEffectMgr(void)
{
}

CEffectMgr::~CEffectMgr(void)
{
}

HRESULT CEffectMgr::Initialize( LPDIRECT3DDEVICE9 pDevice )
{
	return S_OK;
}

_int CEffectMgr::Update( const _float& fDelta )
{
	return 0;
}

void CEffectMgr::Render( void )
{

}

void CEffectMgr::Free(void)
{

}


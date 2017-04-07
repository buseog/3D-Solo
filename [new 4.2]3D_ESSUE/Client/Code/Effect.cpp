#include "StdAfx.h"
#include "Effect.h"

CEffect::CEffect( LPDIRECT3DDEVICE9 pDevice)
: Engine::CGameObject(pDevice)
, m_pRendererCom(NULL)
, m_pTransformCom(NULL)
, m_pTextureCom(NULL)
, m_pBufferCom(NULL)
, m_pShaderCom(NULL)
, m_pTarget(NULL)
, m_pBone(NULL)
, m_pEffectKey(NULL)
, m_iType(0)
, m_iIndex(0)
{
}

CEffect::~CEffect( void )
{

}

_int CEffect::Update( const _float& fDelta )
{
	return 0;
}

void CEffect::Render( void )
{
	
}

_float CEffect::GetSizeDelta( EFFECT* pEffect )
{
	if (pEffect->fSizeWayTime != 0.f)
	{
		if ((pEffect->fLifeTime / pEffect->fMaxLife) < pEffect->fSizeWayTime)
		{
			return pEffect->fLifeTime / pEffect->fSizeWayTime * pEffect->fSizeWayDelta;
		}
		else
		{
			return (pEffect->fLifeTime - pEffect->fSizeWayTime) / (pEffect->fMaxLife - pEffect->fSizeWayTime) * (pEffect->fSizeDelta - pEffect->fSizeWayDelta) + pEffect->fSizeWayDelta;
		}
	}
		
	return pEffect->fLifeTime / pEffect->fMaxLife * pEffect->fSizeDelta;
}

_float CEffect::GetRollDelta( EFFECT* pEffect )
{
	if (pEffect->fRollWayTime != 0.f)
	{
		if ((pEffect->fLifeTime / pEffect->fMaxLife) < pEffect->fRollWayTime)
		{
			return pEffect->fLifeTime / pEffect->fRollWayTime * pEffect->fRollWayDelta;
		}
		else
		{
			return (pEffect->fLifeTime - pEffect->fRollWayTime) / (pEffect->fMaxLife - pEffect->fRollWayTime) * (pEffect->fRollDelta - pEffect->fRollWayDelta) + pEffect->fRollWayDelta;
		}
	}


	return pEffect->fLifeTime / pEffect->fMaxLife * pEffect->fRollDelta;
}

_uint CEffect::GetAlphaDelta( EFFECT* pEffect )
{
	if (pEffect->fAlphaWayTime != 0.f)
	{
		if ((pEffect->fLifeTime / pEffect->fMaxLife) < pEffect->fAlphaWayTime)
		{
			return _uint(pEffect->fLifeTime / pEffect->fAlphaWayTime * pEffect->fAlphaWayDelta);
		}
		else
		{
			return _uint((pEffect->fLifeTime - pEffect->fAlphaWayTime) / (pEffect->fMaxLife - pEffect->fAlphaWayTime) * (pEffect->fAlphaDelta - pEffect->fAlphaWayDelta) + pEffect->fAlphaWayDelta);
		}
	}

	return _uint(pEffect->fLifeTime / pEffect->fMaxLife * pEffect->fAlphaDelta);
}

D3DXVECTOR3 CEffect::GetScaleDelta( EFFECT* pEffect )
{
	D3DXVECTOR3 vDelta = (pEffect->fLifeTime / pEffect->fMaxLife) * pEffect->vScaleDelta;

	return vDelta;
}

D3DXVECTOR3 CEffect::GetColorDelta( EFFECT* pEffect )
{
	D3DXVECTOR3 vDelta = (pEffect->fLifeTime / pEffect->fMaxLife) * pEffect->vColorDelta;
	
	return vDelta;
}

bool CEffect::isDead( void )
{
	EFFECTLIST::iterator iter = m_EffectList.begin();
	EFFECTLIST::iterator iter_end = m_EffectList.end();

	if (iter == iter_end)
		return false;

	for ( ; iter != iter_end; ++iter)
	{	
		if ((*iter)->bAlive == true)
			return false;
	}
	return true;
}

void CEffect::Free( void )
{
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pTextureCom);
	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pShaderCom);

	Engine::CGameObject::Free();
}
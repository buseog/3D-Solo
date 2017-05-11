#include "StdAfx.h"
#include "Effect.h"

CEffect::CEffect( LPDIRECT3DDEVICE9 pDevice)
: Engine::CGameObject(pDevice)
, m_pRendererCom(NULL)
, m_pTransformCom(NULL)
, m_pTextureCom(NULL)
, m_pBufferCom(NULL)
, m_pShaderCom(NULL)
, m_pVB(NULL)
, m_pIB(NULL)
, m_pVBInstance(NULL)
, m_pVertexDecl(NULL)
, m_pTarget(NULL)
, m_pBone(NULL)
, m_pEffectKey(NULL)
, m_iType(0)
, m_iIndex(0)
, m_fDurationAcc(0.f)
, m_fIntervalAcc(0.f)
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

HRESULT CEffect::InitInstance( void )
{
	D3DVERTEXELEMENT9 g_VertexElemHardware[] =
	{
		{ 0, 0,	D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 1, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		{ 1, 4, D3DDECLTYPE_FLOAT4,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
		{ 1, 20, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 },
		{ 1, 36, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3 },
		{ 1, 52, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 4 },
		D3DDECL_END()
	};

	if (FAILED(m_pDevice->CreateVertexDeclaration(g_VertexElemHardware, &m_pVertexDecl)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateVertexBuffer(4 * sizeof(PARTICLEVTX), 0, 0, D3DPOOL_MANAGED, &m_pVB, 0)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateIndexBuffer(2 * sizeof( Engine::INDEX16 ), 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, 0)))
		return E_FAIL;

	PARTICLEVTX* pVerts;

	m_pVB->Lock( 0, NULL, ( void** )&pVerts, 0 );

	pVerts[0].vPos = D3DXVECTOR3(-1.f, 1.f, 0.f);
	pVerts[0].vTexUV = D3DXVECTOR2(0.f, 0.f);

	pVerts[1].vPos = D3DXVECTOR3(1.f, 1.f, 0.f);
	pVerts[1].vTexUV = D3DXVECTOR2(1.f, 0.f);

	pVerts[2].vPos = D3DXVECTOR3(1.f, -1.f, 0.f);
	pVerts[2].vTexUV = D3DXVECTOR2(1.f, 1.f);

	pVerts[3].vPos = D3DXVECTOR3(-1.f, -1.f, 0.f);
	pVerts[3].vTexUV = D3DXVECTOR2(0.f, 1.f);

	m_pVB->Unlock();

	Engine::INDEX16* pIndices;
	m_pIB->Lock( 0, NULL, ( void** )&pIndices, 0 );

	pIndices[0]._0 = 0;
	pIndices[0]._1 = 1;
	pIndices[0]._2 = 2;

	pIndices[1]._0 = 0;
	pIndices[1]._1 = 2;
	pIndices[1]._2 = 3;

	m_pIB->Unlock();

	if (FAILED(m_pDevice->CreateVertexBuffer(500 * sizeof(PARTICLE_INSTANCE), 0, 0, D3DPOOL_MANAGED, &m_pVBInstance, 0)))
		return E_FAIL;

	return S_OK;
}

_float CEffect::GetSizeDelta( EFFECT* pEffect )
{
	if (pEffect->fSizeWayTime != 0.f)
	{
		if ((pEffect->fLifeTime / pEffect->fMaxLife) < pEffect->fSizeWayTime)
		{
			return pEffect->fLifeTime / (pEffect->fSizeWayTime * pEffect->fMaxLife) * pEffect->fSizeWayDelta;
		}
		else
		{
			return (pEffect->fLifeTime - pEffect->fSizeWayTime * pEffect->fMaxLife) / (pEffect->fMaxLife - pEffect->fSizeWayTime * pEffect->fMaxLife) * (pEffect->fSizeDelta - pEffect->fSizeWayDelta) + pEffect->fSizeWayDelta;
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
			return pEffect->fLifeTime / (pEffect->fRollWayTime * pEffect->fMaxLife) * pEffect->fRollWayDelta;
		}
		else
		{
			return (pEffect->fLifeTime - pEffect->fRollWayTime * pEffect->fMaxLife) / (pEffect->fMaxLife - pEffect->fRollWayTime * pEffect->fMaxLife) * (pEffect->fRollDelta - pEffect->fRollWayDelta) + pEffect->fRollWayDelta;
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
			return _uint(pEffect->fLifeTime / (pEffect->fAlphaWayTime * pEffect->fMaxLife) * pEffect->fAlphaWayDelta);
		}
		else
		{
			return _uint((pEffect->fLifeTime - pEffect->fAlphaWayTime * pEffect->fMaxLife) / (pEffect->fMaxLife - pEffect->fAlphaWayTime * pEffect->fMaxLife) * (pEffect->fAlphaDelta - pEffect->fAlphaWayDelta) + pEffect->fAlphaWayDelta);
		}
	}

	return _uint(pEffect->fLifeTime / pEffect->fMaxLife * pEffect->fAlphaDelta) ;
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

	if (m_pVB != NULL)
	{
		Engine::Safe_Release(m_pVertexDecl);
		Engine::Safe_Release(m_pVBInstance);
		Engine::Safe_Release(m_pVB);
		Engine::Safe_Release(m_pIB);
	}
	Engine::CGameObject::Free();
}

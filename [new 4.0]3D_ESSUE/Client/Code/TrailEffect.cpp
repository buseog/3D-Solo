#include "StdAfx.h"
#include "TrailEffect.h"
#include "LongBlade.h"

CTrailEffect::CTrailEffect( LPDIRECT3DDEVICE9 pDevice)
: Engine::CGameObject(pDevice)
, m_pRendererCom(NULL)
, m_pTransformCom(NULL)
, m_pTextureCom(NULL)
, m_pBufferCom(NULL)
, m_pShaderCom(NULL)
, m_iCount(0)
, m_iType(0)
{

}

CTrailEffect::~CTrailEffect( void )
{

}

HRESULT CTrailEffect::Initialize( void )
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_fLifeTime = 1.0f;
	m_fSaveTime = 0.f;

	return S_OK;
}

_int CTrailEffect::Update( const _float& fDelta )
{
	Engine::CGameObject::Update(fDelta);

	m_pRendererCom->AddRenderList(Engine::CRenderer::RENDER_ALPHA, this);

	m_fLifeTime -= fDelta;
	if (m_fLifeTime <= 0.f)
		m_bDestroy = true;

	m_fSaveTime -= fDelta;

	if (m_iCount >= 40)
	{
		for (_uint i = 0; i < 38; ++i)
			m_vecPos[i] = m_vecPos[i + 2];

		m_iCount -= 2;
	}

	if (m_iCount < 40 && m_fSaveTime < 0.f)
	{
		MyMat matSword = *((CLongBlade*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(1))->GetComputeMatrix();

		MyVec3 vRight = *(MyVec3*)&matSword.m[0][0]; 
		D3DXVec3Normalize(&vRight, &vRight);
		MyVec3 vMin = *(MyVec3*)&matSword.m[3][0] + vRight * 2.f;
		MyVec3 vMax = *(MyVec3*)&matSword.m[3][0] + vRight * 8.f;


		m_vecPos[m_iCount] = MyVec4(max(vMin, vMax), 1.f);
		++m_iCount;
		m_vecPos[m_iCount] = MyVec4(min(vMin, vMax), 1.f);
		++m_iCount;
	}

	//m_fFrame += 6.f * fDelta;
	//if (m_fFrame > 6.f)
	//	m_fFrame = 0.f;

	return 0;
}

void CTrailEffect::Render( void )
{
	LPD3DXEFFECT	pEffect = m_pShaderCom->GetEffect();
	if (pEffect == NULL)
		return;

	SetUpShader(pEffect);
}

void CTrailEffect::SetUpShader( LPD3DXEFFECT pEffect )
{
	pEffect->AddRef();

	m_pTransformCom->SetUpShader(pEffect);

	MyMat matView, matProj;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);
	pEffect->SetVectorArray("g_TrailPos", m_vecPos, m_iCount);
	pEffect->SetInt("g_iCount", m_iCount);

	m_pTextureCom->SetUpShader(pEffect, 1);

	pEffect->Begin(NULL, 0);
	pEffect->BeginPass(3);

	m_pBufferCom->Render(); 

	pEffect->EndPass();
	pEffect->End();

	Engine::Safe_Release(pEffect);
}

void CTrailEffect::SetSaveTime(const _float& fSaveTime)
{
	m_fSaveTime = fSaveTime;
}

void CTrailEffect::SetLifeTime(const _float& fLifeTime)
{
	m_fLifeTime += fLifeTime;
}


HRESULT CTrailEffect::Ready_Component( void )
{
	m_pRendererCom = (Engine::CRenderer*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STATIC, L"Component_Renderer");
	if (m_pRendererCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(Engine::CGameObject::UPDATE_NONE, L"Com_Renderer", m_pRendererCom)))
		return E_FAIL;

	m_pTransformCom = (Engine::CTransform*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STATIC, L"Component_Transform");
	if (m_pTransformCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(Engine::CGameObject::UPDATE_DO, L"Com_Transform", m_pTransformCom)))
		return E_FAIL;

	m_pTextureCom = (Engine::CTexture*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Texture_LongBladeTrail");
	if (m_pTextureCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(Engine::CGameObject::UPDATE_NONE, L"Com_Texture", m_pTextureCom)))
		return E_FAIL;

	m_pBufferCom = (Engine::CTrailTex*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Buffer_Trail");
	if (m_pBufferCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(Engine::CGameObject::UPDATE_NONE, L"Com_Buffer", m_pBufferCom)))
		return E_FAIL;

	m_pShaderCom = (Engine::CShader*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Shader_Effect");
	if (m_pShaderCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(Engine::CGameObject::UPDATE_NONE, L"Com_Shader", m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CTrailEffect::Free( void )
{
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pTextureCom);
	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pShaderCom);

	Engine::CGameObject::Free();
}

CTrailEffect* CTrailEffect::Create( LPDIRECT3DDEVICE9 pDevice )
{
	CTrailEffect* pEffect = new CTrailEffect(pDevice);

	if (FAILED(pEffect->Initialize()))
	{
		MSG_BOX("Effect Created Failed");
		Engine::Safe_Release(pEffect);
	}
	return pEffect;
}

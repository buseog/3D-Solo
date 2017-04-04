#include "StdAfx.h"
#include "RuinBlade.h"
#include "LongBlade.h"

CRuinBlade::CRuinBlade( LPDIRECT3DDEVICE9 pDevice)
: Engine::CGameObject(pDevice)
, m_pRendererCom(NULL)
, m_pTransformCom(NULL)
, m_pTextureCom(NULL)
, m_pBufferCom(NULL)
, m_pShaderCom(NULL)

{

}

CRuinBlade::~CRuinBlade( void )
{

}

HRESULT CRuinBlade::Initialize( void )
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	Engine::CTransform* pTrans = (Engine::CTransform*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(0)->GetComponent(L"Com_Transform");
	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_POS, pTrans->GetInfomation(Engine::CTransform::INFO_POS));
	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_SCALE, &MyVec3(15.f, 15.f, 15.f));
	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_ANGLE, &MyVec3(D3DXToRadian(90.f), 0.f, 0.f));
	m_fLifeTime = 6.5f;

	return S_OK;
}

_int CRuinBlade::Update( const _float& fDelta )
{
	if (fDelta == 0)
		return 0;

	m_fLifeTime -= fDelta;

	if (m_fLifeTime <= 0.f)
		m_bDestroy = true;

	Engine::CTransform* pTrans = (Engine::CTransform*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(0)->GetComponent(L"Com_Transform");
	MyVec3 vLook = *(MyVec3*)&pTrans->GetWorldMatrix()->m[2][0];
	D3DXVec3Normalize(&vLook, &vLook);

	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_ANGLE, &MyVec3(0.f, 0.2f, 0.f), 1);
	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_SCALE, &MyVec3(0.03f, 0.03f, 0.03f), 1);

	if (m_fLifeTime < 1.5f)
		m_pTransformCom->SetInfomation(Engine::CTransform::INFO_POS, &(vLook * 30.f * fDelta), 1);

	Engine::CGameObject::Update(fDelta);

	m_pRendererCom->AddRenderList(Engine::CRenderer::RENDER_ALPHA, this);

	
	
	return 0;
}

void CRuinBlade::Render( void )
{
	LPD3DXEFFECT	pEffect = m_pShaderCom->GetEffect();
	if (pEffect == NULL)
		return;

	SetUpShader(pEffect);
}

void CRuinBlade::SetUpShader( LPD3DXEFFECT pEffect )
{
	pEffect->AddRef();

	m_pTransformCom->SetUpShader(pEffect);

	MyMat matView, matProj;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	m_pTextureCom->SetUpShader(pEffect, 0);
	
	pEffect->Begin(NULL, 0);
	pEffect->BeginPass(1);

	m_pBufferCom->Render(); 

	pEffect->EndPass();
	pEffect->End();
	
	Engine::Safe_Release(pEffect);
}

HRESULT CRuinBlade::Ready_Component( void )
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

	m_pTextureCom = (Engine::CTexture*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Texture_RuinBlade");
	if (m_pTextureCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(Engine::CGameObject::UPDATE_NONE, L"Com_Texture", m_pTextureCom)))
		return E_FAIL;

	m_pBufferCom = (Engine::CTrailTex*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STATIC, L"Component_Buffer_RcTex");
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

void CRuinBlade::Free( void )
{
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pTextureCom);
	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pShaderCom);

	Engine::CGameObject::Free();
}

CRuinBlade* CRuinBlade::Create( LPDIRECT3DDEVICE9 pDevice )
{
	CRuinBlade* pEffect = new CRuinBlade(pDevice);

	if (FAILED(pEffect->Initialize()))
	{
		MSG_BOX("Effect Created Failed");
		Engine::Safe_Release(pEffect);
	}
	return pEffect;
}

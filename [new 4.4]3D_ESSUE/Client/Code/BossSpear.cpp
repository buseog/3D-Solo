#include "StdAfx.h"
#include "BossSpear.h"
#include "Boss.h"

CBossSpear::CBossSpear( LPDIRECT3DDEVICE9 pDevice )
: Engine::CGameObject(pDevice)
, m_pRendererCom(NULL)
, m_pTransformCom(NULL)
, m_pMtrCom(NULL)
, m_pMeshCom(NULL)
, m_pShaderCom(NULL)
, m_pMatParent(NULL)
, m_pMatAttachment(NULL)
{

}

CBossSpear::~CBossSpear( void )
{

}

HRESULT CBossSpear::Initialize( void )
{
	if (FAILED(ReadyComponent()))
		return E_FAIL;

	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_POS, &MyVec3(15.f, 0.f, 0.f));
	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_ANGLE, &MyVec3(0.f, D3DXToRadian(180.f), 0.f));

	return S_OK;
}

_int CBossSpear::Update( const _float& fDelta )
{
	if (m_pMatParent == NULL)
	{
		m_pMatParent = ((Engine::CTransform*)Engine::Get_Management()->GetLayer(L"Layer_Boss")->GetObject(0)->GetComponent(L"Com_Transform"))->GetWorldMatrix();
		m_pMatAttachment = ((Engine::CDynamicMesh*)Engine::Get_Management()->GetLayer(L"Layer_Boss")->GetObject(0)->GetComponent(L"Com_Mesh"))->GetPartsMatrix("ValveBiped_Bip01_L_Forearm");
		m_pTransformCom->SetParentMatrix(&m_MatCompute);
	}

	if (((CBoss*)Engine::Get_Management()->GetLayer(L"Layer_Boss")->GetObject(0))->GetHide() == true)
		return 0;

	m_MatCompute = *m_pMatAttachment * *m_pMatParent;

	Engine::CGameObject::Update(fDelta);

	m_pRendererCom->AddRenderList(Engine::CRenderer::RENDER_NONEALPHA, this);

	return 0;
}

void CBossSpear::Render( void )
{
	LPD3DXEFFECT	pEffect = m_pShaderCom->GetEffect();
	if (pEffect == NULL)
		return;
	SetUpShader(pEffect);
}

void CBossSpear::SetUpShader( LPD3DXEFFECT pEffect )
{
	pEffect->AddRef();

	MyMat matView, matProj;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	m_pTransformCom->SetUpShader(pEffect);
	m_pMeshCom->SetUpShader(pEffect, 0);

	Engine::Safe_Release(pEffect);
}

HRESULT CBossSpear::ReadyComponent( void )
{
	m_pRendererCom = (Engine::CRenderer*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STATIC, L"Component_Renderer");
	if (m_pRendererCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(CGameObject::UPDATE_NONE, L"Com_Renderer", m_pRendererCom)))
		return E_FAIL;

	m_pTransformCom = (Engine::CTransform*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STATIC, L"Component_Transform");
	if (m_pTransformCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(CGameObject::UPDATE_DO, L"Com_Transform", m_pTransformCom)))
		return E_FAIL;	

	m_pMtrCom = (Engine::CMaterial*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STATIC, L"Component_Material");
	if (m_pMtrCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(CGameObject::UPDATE_NONE, L"Com_Material", m_pMtrCom)))
		return E_FAIL;

	m_pMeshCom = (Engine::CStaticMesh*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Mesh_BossSpear");
	if (m_pMeshCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(CGameObject::UPDATE_NONE, L"Com_Mesh", m_pMeshCom)))
		return E_FAIL;

	m_pShaderCom = (Engine::CShader*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Shader_Mesh");
	if (m_pShaderCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(Engine::CGameObject::UPDATE_NONE, L"Com_Shader", m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CBossSpear::Free( void )
{
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pMtrCom);
	Engine::Safe_Release(m_pMeshCom);
	Engine::Safe_Release(m_pShaderCom);

	Engine::CGameObject::Free();
}

CBossSpear* CBossSpear::Create( LPDIRECT3DDEVICE9 pDevice )
{
	CBossSpear* pSHield = new CBossSpear(pDevice);

	if (FAILED(pSHield->Initialize()))
	{
		MSG_BOX("LongBlade Created Failed");
		Engine::Safe_Release(pSHield);
	}
	return pSHield;
}

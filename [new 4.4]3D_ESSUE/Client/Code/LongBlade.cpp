#include "StdAfx.h"
#include "LongBlade.h"
#include "Player.h"
#include "Damage.h"
#include "Boss.h"

CLongBlade::CLongBlade( LPDIRECT3DDEVICE9 pDevice )
: Engine::CGameObject(pDevice)
, m_pRendererCom(NULL)
, m_pTransformCom(NULL)
, m_pMtrCom(NULL)
, m_pMeshCom(NULL)
, m_pCollisionCom(NULL)
, m_pShaderCom(NULL)
, m_pMatParent(NULL)
, m_pMatAttachment(NULL)
, m_pTargetCollision(NULL)
, m_bCollision(false)
{

}

CLongBlade::~CLongBlade( void )
{

}

HRESULT CLongBlade::Initialize( void )
{
	if (FAILED(ReadyComponent()))
		return E_FAIL;

	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_POS, &MyVec3(-10.f, 0.f, 0.f));
	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_ANGLE, &MyVec3(0.f, D3DXToRadian(-90.f), 0.f));

	D3DXMatrixScaling(&m_MatBonus, 1.f, 1.f, 2.5f);
	m_pCollisionCom->SetBonus(&m_MatBonus);

	m_fCollisionTime = 0.f;

	return S_OK;
}

_int CLongBlade::Update( const _float& fDelta )
{
	if (m_pMatParent == NULL)
	{
		m_pMatParent = ((Engine::CTransform*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(0)->GetComponent(L"Com_Transform"))->GetWorldMatrix();
		m_pMatAttachment = ((Engine::CDynamicMesh*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(0)->GetComponent(L"Com_Mesh"))->GetPartsMatrix("ValveBiped_Anim_Attachment_RH");
		m_pTransformCom->SetParentMatrix(&m_MatCompute);

		m_pTargetCollision = (Engine::CCollisionBox*)Engine::Get_Management()->GetLayer(L"Layer_Boss")->GetObject(0)->GetComponent(L"Com_CollisionBox");
	}

	m_fCollisionTime -= fDelta;
	if (m_fCollisionTime <= 0.f)
		m_bCollision = false;

	m_MatCompute = *m_pMatAttachment * *m_pMatParent;

	if (m_bCollision == true)
	{
		if (m_pCollisionCom->CollisionOBB(m_pTargetCollision) == true)
		{
			CDamage* pEffect = CDamage::Create(m_pDevice, 0, m_pTargetCollision->GetObbBox()->vCenter);
			Engine::Get_Management()->GetLayer(L"Layer_Player")->AddGameObject(pEffect);

			pEffect = CDamage::Create(m_pDevice, 1, m_pTargetCollision->GetObbBox()->vCenter);
			Engine::Get_Management()->GetLayer(L"Layer_Player")->AddGameObject(pEffect);

			((CBoss*)Engine::Get_Management()->GetLayer(L"Layer_Boss")->GetObject(0))->SetDamage(50.f);

			m_bCollision = false;
		}
	}
	else
		m_pCollisionCom->SetCollision(false);

	Engine::CGameObject::Update(fDelta);

	m_pRendererCom->AddRenderList(Engine::CRenderer::RENDER_NONEALPHA, this);

	return 0;
}

void CLongBlade::Render( void )
{
	LPD3DXEFFECT	pEffect = m_pShaderCom->GetEffect();
	if (pEffect == NULL)
		return;
	SetUpShader(pEffect);

	m_pCollisionCom->Render(Engine::CCollisionBox::COLL_OBB, m_pTransformCom->GetWorldMatrix());
}

void CLongBlade::SetUpShader( LPD3DXEFFECT pEffect )
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

void CLongBlade::SetCollision( const _uint& iIndex /*= 0*/ )
{
	m_bCollision = true;
	m_fCollisionTime = 0.5f;

	SetColliderSize(iIndex);
}


void CLongBlade::SetColliderSize( const _uint& iIndex )
{
	if (iIndex == 0)
	{
		D3DXMatrixScaling(&m_MatBonus, 2.f, 1.f, 3.f);
		m_pCollisionCom->SetBonus(&m_MatBonus);
	}
	else if (iIndex == 1)
	{
		D3DXMatrixScaling(&m_MatBonus, 3.f, 3.f, 7.f);
		m_pCollisionCom->SetBonus(&m_MatBonus);
	}
}

HRESULT CLongBlade::ReadyComponent( void )
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

	m_pMeshCom = (Engine::CDynamicMesh*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Mesh_LongBlade");
	if (m_pMeshCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(CGameObject::UPDATE_NONE, L"Com_Mesh", m_pMeshCom)))
		return E_FAIL;

	m_pCollisionCom = (Engine::CCollisionBox*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STATIC, L"Component_CollisionBox");
	if (m_pCollisionCom == NULL)
		return E_FAIL;
	if (FAILED(m_pCollisionCom->ComputeBox(m_pMeshCom, 0)))
		return E_FAIL;
	if (FAILED(AddComponent(CGameObject::UPDATE_NONE, L"Com_CollisionBox", m_pCollisionCom)))
		return E_FAIL;

	m_pShaderCom = (Engine::CShader*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Shader_Mesh");
	if (m_pShaderCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(Engine::CGameObject::UPDATE_NONE, L"Com_Shader", m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CLongBlade::Free( void )
{
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pMtrCom);
	Engine::Safe_Release(m_pMeshCom);
	Engine::Safe_Release(m_pCollisionCom);
	Engine::Safe_Release(m_pShaderCom);

	Engine::CGameObject::Free();
}

CLongBlade* CLongBlade::Create( LPDIRECT3DDEVICE9 pDevice )
{
	CLongBlade* pBlade = new CLongBlade(pDevice);

	if (FAILED(pBlade->Initialize()))
	{
		MSG_BOX("LongBlade Created Failed");
		Engine::Safe_Release(pBlade);
	}
	return pBlade;
}
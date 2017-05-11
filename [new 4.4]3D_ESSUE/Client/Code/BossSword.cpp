#include "StdAfx.h"
#include "BossSword.h"
#include "Boss.h"
#include "Damage.h"
#include "Player.h"
#include "SoundMgr.h"

CBossSword::CBossSword( LPDIRECT3DDEVICE9 pDevice )
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
, m_bAttachMent(false)
, m_bCollision(false)
{

}

CBossSword::~CBossSword( void )
{

}

HRESULT CBossSword::Initialize( void )
{
	if (FAILED(ReadyComponent()))
		return E_FAIL;

	m_pMatParent = ((Engine::CTransform*)Engine::Get_Management()->GetLayer(L"Layer_Boss")->GetObject(0)->GetComponent(L"Com_Transform"))->GetWorldMatrix();
	m_pMatAttachment = ((Engine::CDynamicMesh*)Engine::Get_Management()->GetLayer(L"Layer_Boss")->GetObject(0)->GetComponent(L"Com_Mesh"))->GetPartsMatrix("ValveBiped_Anim_Attachment_RH");
	m_pTargetCollision = (Engine::CCollisionBox*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(0)->GetComponent(L"Com_CollisionBox");

	D3DXMatrixScaling(&m_MatBonus, 1.f, 1.f, 2.5f);
	m_pCollisionCom->SetBonus(&m_MatBonus);

	m_fCollisionTime = 0.f;

	MyVec3 vPos = *(MyVec3*)&m_pMatParent->m[3][0];
	MyVec3 vLook = *(MyVec3*)&m_pMatParent->m[2][0];
	D3DXVec3Normalize(&vLook, &vLook);
	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_POS, &MyVec3(vPos + MyVec3(2.5f, 80.f, 0.f) + vLook * 70.f));
	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_SCALE, &Engine::MyVec3(0.1f, 0.1f, 0.1f));
	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_ANGLE, &MyVec3(D3DXToRadian(-90.f), D3DXToRadian(90.f), 0.f));

	/*m_pTransformCom->SetInfomation(Engine::CTransform::INFO_POS, &MyVec3(15.f, 0.f, 0.f));
	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_ANGLE, &MyVec3(0.f, D3DXToRadian(-90.f), 0.f));*/

	return S_OK;
}

_int CBossSword::Update( const _float& fDelta )
{
	m_MatCompute = *m_pMatAttachment * *m_pMatParent;

	if (((CBoss*)Engine::Get_Management()->GetLayer(L"Layer_Boss")->GetObject(0))->GetHide() == true)
		return 0;

	if (m_bAttachMent == false)
	{
		if (m_pTransformCom->GetInfomation(Engine::CTransform::INFO_POS)->y >= 17.f)
			m_pTransformCom->SetInfomation(Engine::CTransform::INFO_POS, &MyVec3(0.f, 250.f * -fDelta, 0.f), 1);
	}

	m_fCollisionTime -= fDelta;
	if (m_fCollisionTime <= 0.f)
		m_bCollision = false;

	if (m_bCollision == true)
	{
		if (m_pCollisionCom->CollisionOBB(m_pTargetCollision) == true &&
			((CPlayer*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(0))->GetState() != CPlayer::DamageFront)
		{
			CDamage* pEffect = CDamage::Create(m_pDevice, 0, m_pTargetCollision->GetObbBox()->vCenter);
			Engine::Get_Management()->GetLayer(L"Layer_Player")->AddGameObject(pEffect);

			pEffect = CDamage::Create(m_pDevice, 1, m_pTargetCollision->GetObbBox()->vCenter);
			Engine::Get_Management()->GetLayer(L"Layer_Player")->AddGameObject(pEffect);

			((CPlayer*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(0))->SetDamage(50.f);

			m_bCollision = false;
		}
	}
	else
		m_pCollisionCom->SetCollision(false);

	Engine::CGameObject::Update(fDelta);

	m_pRendererCom->AddRenderList(Engine::CRenderer::RENDER_NONEALPHA, this);

	return 0;
}

void CBossSword::Render( void )
{
	LPD3DXEFFECT	pEffect = m_pShaderCom->GetEffect();
	if (pEffect == NULL)
		return;
	SetUpShader(pEffect);

	m_pCollisionCom->Render(Engine::CCollisionBox::COLL_OBB, m_pTransformCom->GetWorldMatrix());
}

void CBossSword::SetUpShader( LPD3DXEFFECT pEffect )
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

void CBossSword::SetAttach( _bool bTF )
{
	if (bTF == true)
	{
		m_pTransformCom->SetInfomation(Engine::CTransform::INFO_POS, &MyVec3(15.f, 0.f, 0.f));
		m_pTransformCom->SetInfomation(Engine::CTransform::INFO_ANGLE, &MyVec3(0.f, D3DXToRadian(-90.f), 0.f));
		m_pTransformCom->SetInfomation(Engine::CTransform::INFO_SCALE, &Engine::MyVec3(1.f, 1.f, 1.f));
		m_pTransformCom->SetParentMatrix(&m_MatCompute);
	}
	else
	{
		m_pTransformCom->SetInfomation(Engine::CTransform::INFO_POS, (MyVec3*)&m_MatCompute.m[3][0]);
		m_pTransformCom->SetInfomation(Engine::CTransform::INFO_SCALE, &Engine::MyVec3(0.1f, 0.1f, 0.1f));
		m_pTransformCom->SetInfomation(Engine::CTransform::INFO_ANGLE, &MyVec3(D3DXToRadian(-90.f), D3DXToRadian(90.f), 0.f));
		m_pTransformCom->SetParentMatrix(NULL);
	}
	m_bAttachMent = bTF;
}

void CBossSword::SetCollision( const _uint& iIndex /*= 0*/ )
{
	m_bCollision = true;
	m_fCollisionTime = 0.5f;

	if (iIndex == 0)
	{
		D3DXMatrixScaling(&m_MatBonus, 1.f, 1.f, 3.f);
		m_pCollisionCom->SetBonus(&m_MatBonus);
	}
	else if (iIndex == 1)
	{
		D3DXMatrixScaling(&m_MatBonus, 1.f, 1.f, 7.f);
		m_pCollisionCom->SetBonus(&m_MatBonus);
	}
	else if (iIndex == 2)
	{
		D3DXMatrixScaling(&m_MatBonus, 3.f, 1.f, 3.f);
		m_pCollisionCom->SetBonus(&m_MatBonus);
	}
}

HRESULT CBossSword::ReadyComponent( void )
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

	m_pMeshCom = (Engine::CDynamicMesh*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Mesh_BossSword");
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

void CBossSword::Free( void )
{
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pMtrCom);
	Engine::Safe_Release(m_pMeshCom);
	Engine::Safe_Release(m_pCollisionCom);
	Engine::Safe_Release(m_pShaderCom);

	Engine::CGameObject::Free();
}

CBossSword* CBossSword::Create( LPDIRECT3DDEVICE9 pDevice )
{
	CBossSword* pSword = new CBossSword(pDevice);

	if (FAILED(pSword->Initialize()))
	{
		MSG_BOX("LongBlade Created Failed");
		Engine::Safe_Release(pSword);
	}
	return pSword;
}

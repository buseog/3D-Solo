#include "StdAfx.h"
#include "Boss.h"

CBoss::CBoss( LPDIRECT3DDEVICE9 pDevice )
: Engine::CGameObject(pDevice)
, m_pRendererCom(NULL)
, m_pTransformCom(NULL)
, m_pMtrCom(NULL)
, m_pMeshCom(NULL)
, m_pCollisionCom(NULL)
, m_iStateID(1)
, m_pMatPlayer(NULL)
, m_pMatBone(NULL)
{

}

CBoss::~CBoss( void )
{

}

HRESULT CBoss::Initialize( void )
{
	if (FAILED(ReadyComponent()))
		return E_FAIL;

	m_pMatBone = m_pMeshCom->GetPartsMatrix("ValveBiped_Bip01");
	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_SCALE, &Engine::MyVec3(0.1f, 0.1f, 0.1f));
	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_POS, &Engine::MyVec3(0.f, 5.f, 0.f));
	m_pMeshCom->SetAnimation(Idle);

	m_fSpeed = 15.f;

	return S_OK;
}

_int CBoss::Update( const _float& fDelta )
{
	if (m_pMatPlayer == NULL)
		m_pMatPlayer = ((Engine::CTransform*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(0)->GetComponent(L"Com_Transform"))->GetWorldMatrix();

	if (GetAsyncKeyState(VK_ESCAPE))
	{
		_float fTemp = fDelta * 0.5f;

		if (GetAsyncKeyState(VK_F6) & 0x8000)
		{
			m_iStateID = Idle;
			m_pMeshCom->SetAnimation(Idle);
		}

		Engine::CGameObject::Update(fTemp);

		AnimationCheck();
		m_pMeshCom->PlayAnimation(fTemp);

		m_pRendererCom->AddRenderList(Engine::CRenderer::RENDER_NONEALPHA, this);

		return 0;
	}

	if (GetAsyncKeyState(VK_F7) & 0x8000)
	{
		m_iStateID = 2;
		m_pMeshCom->SetAnimation(m_iStateID);
	}

	if (GetAsyncKeyState(VK_F6) & 0x8000)
	{
		m_iStateID = Idle;
		m_pMeshCom->SetAnimation(m_iStateID);
	}


	Engine::CGameObject::Update(fDelta);

	m_pMeshCom->PlayAnimation(fDelta);
	AnimationCheck();
	
	

	m_pRendererCom->AddRenderList(Engine::CRenderer::RENDER_NONEALPHA, this);

	return 0;
}

_int CBoss::LastUpdate( const _float& fDelta )
{
	
	return 0;	
}

void CBoss::Render( void )
{
	m_pTransformCom->SetUpDevice();
	m_pMtrCom->SetMaterial();
	m_pMeshCom->Render();
	m_pCollisionCom->Render(Engine::CCollisionBox::COLL_AABB, m_pTransformCom->GetWorldMatrix());
}

void CBoss::AnimationCheck( void )
{
	if (m_pMeshCom->CheckAnimation() == true)
	{
		m_iStateID = Idle;
		m_pMeshCom->SetAnimation(m_iStateID);
	}
}

void CBoss::Pattern( const _float& fDelta )
{
	MyVec3 vTarget = *(MyVec3*)&m_pMatPlayer->m[3][0];
	MyVec3 vPos = *(MyVec3*)&m_pTransformCom->GetWorldMatrix()->m[3][0];
	MyVec3 vMove = vTarget - vPos;
	vPos += *D3DXVec3Normalize(&vMove, &vMove) * m_fSpeed * fDelta;

	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_POS, &vPos);
	m_pMeshCom->SetAnimation(StepFront);
}

HRESULT CBoss::ReadyComponent( void )
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

	m_pMeshCom = (Engine::CDynamicMesh*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Mesh_Boss");
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

	return S_OK;
}

void CBoss::Free( void )
{
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pMtrCom);
	Engine::Safe_Release(m_pMeshCom);
	Engine::Safe_Release(m_pCollisionCom);

	Engine::CGameObject::Free();
}

CBoss* CBoss::Create( LPDIRECT3DDEVICE9 pDevice )
{
	CBoss* pPlayer = new CBoss(pDevice);

	if (FAILED(pPlayer->Initialize()))
	{
		MSG_BOX("Player Created Failed");
		Engine::Safe_Release(pPlayer);
	}
	return pPlayer;
}
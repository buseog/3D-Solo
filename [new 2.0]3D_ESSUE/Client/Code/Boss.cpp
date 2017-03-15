#include "StdAfx.h"
#include "Boss.h"

CBoss::CBoss( LPDIRECT3DDEVICE9 pDevice )
: Engine::CGameObject(pDevice)
, m_pRendererCom(NULL)
, m_pTransformCom(NULL)
, m_pMtrCom(NULL)
, m_pMeshCom(NULL)
, m_pCollisionCom(NULL)
, m_iStateID(0)
, m_iAttackCnt(Attack_01)
{

}

CBoss::~CBoss( void )
{

}

HRESULT CBoss::Initialize( void )
{
	if (FAILED(ReadyComponent()))
		return E_FAIL;

	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_SCALE, &Engine::MyVec3(0.05f, 0.05f, 0.05f));
	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_POS, &Engine::MyVec3(0.f, 5.f, 0.f));
	m_pMeshCom->SetAnimation(0);

	m_fSpeed = 10.f;

	return S_OK;
}

_int CBoss::Update( const _float& fDelta )
{
	Engine::CGameObject::Update(fDelta);

	AnimationCheck();
	m_pMeshCom->PlayAnimation(fDelta);

	m_pRendererCom->AddRenderList(Engine::CRenderer::RENDER_NONEALPHA, this);

	return 0;
}

void CBoss::Render( void )
{
	m_pTransformCom->SetTransform();
	m_pMtrCom->SetMaterial();
	m_pMeshCom->Render();
	m_pCollisionCom->Render(Engine::CCollisionBox::COLL_AABB, false, m_pTransformCom->GetWorldMatrix());
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

void CBoss::AnimationCheck( void )
{
	if (m_pMeshCom->EndAnimation() == true)
	{
		m_pMeshCom->SetAnimation(0);
		m_iStateID = ST_NORMAL;
	}
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
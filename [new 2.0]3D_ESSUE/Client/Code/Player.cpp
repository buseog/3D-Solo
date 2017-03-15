#include "StdAfx.h"
#include "Player.h"


CPlayer::CPlayer( LPDIRECT3DDEVICE9 pDevice )
: Engine::CGameObject(pDevice)
, m_pKeyMgr(Engine::Get_KeyMgr())
, m_pRendererCom(NULL)
, m_pTransformCom(NULL)
, m_pMtrCom(NULL)
, m_pMeshCom(NULL)
, m_pCollisionCom(NULL)
, m_iStateID(0)
, m_iAttackCnt(Attack_01)
{

}

CPlayer::~CPlayer( void )
{

}

HRESULT CPlayer::Initialize( void )
{
	if (FAILED(ReadyComponent()))
		return E_FAIL;

	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_SCALE, &Engine::MyVec3(0.1f, 0.1f, 0.1f));
	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_POS, &Engine::MyVec3(0.f, 5.f, 0.f));
	m_pMeshCom->SetAnimation(0);

	m_fSpeed = 10.f;

	return S_OK;
}

_int CPlayer::Update( const _float& fDelta )
{
	KeyCheck(fDelta);

	Engine::CGameObject::Update(fDelta);

	m_pMeshCom->PlayAnimation(fDelta);
	AnimationCheck();

	m_pRendererCom->AddRenderList(Engine::CRenderer::RENDER_NONEALPHA, this);

	return 0;
}

void CPlayer::Render( void )
{
	m_pTransformCom->SetTransform();
	m_pMtrCom->SetMaterial();
	m_pMeshCom->Render();
	m_pCollisionCom->Render(Engine::CCollisionBox::COLL_AABB, false, m_pTransformCom->GetWorldMatrix());
}

HRESULT CPlayer::ReadyComponent( void )
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

	m_pMeshCom = (Engine::CDynamicMesh*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Mesh_Player");
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

void CPlayer::KeyCheck( const _float& fDelta )
{
	if (fDelta == 0.f)
		return;

	if (m_pKeyMgr->KeyDown(VK_LBUTTON))
	{
		switch (m_iAttackCnt)
		{
		case Attack_01:
			m_pMeshCom->SetAnimation(m_iAttackCnt);
			m_iAttackCnt = Attack_02;
			m_iStateID = ST_ATTACK;
			break;
		case Attack_02:
			m_pMeshCom->SetAnimation(m_iAttackCnt);
			m_iAttackCnt = Attack_03;
			m_iStateID = ST_ATTACK;
			break;
		case Attack_03:
			m_pMeshCom->SetAnimation(m_iAttackCnt);
			m_iAttackCnt = Attack_01;
			m_iStateID = ST_ATTACK;
			break;
		}
	}

	if (m_pKeyMgr->KeyDown('1') && m_iStateID != ST_ATTACK)
	{
		m_iStateID = LordOfMana;
		m_pMeshCom->SetAnimation(m_iStateID);
	}

	if (m_pKeyMgr->StayKeyDown('W') && m_iStateID != ST_ATTACK)
	{
		m_pTransformCom->Move(m_fSpeed * fDelta);
		m_pMeshCom->SetAnimation(1);
		m_iStateID = Run_Lower;
	}
	else if (m_pKeyMgr->StayKeyDown('S') && m_iStateID != ST_ATTACK)
	{
		m_pTransformCom->Move(-m_fSpeed * fDelta);
		m_pMeshCom->SetAnimation(1);
		m_iStateID = Run_Lower;
	}
	else if (m_pKeyMgr->StayKeyDown('A') && m_iStateID != ST_ATTACK)
	{
		m_pTransformCom->SideMove(-m_fSpeed * fDelta);
		m_pTransformCom->SetRotation(Engine::CTransform::ANGLE_Y, D3DXToRadian(-90.f));
		m_pMeshCom->SetAnimation(1);
		m_iStateID = Run_Lower;
	}
	else if (m_pKeyMgr->StayKeyDown('D') && m_iStateID != ST_ATTACK)
	{
		m_pTransformCom->SideMove(m_fSpeed * fDelta);
		m_pTransformCom->SetRotation(Engine::CTransform::ANGLE_Y, D3DXToRadian(90.f));
		m_pMeshCom->SetAnimation(1);
		m_iStateID = Run_Lower;
	}
	else
		m_iStateID = Battle_Idle;
}

void CPlayer::AnimationCheck( void )
{
	if (m_pMeshCom->EndAnimation() == true)
	{
		m_pMeshCom->SetAnimation(m_iStateID);
	}
}

void CPlayer::Free( void )
{
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pMtrCom);
	Engine::Safe_Release(m_pMeshCom);
	Engine::Safe_Release(m_pCollisionCom);

	Engine::CGameObject::Free();
}

CPlayer* CPlayer::Create( LPDIRECT3DDEVICE9 pDevice )
{
	CPlayer* pPlayer = new CPlayer(pDevice);

	if (FAILED(pPlayer->Initialize()))
	{
		MSG_BOX("Player Created Failed");
		Engine::Safe_Release(pPlayer);
	}
	return pPlayer;
}
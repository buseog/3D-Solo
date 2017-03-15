#include "StdAfx.h"
#include "LongBlade.h"
#include "Player.h"

CLongBlade::CLongBlade( LPDIRECT3DDEVICE9 pDevice )
: Engine::CGameObject(pDevice)
, m_pRendererCom(NULL)
, m_pTransformCom(NULL)
, m_pMtrCom(NULL)
, m_pMeshCom(NULL)
, m_pCollisionCom(NULL)
, m_pPlayer(NULL)
, m_pMatParent(NULL)
, m_pMatAttachment(NULL)
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

	return S_OK;
}

_int CLongBlade::Update( const _float& fDelta )
{
	if (m_pMatParent == NULL)
	{
		m_pMatParent = ((Engine::CTransform*)m_pPlayer->GetComponent(L"Com_Transform"))->GetWorldMatrix();
		m_pMatAttachment = ((Engine::CDynamicMesh*)m_pPlayer->GetComponent(L"Com_Mesh"))->GetPartsMatrix("ValveBiped_Anim_Attachment_RH");
	}

	m_MatCompute = *m_pMatAttachment * *m_pMatParent;

	m_pTransformCom->SetParentMatrix(&m_MatCompute);

	Engine::CGameObject::Update(fDelta);

	m_pRendererCom->AddRenderList(Engine::CRenderer::RENDER_NONEALPHA, this);

	Engine::CCollisionBox* pBox = (Engine::CCollisionBox*)m_pPlayer->GetComponent(L"Com_CollisionBox");

	m_pCollisionCom->CollisionOBB(pBox);

	return 0;
}

void CLongBlade::Render( void )
{
	m_pTransformCom->SetTransform();
	m_pMtrCom->SetMaterial();
	m_pMeshCom->Render();
	m_pCollisionCom->Render(Engine::CCollisionBox::COLL_OBB, false, m_pTransformCom->GetWorldMatrix());
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

	m_pMeshCom = (Engine::CStaticMesh*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Mesh_LongBlade");
	if (m_pMeshCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(CGameObject::UPDATE_NONE, L"Com_Mesh", m_pMeshCom)))
		return E_FAIL;

	m_pCollisionCom = (Engine::CCollisionBox*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STATIC, L"Component_CollisionBox");
	if (m_pCollisionCom == NULL)
		return E_FAIL;
	if (FAILED(m_pCollisionCom->ComputeBox(m_pMeshCom)))
		return E_FAIL;
	if (FAILED(AddComponent(CGameObject::UPDATE_NONE, L"Com_CollisionBox", m_pCollisionCom)))
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

void CLongBlade::SetPlayer( CPlayer* pPlayer )
{
	m_pPlayer = pPlayer;
}

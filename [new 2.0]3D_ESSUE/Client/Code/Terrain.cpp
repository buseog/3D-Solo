#include "StdAfx.h"
#include "Terrain.h"

CTerrain::CTerrain( LPDIRECT3DDEVICE9 pDevice )
: CGameObject(pDevice)
, m_pRendererCom(NULL)
, m_pTransformCom(NULL)
, m_pMtrCom(NULL)
, m_pMeshCom(NULL)
{

}

CTerrain::~CTerrain( void )
{

}

HRESULT CTerrain::Initialize( void )
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_SCALE, &Engine::MyVec3(0.1f, 0.1f, 0.1f));

	return S_OK;
}

_int CTerrain::Update( const _float& fDelta )
{
	Engine::CGameObject::Update(fDelta);

	m_pRendererCom->AddRenderList(Engine::CRenderer::RENDER_NONEALPHA, this);

	return 0;
}

void CTerrain::Render( void )
{
	m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pTransformCom->SetTransform();
	m_pMtrCom->SetMaterial();
	m_pMeshCom->Render();

	m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}

HRESULT CTerrain::Ready_Component( void )
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

	m_pMtrCom = (Engine::CMaterial*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STATIC, L"Component_Material");
	if (m_pMtrCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(Engine::CGameObject::UPDATE_NONE, L"Com_Material", m_pMtrCom)))
		return E_FAIL;

	m_pMeshCom = (Engine::CStaticMesh*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Mesh_BossMap");
	if (m_pMeshCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(Engine::CGameObject::UPDATE_NONE, L"Com_Mesh", m_pMeshCom)))
		return E_FAIL;

	return S_OK;
}

void CTerrain::Free( void )
{
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pMtrCom);
	Engine::Safe_Release(m_pMeshCom);

	Engine::CGameObject::Free();
}

CTerrain* CTerrain::Create( LPDIRECT3DDEVICE9 pDevice )
{
	CTerrain* pTerrain = new CTerrain(pDevice);

	if (FAILED(pTerrain->Initialize()))
	{
		MSG_BOX("Terrain Created Failed");
		::Safe_Release(pTerrain);
	}

	return pTerrain;
}

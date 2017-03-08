#include "StdAfx.h"
#include "Obstacles.h"

CObstacles::CObstacles(LPDIRECT3DDEVICE9 pDevice)
: CLandObject(pDevice)
{

}

CObstacles::~CObstacles(void)
{

}

HRESULT CObstacles::Initialize(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	return S_OK;
}

_int CObstacles::Update(const _float& fDelta)
{
	Engine::CGameObject::Update(fDelta);

	m_pRendererCom->AddRenderList(Engine::CRenderer::RENDER_NONEALPHA, this);

	return 0;
}

void CObstacles::Render(void)
{
	m_pTransformCom->SetWorld();
	m_pMtrCom->SetMaterial();
	m_pMeshCom->Render();
}

HRESULT CObstacles::Ready_Component(void)
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

	m_pMeshCom = (Engine::CStaticMesh*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Mesh_Stone");
	if (m_pMeshCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(CGameObject::UPDATE_NONE, L"Com_Mesh", m_pMeshCom)))
		return E_FAIL;


	return S_OK;
}

void CObstacles::Free(void)
{
	CLandObject::Free();
}

CObstacles* CObstacles::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CObstacles* pObstacles = new CObstacles(pDevice);

	if (FAILED(pObstacles->Initialize()))
	{
		MSG_BOX("Obstacles Created Failed");
		Engine::Safe_Release(pObstacles);
	}

	return pObstacles;
}

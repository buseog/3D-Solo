#include "StdAfx.h"
#include "Obstacle.h"

CObstacle::CObstacle(LPDIRECT3DDEVICE9 pDevice)
: Engine::CGameObject(pDevice)
, m_pRendererCom(NULL)
, m_pTransformCom(NULL)
, m_pMtrCom(NULL)
, m_pMeshCom(NULL)
{
	ZeroMemory(szKey, sizeof(_tchar) * 256);
}

CObstacle::~CObstacle(void)
{

}

HRESULT CObstacle::Initialize(const _tchar* pName)
{
	if (FAILED(Ready_Component(pName)))
		return E_FAIL;

	lstrcpy(szKey, pName);

	return S_OK;
}

_int CObstacle::Update(const _float& fDelta)
{
	Engine::CGameObject::Update(fDelta);

	m_pRendererCom->AddRenderList(Engine::CRenderer::RENDER_NONEALPHA, this);

	return 0;
}

void CObstacle::Render(void)
{
	//m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pTransformCom->SetUpDevice();
	m_pMtrCom->SetMaterial();
	m_pMeshCom->Render();

	//m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}

HRESULT CObstacle::Ready_Component(const _tchar* pName)
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

	m_pMeshCom = (Engine::CStaticMesh*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STATIC, pName);
	if (m_pMeshCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(CGameObject::UPDATE_NONE, L"Com_Mesh", m_pMeshCom)))
		return E_FAIL;


	return S_OK;
}

void CObstacle::Free(void)
{
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pMtrCom);
	Engine::Safe_Release(m_pMeshCom);

	Engine::CGameObject::Free();
}

CObstacle* CObstacle::Create(LPDIRECT3DDEVICE9 pDevice, const _tchar* pName)
{
	CObstacle* pObstacles = new CObstacle(pDevice);

	if (FAILED(pObstacles->Initialize(pName)))
	{
		MSG_BOX("Obstacles Created Failed");
		Engine::Safe_Release(pObstacles);
	}

	return pObstacles;
}

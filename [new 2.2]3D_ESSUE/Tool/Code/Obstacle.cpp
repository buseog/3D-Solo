#include "StdAfx.h"
#include "Obstacle.h"

CObstacle::CObstacle(LPDIRECT3DDEVICE9 pDevice)
: Engine::CGameObject(pDevice)
, m_pTransformCom(NULL)
, m_pMeshCom(NULL)
{
	ZeroMemory(szKey, sizeof(_tchar) * 256);
}

CObstacle::~CObstacle(void)
{

}

HRESULT CObstacle::Initialize(const _tchar* pNameKey, MyVec3 vPos)
{
	m_pTransformCom = (Engine::CTransform*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STATIC, L"Component_Transform");
	if (m_pTransformCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(CGameObject::UPDATE_DO, L"Com_Transform", m_pTransformCom)))
		return E_FAIL;	

	m_pMeshCom = (Engine::CStaticMesh*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STATIC, pNameKey);
	if (m_pMeshCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(CGameObject::UPDATE_NONE, L"Com_Mesh", m_pMeshCom)))
		return E_FAIL;

	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_POS, &vPos);
	m_pTransformCom->Update();

	return S_OK;
}

_int CObstacle::Update(const _float& fDelta)
{
	Engine::CGameObject::Update(fDelta);

	return 0;
}

void CObstacle::Render(void)
{
	m_pTransformCom->SetUpDevice();
	m_pMeshCom->Render();
}

void CObstacle::Free(void)
{
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pMeshCom);

	Engine::CGameObject::Free();
}

CObstacle* CObstacle::Create(LPDIRECT3DDEVICE9 pDevice, const _tchar* pNameKey, MyVec3 vPos)
{
	CObstacle* pObstacle = new CObstacle(pDevice);

	if (FAILED(pObstacle->Initialize(pNameKey, vPos)))
	{
		MSG_BOX("Obstacle Created Failed");
		Engine::Safe_Release(pObstacle);
	}

	return pObstacle;
}

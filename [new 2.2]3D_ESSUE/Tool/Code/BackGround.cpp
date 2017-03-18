#include "StdAfx.h"
#include "BackGround.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CBackGround::CBackGround( LPDIRECT3DDEVICE9 pDevice )
: CGameObject(pDevice)
, m_pTransformCom(NULL)
, m_pMeshCom(NULL)
{

}

CBackGround::~CBackGround( void )
{

}

HRESULT CBackGround::Initialize( void )
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_SCALE, &Engine::MyVec3(0.1f, 0.1f, 0.1f));
	m_pTransformCom->Update();

	return S_OK;
}

_int CBackGround::Update( const _float& fDelta )
{
	Engine::CGameObject::Update(fDelta);

	return 0;
}

void CBackGround::Render( void )
{
	m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pTransformCom->SetUpDevice();
	m_pMeshCom->Render();
	m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}

HRESULT CBackGround::Ready_Component( void )
{
	m_pTransformCom = (Engine::CTransform*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STATIC, L"Component_Transform");
	if (m_pTransformCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(Engine::CGameObject::UPDATE_DO, L"Com_Transform", m_pTransformCom)))
		return E_FAIL;

	m_pMeshCom = (Engine::CStaticMesh*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STATIC, L"Component_Mesh_BossMap");
	if (m_pMeshCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(Engine::CGameObject::UPDATE_NONE, L"Com_Mesh", m_pMeshCom)))
		return E_FAIL;

	return S_OK;
}

void CBackGround::Free( void )
{
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pMeshCom);

	Engine::CGameObject::Free();
}

CBackGround* CBackGround::Create( LPDIRECT3DDEVICE9 pDevice )
{
	CBackGround* pBackGround = new CBackGround(pDevice);

	if (FAILED(pBackGround->Initialize()))
	{
		MSG_BOX("Terrain Created Failed");
		Engine::Safe_Release(pBackGround);
	}

	return pBackGround;
}

void CBackGround::AddObstacle(CObstacle* pObstacle)
{
	m_ObstacleList.push_back(pObstacle);
}

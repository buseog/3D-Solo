#include "StdAfx.h"
#include "Obstacle.h"

CObstacle::CObstacle(LPDIRECT3DDEVICE9 pDevice)
: Engine::CGameObject(pDevice)
, m_pRendererCom(NULL)
, m_pTransformCom(NULL)
, m_pMtrCom(NULL)
, m_pMeshCom(NULL)
, m_pShaderCom(NULL)
, m_pKey(NULL)
{
	
}

CObstacle::~CObstacle(void)
{

}

HRESULT CObstacle::Initialize(const _tchar* pName)
{
	if (FAILED(Ready_Component(pName)))
		return E_FAIL;

	m_pKey = pName;

	return S_OK;
}

_int CObstacle::Update(const _float& fDelta)
{
	Engine::CGameObject::Update(fDelta);


	if (lstrcmp(m_pKey, L"Aurora") == false)
		m_pRendererCom->AddRenderList(Engine::CRenderer::RENDER_ALPHA, this);
	else
		m_pRendererCom->AddRenderList(Engine::CRenderer::RENDER_NONEALPHA, this);
	

	return 0;
}

void CObstacle::Render(void)
{
	LPD3DXEFFECT	pEffect = m_pShaderCom->GetEffect();
	if (pEffect == NULL)
		return;

	SetUpShader(pEffect);
}

void CObstacle::SetUpShader( LPD3DXEFFECT pEffect )
{
	pEffect->AddRef();

	D3DLIGHT9	LightInfo;
	ZeroMemory(&LightInfo, sizeof(D3DLIGHT9));
	m_pDevice->GetLight(0, &LightInfo);

	pEffect->SetVector("g_vLightDir", (MyVec4*)&LightInfo.Direction);
	pEffect->SetVector("g_vLightDiffuse", (MyVec4*)&LightInfo.Diffuse);
	pEffect->SetVector("g_vLightAmbient", (MyVec4*)&LightInfo.Ambient);
	pEffect->SetVector("g_vLightSpecular", (MyVec4*)&LightInfo.Specular);
	pEffect->SetVector("g_vCamPos", (MyVec4*)Engine::Get_CameraMgr()->GetCameraEye());

	MyMat matView, matProj;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	m_pTransformCom->SetUpShader(pEffect);
	m_pMtrCom->SetUpShader(pEffect);
	if (lstrcmp(m_pKey, L"Aurora") == false)
		m_pMeshCom->SetUpShader(pEffect, 4);
	else if (lstrcmp(m_pKey, L"vol_light") == false)
		m_pMeshCom->SetUpShader(pEffect, 2);
	else
		m_pMeshCom->SetUpShader(pEffect);

	Engine::Safe_Release(pEffect);
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

	m_pMeshCom = (Engine::CStaticMesh*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, pName);
	if (m_pMeshCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(CGameObject::UPDATE_NONE, L"Com_Mesh", m_pMeshCom)))
		return E_FAIL;

	m_pShaderCom = (Engine::CShader*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Shader_Mesh");
	if (m_pShaderCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(Engine::CGameObject::UPDATE_NONE, L"Com_Shader", m_pShaderCom)))
		return E_FAIL;


	return S_OK;
}

void CObstacle::Free(void)
{
	Engine::Safe_DeleteArray(m_pKey);
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pMtrCom);
	Engine::Safe_Release(m_pMeshCom);
	Engine::Safe_Release(m_pShaderCom);

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

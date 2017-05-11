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
, m_fOffset(0.f)
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
	_float fTime = fDelta;

	if (fTime == 0.f)
		fTime = Engine::Get_TimeMgr()->GetTimeDelta(L"Timer_60");

	Engine::CGameObject::Update(fTime);


	if (lstrcmp(m_pKey, L"Aurora") == false)
	{
		m_fOffset += 0.03f * fTime;
		m_pRendererCom->AddRenderList(Engine::CRenderer::RENDER_ALPHA, this);
	}
	else if (lstrcmp(m_pKey, L"MagicBroken") == false)
		m_pRendererCom->AddRenderList(Engine::CRenderer::RENDER_NONEALPHA, this);

	else if (lstrcmp(m_pKey, L"Pillar_a") == false || lstrcmp(m_pKey, L"Pillar_b") == false || lstrcmp(m_pKey, L"Pillar_c") == false)
		m_pRendererCom->AddRenderList(Engine::CRenderer::RENDER_ALPHA, this);
	
	else if (lstrcmp(m_pKey, L"Pillar_a_haze") == false || lstrcmp(m_pKey, L"Pillar_b_haze") == false || lstrcmp(m_pKey, L"Pillar_c_haze") == false)
	{
		m_fOffset += -0.05f * fTime;
		m_pRendererCom->AddRenderList(Engine::CRenderer::RENDER_ALPHA, this);
	}
	else 
		m_pRendererCom->AddRenderList(Engine::CRenderer::RENDER_ALPHA, this);
	

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

	MyVec3 vPlayerPos = *((Engine::CTransform*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(0)->GetComponent(L"Com_Transform"))->GetInfomation(Engine::CTransform::INFO_POS);

	MyMat matLightView;
	MyVec3 vPos = MyVec3(80.f, 120.f, 120.f);
	MyVec3 vDir = vPlayerPos;
	D3DXMatrixLookAtLH(&matLightView, &vPos, &vDir, &MyVec3(0.f, 1.f, 0.f));
	pEffect->SetMatrix("g_matLight", &matLightView);

	MyMat matView, matProj;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);
	pEffect->SetFloat("g_fOffset", m_fOffset);
	pEffect->SetVector("g_vColor", &MyVec4(0.1f, 0.3f, 0.3f, 0.1f));

	m_pTransformCom->SetUpShader(pEffect);
	Engine::Get_TargetMgr()->SetUpShader(pEffect, L"Target_Shadow", "g_ShadowTexture");

	if (lstrcmp(m_pKey, L"Aurora") == false)
		m_pMeshCom->SetUpShader(pEffect, 5);

	else if (lstrcmp(m_pKey, L"MagicBroken") == false)
		m_pMeshCom->SetUpShader(pEffect, 4);

	else if (lstrcmp(m_pKey, L"Pillar_a") == false || lstrcmp(m_pKey, L"Pillar_b") == false || lstrcmp(m_pKey, L"Pillar_c") == false)
		m_pMeshCom->SetUpShader(pEffect, 1);

	else if (lstrcmp(m_pKey, L"Pillar_a_haze") == false || lstrcmp(m_pKey, L"Pillar_b_haze") == false || lstrcmp(m_pKey, L"Pillar_c_haze") == false)
		m_pMeshCom->SetUpShader(pEffect, 1);

	else
		m_pMeshCom->SetUpShader(pEffect, 1);

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

	m_pShaderCom = (Engine::CShader*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Shader_Environ");
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

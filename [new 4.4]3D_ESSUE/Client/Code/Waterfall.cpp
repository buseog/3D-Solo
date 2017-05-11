#include "StdAfx.h"
#include "Waterfall.h"

CWaterfall::CWaterfall( LPDIRECT3DDEVICE9 pDevice )
: CGameObject(pDevice)
, m_pRendererCom(NULL)
, m_pTransformCom(NULL)
, m_pMtrCom(NULL)
, m_pMeshCom(NULL)
, m_pShaderCom(NULL)
, m_fOffset(0.f)
{

}

CWaterfall::~CWaterfall( void )
{

}

HRESULT CWaterfall::Initialize( void )
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_SCALE, &Engine::MyVec3(0.15f, 0.15f, 0.15f));
	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_ANGLE, &Engine::MyVec3(0.f, D3DX_PI, 0.f));

	return S_OK;
}

_int CWaterfall::Update( const _float& fDelta )
{
	_float fTime = fDelta;

	if (fTime == 0.f)
		fTime = Engine::Get_TimeMgr()->GetTimeDelta(L"Timer_60");

	m_fOffset += 2.f * fTime;

	Engine::CGameObject::Update(fTime);

	m_pRendererCom->AddRenderList(Engine::CRenderer::RENDER_ALPHA, this);

	return 0;
}

void CWaterfall::Render( void )
{
	LPD3DXEFFECT pEffect = m_pShaderCom->GetEffect();
	if (pEffect == NULL)
		return;

	SetUpShader(pEffect);
}

void CWaterfall::SetUpShader( LPD3DXEFFECT pEffect )
{
	pEffect->AddRef();

	MyMat matView, matProj;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);
	pEffect->SetFloat("g_fOffset", m_fOffset);
	pEffect->SetVector("g_vColor", &MyVec4(0.f, 0.f, 0.f, 0.7f));

	m_pTransformCom->SetUpShader(pEffect);

	m_pMeshCom->SetUpShader(pEffect, 2);

	Engine::Safe_Release(pEffect);
}

HRESULT CWaterfall::Ready_Component( void )
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

	m_pMeshCom = (Engine::CStaticMesh*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Mesh_Waterfall");
	if (m_pMeshCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(Engine::CGameObject::UPDATE_NONE, L"Com_Mesh", m_pMeshCom)))
		return E_FAIL;

	m_pShaderCom = (Engine::CShader*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Shader_Environ");
	if (m_pShaderCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(Engine::CGameObject::UPDATE_NONE, L"Com_Shader", m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CWaterfall::Free( void )
{
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pMtrCom);
	Engine::Safe_Release(m_pMeshCom);
	Engine::Safe_Release(m_pShaderCom);

	Engine::CGameObject::Free();
}

CWaterfall* CWaterfall::Create( LPDIRECT3DDEVICE9 pDevice )
{
	CWaterfall* pWaterfall = new CWaterfall(pDevice);

	if (FAILED(pWaterfall->Initialize()))
	{
		MSG_BOX("Terrain Created Failed");
		::Safe_Release(pWaterfall);
	}

	return pWaterfall;
}

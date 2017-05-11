#include "StdAfx.h"
#include "Water.h"

CWater::CWater( LPDIRECT3DDEVICE9 pDevice )
: CGameObject(pDevice)
, m_pRendererCom(NULL)
, m_pTransformCom(NULL)
, m_pMtrCom(NULL)
, m_pMeshCom(NULL)
, m_pShaderCom(NULL)
, m_fOffset(0.f)
, m_fTime(0.f)
{

}

CWater::~CWater( void )
{

}

HRESULT CWater::Initialize( void )
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_SCALE, &Engine::MyVec3(0.15f, 0.15f, 0.15f));
	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_ANGLE, &Engine::MyVec3(0.f, D3DX_PI, 0.f));
	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_POS, &Engine::MyVec3(0.f, -10.f, 0.f));

	m_fHeight = 42.f;
	m_fSpeed = 1.6f;
	m_fFrequency = 2.2f;

	return S_OK;
}

_int CWater::Update( const _float& fDelta )
{
	_float fTime = fDelta;

	if (fTime == 0.f)
		fTime = Engine::Get_TimeMgr()->GetTimeDelta(L"Timer_60");

	m_fOffset += 0.01f * fTime;
	m_fTime += fTime;

	if (Engine::Get_KeyMgr()->StayKeyDown(VK_NUMPAD1))
		m_fHeight += 0.1f;
	if (Engine::Get_KeyMgr()->StayKeyDown(VK_NUMPAD2))
		m_fHeight -= 0.1f;
	if (Engine::Get_KeyMgr()->StayKeyDown(VK_NUMPAD4))
		m_fSpeed += 0.1f;
	if (Engine::Get_KeyMgr()->StayKeyDown(VK_NUMPAD5))
		m_fSpeed -= 0.1f;
	if (Engine::Get_KeyMgr()->StayKeyDown(VK_NUMPAD7))
		m_fFrequency += 0.1f;
	if (Engine::Get_KeyMgr()->StayKeyDown(VK_NUMPAD8))
		m_fFrequency -= 0.1f;

	Engine::CGameObject::Update(fDelta);

	m_pRendererCom->AddRenderList(Engine::CRenderer::RENDER_ALPHA, this);

	return 0;
}

void CWater::Render( void )
{
	LPD3DXEFFECT pEffect = m_pShaderCom->GetEffect();
	if (pEffect == NULL)
		return;

	SetUpShader(pEffect);

	/*_tchar szBuf[128] = L"";
	swprintf_s(szBuf, L"height : %f", m_fHeight);
	Engine::Get_FontMgr()->Render(L"Font_Batang", szBuf, MyVec3(250.f, 0.f, 0.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	swprintf_s(szBuf, L"speed : %f", m_fSpeed);
	Engine::Get_FontMgr()->Render(L"Font_Batang", szBuf, MyVec3(250.f, 20.f, 0.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	swprintf_s(szBuf, L"Freque : %f", m_fFrequency);
	Engine::Get_FontMgr()->Render(L"Font_Batang", szBuf, MyVec3(250.f, 40.f, 0.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));*/
}

void CWater::SetUpShader( LPD3DXEFFECT pEffect )
{
	pEffect->AddRef();

	MyMat matView, matProj;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);
	pEffect->SetFloat("g_fOffset", m_fOffset);
	pEffect->SetFloat("g_fTime", m_fTime);
	pEffect->SetFloat("g_fHeight", m_fHeight);
	pEffect->SetFloat("g_fSpeed", m_fSpeed);
	pEffect->SetFloat("g_fFrequency", m_fFrequency);
	pEffect->SetVector("g_vColor", &MyVec4(0.f, 0.f, 0.f, 0.f));

	m_pTransformCom->SetUpShader(pEffect);

	m_pMeshCom->SetUpShader(pEffect, 3);

	Engine::Safe_Release(pEffect);
}

HRESULT CWater::Ready_Component( void )
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

	m_pMeshCom = (Engine::CStaticMesh*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Mesh_Water");
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

void CWater::Free( void )
{
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pMtrCom);
	Engine::Safe_Release(m_pMeshCom);
	Engine::Safe_Release(m_pShaderCom);

	Engine::CGameObject::Free();
}

CWater* CWater::Create( LPDIRECT3DDEVICE9 pDevice )
{
	CWater* pWater = new CWater(pDevice);

	if (FAILED(pWater->Initialize()))
	{
		MSG_BOX("Terrain Created Failed");
		::Safe_Release(pWater);
	}

	return pWater;
}

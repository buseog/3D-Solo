#include "stdafx.h"
#include "UI.h"
#include "Boss.h"

CUI::CUI(LPDIRECT3DDEVICE9 pDevice)
: Engine::CGameObject(pDevice)
, m_pRendererCom(NULL)
, m_pTransformCom(NULL)
, m_pTextureCom(NULL)
, m_pBufferCom(NULL)
, m_pShaderCom(NULL)
, m_bHide(false)
{

}

CUI::~CUI(void)
{

}

HRESULT CUI::Initialize(const _uint& iType)
{
	if(FAILED(Ready_Component()))
		return E_FAIL;	

	m_iType = iType;

	if (iType == 0)
	{
		m_fX = WINCX  * 0.5f;
		m_fY = 50.f;
		m_fSizeX = 400.f;
		m_fSizeY = 50.f;
	}
	else
	{
		m_fX = WINCX  * 0.5f;
		m_fY = 50.f;
		m_fSizeX = 320.f;
		m_fSizeY = 20.f;
	}

	m_fPercent = 1.f;

	return S_OK;
}

_int CUI::Update(const _float& fTimeDelta)
{	
	if (m_iType == 1)
		m_fPercent = ((CBoss*)Engine::Get_Management()->GetLayer(L"Layer_Boss")->GetObject(0))->GetHp();

	if (((CBoss*)Engine::Get_Management()->GetLayer(L"Layer_Boss")->GetObject(0))->GetHp() <= 0.f)
		m_bHide = true;

	Engine::CGameObject::Update(fTimeDelta);

	if (m_bHide == true)
		return 0;

	m_pRendererCom->AddRenderList(Engine::CRenderer::RENDER_UI, this);

	return 0;
}

void CUI::Render(void)
{
	LPD3DXEFFECT	pEffect = m_pShaderCom->GetEffect();

	if(NULL == pEffect)
		return;

	pEffect->AddRef();

	_uint		iNumPass = 0;

	m_pTransformCom->SetUpShader(pEffect);

	MyMat		matWorld, matView, matProj;
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixIdentity(&matView);	

	D3DXMatrixOrthoLH(&matProj, (_float)WINCX, (_float)WINCY, 0.0f, 1.f);

	matWorld._11 = m_fSizeX;
	matWorld._22 = m_fSizeY;
	matWorld._41 = m_fX - WINCX * 0.5f;
	matWorld._42 = -m_fY + WINCY * 0.5f;
	

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);
	pEffect->SetFloat("g_fPercent", m_fPercent);

	m_pTextureCom->SetUpShader(pEffect, m_iType);

	pEffect->Begin(&iNumPass, 0);

	pEffect->BeginPass(0);

	m_pBufferCom->Render();

	pEffect->EndPass();

	pEffect->End();
	
	Engine::Safe_Release(pEffect);		
}

HRESULT CUI::Ready_Component(void)
{
	m_pRendererCom = (Engine::CRenderer*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STATIC, L"Component_Renderer");
	if (m_pRendererCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(CGameObject::UPDATE_NONE, L"Com_Renderer", m_pRendererCom)))
		return E_FAIL;

	m_pTransformCom = (Engine::CTransform*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STATIC, L"Component_Transform");
	if(NULL == m_pTransformCom)
		return E_FAIL;
	if(FAILED(AddComponent(Engine::CGameObject::UPDATE_DO, L"Com_Transform", m_pTransformCom)))
		return E_FAIL;

	m_pTextureCom = (Engine::CTexture*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Texture_HpBar");
	if(NULL == m_pTextureCom)
		return E_FAIL;
	if(FAILED(AddComponent(Engine::CGameObject::UPDATE_NONE, L"Com_Texture", m_pTextureCom)))
		return E_FAIL;

	m_pBufferCom = (Engine::CRcTex*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STATIC, L"Component_Buffer_RcTex");
	if(NULL == m_pBufferCom)
		return E_FAIL;
	if(FAILED(AddComponent(Engine::CGameObject::UPDATE_NONE, L"Com_Buffer", m_pBufferCom)))
		return E_FAIL;

	m_pShaderCom = (Engine::CShader*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Shader_UI");
	if(NULL == m_pShaderCom)
		return E_FAIL;
	if(FAILED(AddComponent(Engine::CGameObject::UPDATE_NONE, L"Com_Shader", m_pShaderCom)))
		return E_FAIL;


	return S_OK;
}

void CUI::Free(void)
{	
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pTextureCom);
	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pShaderCom);

	Engine::CGameObject::Free();	
}

CUI* CUI::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint& iType)
{
	CUI* pInstance = new CUI(pGraphicDev);

	if(FAILED(pInstance->Initialize(iType)))
	{
		MSG_BOX("UI Created Failed");
		::Safe_Release(pInstance);
	}
	return pInstance;
}


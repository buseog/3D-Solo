#include "StdAfx.h"
#include "CommonEffect.h"

CCommonEffect::CCommonEffect( LPDIRECT3DDEVICE9 pDevice)
: Engine::CGameObject(pDevice)
, m_pRendererCom(NULL)
, m_pTransformCom(NULL)
, m_pTextureCom(NULL)
, m_pBufferCom(NULL)
, m_pShaderCom(NULL)
, m_iIndex(0)
, m_iType(0)
{
	ZeroMemory(m_szKey, sizeof(_tchar) * 256);
}

CCommonEffect::~CCommonEffect( void )
{

}

HRESULT CCommonEffect::Initialize( const _tchar* pEffectKey, const _uint& iIndex)
{
	lstrcpy(m_szKey, pEffectKey);
	m_iIndex = iIndex;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_ANGLE, &MyVec3(0.f, D3DXToRadian(rand() % 180), 0.f));

	m_fLifeTime = 0.1f;

	return S_OK;
}

_int CCommonEffect::Update( const _float& fDelta )
{
	m_fLifeTime -= fDelta;

	if (m_fLifeTime <= 0.f)
		m_bDestroy = true;

	if (m_iType == 1)
		m_pTransformCom->SetInfomation(Engine::CTransform::INFO_SCALE, &MyVec3(0.1f, 0.1f, 0.1f), 1);

	Engine::CGameObject::Update(fDelta);
	m_pRendererCom->AddRenderList(Engine::CRenderer::RENDER_ALPHA, this);

	return 0;
}

void CCommonEffect::Render( void )
{
	LPD3DXEFFECT	pEffect = m_pShaderCom->GetEffect();
	if (pEffect == NULL)
		return;

	SetUpShader(pEffect);
}

void CCommonEffect::SetUpShader( LPD3DXEFFECT pEffect )
{
	pEffect->AddRef();

	m_pTransformCom->SetUpShader(pEffect);

	MyMat matView, matProj;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	m_pTextureCom->SetUpShader(pEffect, m_iIndex);

	pEffect->Begin(NULL, 0);
	pEffect->BeginPass(2);

	m_pBufferCom->Render(); 

	pEffect->EndPass();
	pEffect->End();

	Engine::Safe_Release(pEffect);
}

void CCommonEffect::SetInformation( const _uint& iIndex, const MyVec3* pVec )
{
	if (iIndex == 0)
		m_pTransformCom->SetInfomation(Engine::CTransform::INFO_SCALE, pVec);
	else if (iIndex == 1)
		m_pTransformCom->SetInfomation(Engine::CTransform::INFO_ANGLE, pVec);
	else
		m_pTransformCom->SetInfomation(Engine::CTransform::INFO_POS, pVec);
}

HRESULT CCommonEffect::Ready_Component( void )
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

	m_pTextureCom = (Engine::CTexture*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, m_szKey);
	if (m_pTextureCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(Engine::CGameObject::UPDATE_NONE, L"Com_Texture", m_pTextureCom)))
		return E_FAIL;

	m_pBufferCom = (Engine::CTrailTex*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STATIC, L"Component_Buffer_RcTex");
	if (m_pBufferCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(Engine::CGameObject::UPDATE_NONE, L"Com_Buffer", m_pBufferCom)))
		return E_FAIL;

	m_pShaderCom = (Engine::CShader*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Shader_Effect");
	if (m_pShaderCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(Engine::CGameObject::UPDATE_NONE, L"Com_Shader", m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CCommonEffect::Free( void )
{
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pTextureCom);
	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pShaderCom);

	Engine::CGameObject::Free();
}

CCommonEffect* CCommonEffect::Create( LPDIRECT3DDEVICE9 pDevice, const _tchar* pEffectKey, const _uint& iIndex)
{
	CCommonEffect* pEffect = new CCommonEffect(pDevice);

	if (FAILED(pEffect->Initialize(pEffectKey, iIndex)))
	{
		MSG_BOX("Effect Created Failed");
		Engine::Safe_Release(pEffect);
	}
	return pEffect;
}


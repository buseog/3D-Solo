#include "StdAfx.h"
#include "Effect.h"

CEffect::CEffect( LPDIRECT3DDEVICE9 pDevice)
: Engine::CGameObject(pDevice)
, m_pRendererCom(NULL)
, m_pTransformCom(NULL)
, m_pTextureCom(NULL)
, m_pBufferCom(NULL)
, m_pShaderCom(NULL)
, m_fLiveTime(0.f)
, m_fFrame(0.f)
{

}

CEffect::~CEffect( void )
{

}

HRESULT CEffect::Initialize( const _tchar* pName )
{
	if (FAILED(Ready_Component(pName)))
		return E_FAIL;

	m_fLiveTime = 0.5f;

	return S_OK;
}

_int CEffect::Update( const _float& fDelta )
{
	Engine::CGameObject::Update(fDelta);

	m_pRendererCom->AddRenderList(Engine::CRenderer::RENDER_ALPHA, this);

	m_fFrame += 6.f * fDelta;
	if (m_fFrame > 6.f)
		m_fFrame = 0.f;

	if (m_fLiveTime > 0.f)
	{
		m_fLiveTime -= fDelta;
	}
	else
		m_bDestroy = true;

	return 0;
}

void CEffect::Render( void )
{
	LPD3DXEFFECT	pEffect = m_pShaderCom->GetEffect();
	if (pEffect == NULL)
		return;

	SetUpShader(pEffect);
}

void CEffect::SetUpShader( LPD3DXEFFECT pEffect )
{
	pEffect->AddRef();

	m_pTransformCom->SetUpShader(pEffect);

	MyMat matView, matProj;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	m_pTextureCom->SetUpShader(pEffect, m_fFrame);

	pEffect->Begin(NULL, 0);
	pEffect->BeginPass(1);

	m_pBufferCom->Render();

	pEffect->EndPass();
	pEffect->End();

	Engine::Safe_Release(pEffect);
}

HRESULT CEffect::Ready_Component( const _tchar* pName )
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

	m_pTextureCom = (Engine::CTexture*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Texture_LongBladeTrail");
	if (m_pTextureCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(Engine::CGameObject::UPDATE_NONE, L"Com_Texture", m_pTextureCom)))
		return E_FAIL;

	m_pBufferCom = (Engine::CTrainTex*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Buffer_Train");
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

void CEffect::Free( void )
{
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pTextureCom);
	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pShaderCom);

	Engine::CGameObject::Free();
}

CEffect* CEffect::Create( LPDIRECT3DDEVICE9 pDevice, const _tchar* pName )
{
	CEffect* pEffect = new CEffect(pDevice);

	if (FAILED(pEffect->Initialize(pName)))
	{
		MSG_BOX("Effect Created Failed");
		Engine::Safe_Release(pEffect);
	}
	return pEffect;
}

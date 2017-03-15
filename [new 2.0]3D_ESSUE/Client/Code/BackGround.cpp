#include "StdAfx.h"
#include "BackGround.h"

#include "Export_Engine.h"

CBackGround::CBackGround( LPDIRECT3DDEVICE9 pDevice )
: CGameObject(pDevice)
, m_pRendererCom(NULL)
, m_pTransformCom(NULL)
, m_pTextureCom(NULL)
, m_pBufferCom(NULL)
{

}

CBackGround::~CBackGround( void )
{

}

HRESULT CBackGround::Initialize( void )
{
	if(FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_SCALE, &D3DXVECTOR3(2.f, 2.f, 2.f));

	return S_OK;
}

_int CBackGround::Update( const float& fDelta )
{
	Engine::CGameObject::Update(fDelta);

	m_pRendererCom->AddRenderList(Engine::CRenderer::RENDER_PRIORITY, this);

	return 0;
}

void CBackGround::Render( void )
{
	m_pTransformCom->SetTransform();
	m_pTextureCom->Render();
	m_pBufferCom->Render();
}

HRESULT CBackGround::Ready_Component( void )
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

	m_pTextureCom = (Engine::CTexture*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_LOGO, L"Component_Texture_Logo");
	if (m_pTextureCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(Engine::CGameObject::UPDATE_NONE, L"Com_Texture", m_pTextureCom)))
		return E_FAIL;

	m_pBufferCom = (Engine::CRcTex*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STATIC, L"Component_Buffer_RcTex");
	if (m_pBufferCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(Engine::CGameObject::UPDATE_NONE, L"Com_Buffer", m_pBufferCom)))
		return E_FAIL;

	return S_OK;
}

void CBackGround::Free( void )
{
	::Safe_Release(m_pRendererCom);
	::Safe_Release(m_pTransformCom);
	::Safe_Release(m_pTextureCom);
	::Safe_Release(m_pBufferCom);

	Engine::CGameObject::Free();
}

CBackGround* CBackGround::Create( LPDIRECT3DDEVICE9 pDevice )
{
	CBackGround* pBack = new CBackGround(pDevice);

	if (FAILED(pBack->Initialize()))
	{
		MSG_BOX("BackGround Created Failed");
		::Safe_Release(pBack);
	}

	return pBack;
}

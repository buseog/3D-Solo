#include "StdAfx.h"
#include "SkyBox.h"

#include "Export_Engine.h"

CSkyBox::CSkyBox(LPDIRECT3DDEVICE9 pDevice)
: CGameObject(pDevice)
, m_pRendererCom(NULL)
, m_pTransformCom(NULL)
, m_pTextureCom(NULL)
, m_pBufferCom(NULL)
{

}

CSkyBox::~CSkyBox(void)
{

}

HRESULT CSkyBox::Initialize(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_SCALE, &D3DXVECTOR3(100.f, 100.f, 100.f));

	return S_OK;
}

_int CSkyBox::Update(const _float& fDelta)
{
	Engine::CGameObject::Update(fDelta);

	m_pRendererCom->AddRenderList(Engine::CRenderer::RENDER_PRIORITY, this);

	return 0;
}

void CSkyBox::Render(void)
{
	m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pTransformCom->SetWorld();
	m_pTextureCom->Render(3);
	m_pBufferCom->Render();

	m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}

HRESULT CSkyBox::Ready_Component(void)
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

	m_pTextureCom = (Engine::CTexture*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Texture_SkyBox");
	if (m_pTextureCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(Engine::CGameObject::UPDATE_NONE, L"Com_Transform", m_pTextureCom)))
		return E_FAIL;

	m_pBufferCom = (Engine::CCubeTex*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STATIC, L"Component_Buffer_CubeTex");
	if (m_pBufferCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(Engine::CGameObject::UPDATE_NONE, L"Com_Buffer", m_pBufferCom)))
		return E_FAIL;

	return S_OK;
}

void CSkyBox::Free(void)
{
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pTextureCom);
	Engine::Safe_Release(m_pBufferCom);

	Engine::CGameObject::Free();
}

CSkyBox* CSkyBox::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CSkyBox* pSkyBox = new CSkyBox(pDevice);

	if (FAILED(pSkyBox->Initialize()))
	{
		MSG_BOX("SkyBox Created Failed");
		::Safe_Release(pSkyBox);
	}

	return pSkyBox;
}

#include "StdAfx.h"
#include "Logo.h"

#include "Export_Engine.h"
#include "Loading.h"
#include "BackGround.h"
#include "Stage.h"

CLogo::CLogo(LPDIRECT3DDEVICE9 pDevice)
: CScene(pDevice)
, m_pLoading(NULL)
, m_pBack(NULL)
{

}

CLogo::~CLogo(void)
{

}

HRESULT CLogo::Initialize(void)
{
	if (FAILED(Ready_RenderState()))
		return E_FAIL;

	if (FAILED(Ready_Light()))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(Ready_LayerBackGround()))
		return E_FAIL;

	m_pLoading = CLoading::Create(m_pDevice, CLoading::LOADING_STAGE);
	if(NULL == m_pLoading)
		return E_FAIL;

	return S_OK;
}

_int CLogo::Update(const _float& fDelta)
{
	Engine::CScene::Update(fDelta);
	m_pBack->SetPercent(m_pLoading->GetLoadPercent());

	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		if (m_pLoading->GetComplete() == true)
		{
 			Engine::CScene* pStage = CStage::Create(m_pDevice);
			if (FAILED(Engine::Get_Management()->SetScene(pStage)))
				return -1;

			return 0;
		}
	}

	return 0;
}

void CLogo::Render(void)
{
	Engine::Get_FontMgr()->Render(L"Font_Batang", m_pLoading->GetString(), Engine::MyVec3(300.f, 0.f, 0.f), D3DXCOLOR(1.f, 0.f, 0.f, 1.f));
}

HRESULT CLogo::Ready_RenderState(void)
{
	m_pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	return S_OK;
}

HRESULT CLogo::Ready_Light(void)
{
	return S_OK;
}

HRESULT CLogo::Ready_Component( void )
{
	Engine::CComponent* pComponent = NULL;

	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Logo/Logo.png", Engine::CTexture::TEX_NORMAL);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_LOGO, L"Component_Texture_Logo", pComponent);

	return S_OK;
}

HRESULT CLogo::Ready_LayerBackGround(void)
{
	Engine::CLayer*	pLayer = Engine::CLayer::Create();
	if (pLayer == NULL)
		return E_FAIL;

	Engine::CGameObject* pGameObject = NULL;

	//// BackGround
	pGameObject = m_pBack = CBackGround::Create(m_pDevice);
	if (pGameObject == NULL)
		goto except;
	pLayer->AddGameObject(pGameObject);

	CScene::AddLayer(L"Layer_BackGround", pLayer);

	return S_OK;

except:
	::Safe_Release(pLayer);
	return E_FAIL;
}

void CLogo::Free(void)
{
	Engine::CScene::Free();

	Engine::Get_ComponentMgr()->Clear(SCENE_LOGO);

	::Safe_Release(m_pLoading);
}

CLogo* CLogo::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CLogo* pLogo = new CLogo(pDevice);

	if (FAILED(pLogo->Initialize()))
	{
		::MSG_BOX("Logo Created Failed");
		Engine::Safe_Release(pLogo);
	}

	return pLogo;
}

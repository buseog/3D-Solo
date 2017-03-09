#include "StdAfx.h"
#include "Stage.h"

#include "Export_Engine.h"
#include "Terrain.h"
#include "DynamicCamera.h"
#include "SkyBox.h"
#include "Obstacles.h"
#include "Player.h"
#include "LongBlade.h"

CStage::CStage(LPDIRECT3DDEVICE9 pDevice)
: CScene(pDevice)
{

}

CStage::~CStage(void)
{

}

HRESULT CStage::Initialize(void)
{
	if (FAILED(Ready_RenderState()))
		return E_FAIL;

	if (FAILED(Ready_Light()))
		return E_FAIL;

	if (FAILED(Ready_LayerCamera()))
		return E_FAIL;

	if (FAILED(Ready_LayerBackGround()))
		return E_FAIL;

	if (FAILED(Ready_LayerSkyBox()))
		return E_FAIL;

	if (FAILED(Ready_LayerPlayer()))
		return E_FAIL;

	return S_OK;
}

_int CStage::Update(const _float& fDelta)
{
	Engine::CScene::Update(fDelta);

	return 0;
}

void CStage::Render(void)
{
	
}

HRESULT CStage::Ready_RenderState(void)
{
	m_pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	m_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	return S_OK;
}

HRESULT CStage::Ready_Light(void)
{
	D3DLIGHT9 LightInfo;
	ZeroMemory(&LightInfo, sizeof(D3DLIGHT9));

	LightInfo.Type = D3DLIGHT_DIRECTIONAL;
	LightInfo.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	LightInfo.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	LightInfo.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	LightInfo.Direction = D3DXVECTOR3(1.f, -1.f, 1.f);

	m_pDevice->SetLight(0, &LightInfo);
	m_pDevice->LightEnable(0, TRUE);

	return S_OK;
}

HRESULT CStage::Ready_LayerCamera( void )
{
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	if (pLayer == NULL)
		return E_FAIL;

	Engine::CGameObject* pGameObject = NULL;

	pGameObject = CDynamicCamera::Create(m_pDevice, &MyVec3(0.f, 10.f, -10.f), &MyVec3(0.f, 0.f, 0.f), &MyVec3(0.f, 1.f, 0.f));
	if (pGameObject == NULL)
		goto except;
	pLayer->AddGameObject(pGameObject);

	CScene::AddLayer(L"Layer_Camera", pLayer);

	return S_OK;

except:
	::Safe_Release(pLayer);
	return E_FAIL;
}

HRESULT CStage::Ready_LayerBackGround(void)
{
	Engine::CLayer*	pLayer = Engine::CLayer::Create();
	if (pLayer == NULL)
		return E_FAIL;

	Engine::CGameObject* pGameObject = NULL;

	//// Terrain
	pGameObject = CTerrain::Create(m_pDevice);
	if (pGameObject == NULL)
		goto except;
	pLayer->AddGameObject(pGameObject);

	
	pGameObject = CObstacles::Create(m_pDevice);
	if (pGameObject == NULL)
		return E_FAIL;
	pLayer->AddGameObject(pGameObject);

	CScene::AddLayer(L"Layer_BackGround", pLayer);

	return S_OK;

except:
	::Safe_Release(pLayer);
	return E_FAIL;
}

HRESULT CStage::Ready_LayerSkyBox( void )
{
	Engine::CLayer*	pLayer = Engine::CLayer::Create();
	if (pLayer == NULL)
		return E_FAIL;

	Engine::CGameObject* pGameObject = NULL;

	//// SkyBox
	pGameObject = CSkyBox::Create(m_pDevice);
	if (pGameObject == NULL)
		goto except;
	pLayer->AddGameObject(pGameObject);

	CScene::AddLayer(L"Layer_SkyBox", pLayer);

	return S_OK;

except:
	::Safe_Release(pLayer);
	return E_FAIL;
}

HRESULT CStage::Ready_LayerPlayer( void )
{
	Engine::CLayer*	pLayer = Engine::CLayer::Create();
	if (pLayer == NULL)
		return E_FAIL;

	Engine::CGameObject* pGameObject = NULL;
	CPlayer* pPlaye = NULL;

	//// Player
	pGameObject = pPlaye = CPlayer::Create(m_pDevice);
	if (pGameObject == NULL)
		goto except;
	pLayer->AddGameObject(pGameObject);

	//// LongBlade
	pGameObject = CLongBlade::Create(m_pDevice);
	if (pGameObject == NULL)
		goto except;
	pLayer->AddGameObject(pGameObject);
	((CLongBlade*)pGameObject)->SetPlayer(pPlaye);

	CScene::AddLayer(L"Layer_Player", pLayer);

	return S_OK;

except:
	::Safe_Release(pLayer);
	return E_FAIL;
}

void CStage::Free(void)
{
	Engine::CScene::Free();
}

CStage* CStage::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CStage* pStage = new CStage(pDevice);

	if (FAILED(pStage->Initialize()))
	{
		::MSG_BOX("Logo Created Failed");
		Engine::Safe_Release(pStage);
	}

	return pStage;
}

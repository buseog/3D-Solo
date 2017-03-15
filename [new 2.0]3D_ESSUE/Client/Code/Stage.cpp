#include "StdAfx.h"
#include "Stage.h"

#include "Terrain.h"
#include "DefaultCamera.h"
#include "StopCamera.h"
#include "ActionCamera.h"
#include "SkyBox.h"
#include "Obstacles.h"
#include "Player.h"
#include "LongBlade.h"
#include "Boss.h"

CStage::CStage(LPDIRECT3DDEVICE9 pDevice)
: CScene(pDevice)
, m_pCamMgr(Engine::Get_CameraMgr())
, m_bStop(false)
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

	if (FAILED(Ready_LayerBackGround()))
		return E_FAIL;

	if (FAILED(Ready_LayerSkyBox()))
		return E_FAIL;

	/*if (FAILED(Ready_LayerMonster()))
		return E_FAIL;*/

	if (FAILED(Ready_LayerPlayer()))
		return E_FAIL;

	if (FAILED(Ready_Camera()))
		return E_FAIL;

	return S_OK;
}

_int CStage::Update(const _float& fDelta)
{
	m_pCamMgr->Update(fDelta);
	if (Engine::Get_KeyMgr()->KeyDown(VK_F2))
	{
		m_pCamMgr->SetCamera(L"Cam_Action");
	}

	if (Engine::Get_KeyMgr()->KeyDown(VK_F1))
	{
		if (m_bStop == false)
		{
			m_pCamMgr->SetCamera(L"Cam_Stop");
			((CStopCamera*)m_pCamMgr->GetCamera(L"Cam_Stop"))->SetCameraSetting();
			m_bStop = true;
		}
		else
		{
			m_pCamMgr->SetCamera(L"Cam_Default");
			m_bStop = false;
		}
	}

	if (m_bStop == true)
		Engine::CScene::Update(0.f);
	else
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

HRESULT CStage::Ready_LayerMonster( void )
{
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	if (pLayer == NULL)
		return E_FAIL;

	Engine::CGameObject* pGameObject = NULL;

	pGameObject = CBoss::Create(m_pDevice);
	if (pGameObject == NULL)
		goto except;
	pLayer->AddGameObject(pGameObject);

	CScene::AddLayer(L"Layer_Monster", pLayer);

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
		goto except;
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

HRESULT CStage::Ready_Camera( void )
{
	CPlayer* pPlayer = (CPlayer*)GetLayer(L"Layer_Player")->GetObject(0);
	Engine::CTransform* pTrans = (Engine::CTransform*)pPlayer->GetComponent(L"Com_Transform");

	Engine::CCamera* pDefaultCam = CDefaultCamera::Create(m_pDevice, pTrans->GetWorldMatrix(), &MyVec3(0.f, 1.f, 0.f));
	if (pDefaultCam == NULL)
		return E_FAIL;

	m_pCamMgr->AddCamera(L"Cam_Default", pDefaultCam);
	m_pCamMgr->SetCamera(L"Cam_Default");

	pDefaultCam = CStopCamera::Create(m_pDevice, &MyVec3(0.f, 10.f, -10.f), &MyVec3(0.f, 0.f, 0.f), &MyVec3(0.f, 1.f, 0.f));
	if (pDefaultCam == NULL)
		return E_FAIL;
	m_pCamMgr->AddCamera(L"Cam_Stop", pDefaultCam);

	pDefaultCam = CActionCamera::Create(m_pDevice, pPlayer, (Engine::CDynamicMesh*)pPlayer->GetComponent(L"Com_Mesh"), &MyVec3(0.f, 1.f, 0.f));
	if (pDefaultCam == NULL)
		return E_FAIL;
	m_pCamMgr->AddCamera(L"Cam_Action", pDefaultCam);

	return S_OK;
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

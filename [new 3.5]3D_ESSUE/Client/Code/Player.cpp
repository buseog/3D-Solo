#include "StdAfx.h"
#include "Player.h"
#include "TrailEffect.h"
#include "Satellite.h"
#include "ManaStone.h"
#include "CommonEffect.h"
#include "AttachEffect.h"
#include "Teleport.h"
#include "SharpenCombo.h"

CPlayer::CPlayer( LPDIRECT3DDEVICE9 pDevice )
: Engine::CGameObject(pDevice)
, m_pKeyMgr(Engine::Get_KeyMgr())
, m_pRendererCom(NULL)
, m_pTransformCom(NULL)
, m_pMtrCom(NULL)
, m_pMeshCom(NULL)
, m_pCollisionCom(NULL)
, m_pShaderCom(NULL)
, m_iStateID(BattleIdle)
, m_pBlurShaderCom(NULL)
, m_pShaderBufferCom(NULL)
, m_pSatellite(NULL)
, m_iEffectCnt(0)
, m_fEffectCool(0.f)
{

}

CPlayer::~CPlayer( void )
{

}

HRESULT CPlayer::Initialize( void )
{
	if (FAILED(ReadyComponent()))
		return E_FAIL;

	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_SCALE, &Engine::MyVec3(0.1f, 0.1f, 0.1f));
	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_POS, &Engine::MyVec3(0.f, 0.f, 350.f));
	m_pMeshCom->SetAnimation(0);

	m_fSpeed = 20.f;
	m_fTime = 0.6f;

	m_pSatellite = CSatellite::Create(m_pDevice, L"Component_Texture_Satellite");

	D3DVIEWPORT9	ViewPort;
	m_pDevice->GetViewport(&ViewPort);

	if(FAILED(D3DXCreateTexture(m_pDevice, ViewPort.Width, ViewPort.Height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pTargetTexture[0])))
		return E_FAIL;
	m_pTargetTexture[0]->GetSurfaceLevel(0, &m_pTargetSurface[0]);

	if(FAILED(D3DXCreateTexture(m_pDevice, ViewPort.Width, ViewPort.Height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pTargetTexture[1])))
		return E_FAIL;
	m_pTargetTexture[1]->GetSurfaceLevel(0, &m_pTargetSurface[1]);

	return S_OK;
}

_int CPlayer::Update( const _float& fDelta )
{
	m_fTime -= fDelta;

	KeyCheck(fDelta);
	Active(fDelta);

	Engine::CGameObject::Update(fDelta);
	m_pSatellite->Update(fDelta);

	m_pMeshCom->PlayAnimation(fDelta);
	AnimationCheck();

	m_pRendererCom->AddRenderList(Engine::CRenderer::RENDER_NONEALPHA, this);

	return 0;
}

void CPlayer::Render( void )
{
	_tchar szBuf[128] = L"";
	swprintf_s(szBuf, L"%f", m_pTransformCom->GetInfomation(Engine::CTransform::INFO_POS)->z);
	Engine::Get_FontMgr()->Render(L"Font_Batang", szBuf, MyVec3(400.f, 0.f, 0.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	/*PDIRECT3DSURFACE9			pBackBuffer = NULL;

	m_pDevice->GetRenderTarget(0, &pBackBuffer);

	m_pDevice->SetRenderTarget(0, m_pTargetSurface[0]);
	m_pDevice->SetRenderTarget(1, m_pTargetSurface[1]);

	if (m_fTime < 0)
	{
		m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DXCOLOR(0.f, 0.f, 0.f, 0.f), 1.f, 0);
		m_fTime = 0.4f;
	}*/

	////
	LPD3DXEFFECT	pEffect = m_pShaderCom->GetEffect();
	if (pEffect == NULL)
		return;
	SetUpShader(pEffect);
	////

	/*m_pDevice->SetRenderTarget(0, pBackBuffer);

	LPD3DXEFFECT	pEffect2 = m_pBlurShaderCom->GetEffect();
	if(NULL == pEffect2)
		return;

	pEffect2->AddRef();

	pEffect2->SetTexture("g_BaseTexture", m_pTargetTexture[0]);
	pEffect2->SetTexture("g_BlurTexture", m_pTargetTexture[1]);  

	pEffect2->Begin(0, NULL);
	pEffect2->BeginPass(0);

	m_pShaderBufferCom->Render();

	pEffect2->EndPass();
	pEffect2->End();

	Engine::Safe_Release(pEffect2);*/
	m_pCollisionCom->Render(Engine::CCollisionBox::COLL_OBB, m_pTransformCom->GetWorldMatrix());
	m_pSatellite->SetUpShader();

}

void CPlayer::SetUpShader( LPD3DXEFFECT pEffect )
{
	pEffect->AddRef();

	D3DLIGHT9	LightInfo;
	ZeroMemory(&LightInfo, sizeof(D3DLIGHT9));
	m_pDevice->GetLight(0, &LightInfo);

	pEffect->SetVector("g_vLightDir", (MyVec4*)&LightInfo.Direction);
	pEffect->SetVector("g_vLightDiffuse", (MyVec4*)&LightInfo.Diffuse);
	pEffect->SetVector("g_vLightAmbient", (MyVec4*)&LightInfo.Ambient);
	pEffect->SetVector("g_vLightSpecular", (MyVec4*)&LightInfo.Specular);
	pEffect->SetVector("g_vCamPos", (MyVec4*)Engine::Get_CameraMgr()->GetCameraEye());

	MyMat matView, matProj;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	m_pTransformCom->SetUpShader(pEffect);
	m_pMtrCom->SetUpShader(pEffect);
	m_pMeshCom->SetUpShader(pEffect);

	Engine::Safe_Release(pEffect);
}

void CPlayer::KeyCheck( const _float& fDelta )
{
	if (fDelta == 0.f)
		return;

	Move(fDelta);
	Command(fDelta);
}

void CPlayer::Active( const _float& fDelta )
{
	if (m_iStateID == RuinBlade)
	{
		m_fEffectCool -= fDelta;

		if (m_pMeshCom->ProgressAnimation() > 1.3 && m_pMeshCom->ProgressAnimation() < 1.4)
		{
			
		}
		
		if (m_pMeshCom->ProgressAnimation() > 1.3 && m_pMeshCom->ProgressAnimation() < 2.3)
		{
			
		}

		if (m_pMeshCom->ProgressAnimation() > 5 && m_pMeshCom->ProgressAnimation() < 6)
		{
			if (m_fEffectCool > 0.f)
				return;

			MyVec3 vLook = *(MyVec3*)&m_pTransformCom->GetWorldMatrix()->m[2][0];
			D3DXVec3Normalize(&vLook, &vLook);
			MyVec3 vPos = *(MyVec3*)&m_pTransformCom->GetWorldMatrix()->m[3][0];

			for (_uint i = 0; i < 5; ++i)
			{
				
			}
			++m_iEffectCnt;
			m_fEffectCool = 0.3f;
		}
	}

	if (m_iStateID == SharpenCombo)
	{
		if (m_pMeshCom->ProgressAnimation() > 0.8 && m_pMeshCom->ProgressAnimation() < 0.82)
		{
			CCommonEffect* pEffect = CCommonEffect::Create(m_pDevice, L"Component_Texture_MagicCircle", 1);
			Engine::Get_Management()->GetLayer(L"Layer_Player")->AddGameObject(pEffect);
			pEffect->SetLifeTime(0.3f);
			pEffect->SetInformation(0, &MyVec3(12.f, 12.f, 12.f));
			pEffect->SetInformation(1, &MyVec3(0.f, m_pTransformCom->GetInfomation(Engine::CTransform::INFO_ANGLE)->y, 0.f));
			pEffect->SetInformation(2, &MyVec3(*m_pTransformCom->GetInfomation(Engine::CTransform::INFO_POS) + MyVec3(0.f, 7.f, 0.f)));
		}
		else if (m_pMeshCom->ProgressAnimation() > 0.8 && m_pMeshCom->ProgressAnimation() < 1.2)
		{
			CCommonEffect* pEffect = CCommonEffect::Create(m_pDevice, L"Component_Texture_Ring_Outer_Wind");
			Engine::Get_Management()->GetLayer(L"Layer_Player")->AddGameObject(pEffect);
			pEffect->SetLifeTime(0.25f);
			pEffect->SetInformation(0, &MyVec3(9.f, 9.f, 9.f));
			pEffect->SetInformation(1, &MyVec3(0.f, m_pTransformCom->GetInfomation(Engine::CTransform::INFO_ANGLE)->y, 0.f));
			pEffect->SetInformation(2, &MyVec3(*m_pTransformCom->GetInfomation(Engine::CTransform::INFO_POS) + MyVec3(0.f, 7.f, 0.f)));
		}
		MyVec3 vMove = *(MyVec3*)&m_pMeshCom->GetPartsMatrix("ValveBiped_Anim_Attachment_GRAPPLING")->m[3][0];
		MyMat matScale, matRot, matTrans;
		D3DXMatrixScaling(&matScale, 0.1f, 0.1f, 0.1f);
		D3DXMatrixRotationY(&matRot, m_pTransformCom->GetInfomation(Engine::CTransform::INFO_ANGLE)->y);
		D3DXVec3TransformCoord(&vMove, &vMove, &(matScale * matRot));
		vMove.x *= -1.f;
		vMove.z *= -1.f;

		m_pTransformCom->SetInfomation(Engine::CTransform::INFO_POS, &(m_vPrevPos + vMove));
	}

	if (m_iStateID == Teleport)
	{
		MyVec3 vPos = *(MyVec3*)&m_pTransformCom->GetWorldMatrix()->m[3][0];
		MyVec3 vDir = *(MyVec3*)&m_pTransformCom->GetWorldMatrix()->m[2][0];
		m_pTransformCom->SetInfomation(Engine::CTransform::INFO_POS, &(vPos + vDir * m_fSpeed * fDelta * 35.f));
	}

	if (m_iStateID == DoomSayer)
	{
		MyVec3 vLook = *(MyVec3*)&m_pTransformCom->GetWorldMatrix()->m[2][0];
		D3DXVec3Normalize(&vLook, &vLook);

		if (m_pMeshCom->ProgressAnimation() > 0.2 && m_pMeshCom->ProgressAnimation() < 0.6)
			m_pTransformCom->Straight((Engine::MyVec3*)&vLook, m_fSpeed * fDelta * 0.4f);

		else if (m_pMeshCom->ProgressAnimation() > 0.7 && m_pMeshCom->ProgressAnimation() < 1.2)
			m_pTransformCom->Straight((Engine::MyVec3*)&vLook, m_fSpeed * fDelta * 1.1f);
	}
	if (m_iStateID == DoomSayerCombo)
	{
		MyVec3 vLook = *(MyVec3*)&m_pTransformCom->GetWorldMatrix()->m[2][0];
		D3DXVec3Normalize(&vLook, &vLook);

		if (m_pMeshCom->ProgressAnimation() > 0.45 && m_pMeshCom->ProgressAnimation() < 0.47)
		{
			MyVec3 vRight = *D3DXVec3Normalize(&vRight, (MyVec3*)&m_pTransformCom->GetWorldMatrix()->m[0][0]) * 1.5f;
			MyVec3 vUp(0.f, 0.5f, 0.f);

			CCommonEffect* pEffect = CCommonEffect::Create(m_pDevice, L"Component_Texture_MagicCircle", 1);
			Engine::Get_Management()->GetLayer(L"Layer_Player")->AddGameObject(pEffect);
			pEffect->SetLifeTime(0.5f);
			pEffect->SetInformation(0, &MyVec3(1.f, 1.f, 1.f));
			pEffect->SetInformation(1, &MyVec3(0.f, m_pTransformCom->GetInfomation(Engine::CTransform::INFO_ANGLE)->y - D3DX_PI / 3.f, 0.f));
			pEffect->SetInformation(2, (MyVec3*)&(*m_pTransformCom->GetInfomation(Engine::CTransform::INFO_POS) - vRight + vUp));

			pEffect = CCommonEffect::Create(m_pDevice, L"Component_Texture_SlashPoint");
			Engine::Get_Management()->GetLayer(L"Layer_Player")->AddGameObject(pEffect);
			pEffect->SetLifeTime(0.5f);
			pEffect->SetInformation(0, &MyVec3(2.f, 4.f, 2.f));
			pEffect->SetInformation(1, &MyVec3(D3DXToRadian(-90.f), m_pTransformCom->GetInfomation(Engine::CTransform::INFO_ANGLE)->y - D3DX_PI / 3.f, 0.f));
			pEffect->SetInformation(2, (MyVec3*)&(*m_pTransformCom->GetInfomation(Engine::CTransform::INFO_POS) - vRight + vUp));
		}
		if (m_pMeshCom->ProgressAnimation() > 0.2 && m_pMeshCom->ProgressAnimation() < 1.6)
		{
			m_pTransformCom->Straight((Engine::MyVec3*)&vLook, m_fSpeed * fDelta * 0.8f);
		}
		else if (m_pMeshCom->ProgressAnimation() > 1.6 && m_pMeshCom->ProgressAnimation() < 2.8)
		{
			m_pTransformCom->Straight((Engine::MyVec3*)&vLook, m_fSpeed * fDelta * 0.2f);
		}
	}
}

void CPlayer::Move( const _float& fDelta )
{
	if (m_pKeyMgr->StayKeyDown('W') && m_pKeyMgr->StayKeyDown('A') && m_iStateID < Teleport)
	{
		MyMat matView;
		m_pDevice->GetTransform(D3DTS_VIEW, &matView);
		D3DXMatrixInverse(&matView, 0 ,&matView);

		MyVec3 vLook;
		memcpy(&vLook, &matView.m[2][0], sizeof(MyVec3));
		vLook.y = 0.f;
		D3DXVec3Normalize(&vLook, &vLook);

		MyVec3 vRight;
		memcpy(&vRight, &matView.m[0][0], sizeof(MyVec3));
		vRight.y = 0.f;
		D3DXVec3Normalize(&vRight, &vRight);

		MyVec3 vDir = vLook - vRight;
		D3DXVec3Normalize(&vDir, &vDir);

		m_iStateID = RunLower;
		m_pMeshCom->SetAnimation(m_iStateID);
		m_pTransformCom->Move((Engine::MyVec3*)&vDir, m_fSpeed * fDelta);	
	}
	else if (m_pKeyMgr->StayKeyDown('W') && m_pKeyMgr->StayKeyDown('D') && m_iStateID < Teleport)
	{
		MyMat matView;
		m_pDevice->GetTransform(D3DTS_VIEW, &matView);
		D3DXMatrixInverse(&matView, 0 ,&matView);

		MyVec3 vLook;
		memcpy(&vLook, &matView.m[2][0], sizeof(MyVec3));
		vLook.y = 0.f;
		D3DXVec3Normalize(&vLook, &vLook);

		MyVec3 vRight;
		memcpy(&vRight, &matView.m[0][0], sizeof(MyVec3));
		vRight.y = 0.f;
		D3DXVec3Normalize(&vRight, &vRight);

		MyVec3 vDir = vLook + vRight;
		D3DXVec3Normalize(&vDir, &vDir);

		m_iStateID = RunLower;
		m_pMeshCom->SetAnimation(m_iStateID);
		m_pTransformCom->Move((Engine::MyVec3*)&vDir, m_fSpeed * fDelta);	
	}
	else if (m_pKeyMgr->StayKeyDown('W') && m_iStateID < Teleport)
	{
		MyMat matView;
		m_pDevice->GetTransform(D3DTS_VIEW, &matView);
		D3DXMatrixInverse(&matView, 0 ,&matView);

		MyVec3 vLook;
		memcpy(&vLook, &matView.m[2][0], sizeof(MyVec3));
		vLook.y = 0.f;
		D3DXVec3Normalize(&vLook, &vLook);

		m_iStateID = RunLower;
		m_pMeshCom->SetAnimation(m_iStateID);
		m_pTransformCom->Move((Engine::MyVec3*)&vLook, m_fSpeed * fDelta);		
	}
	else if (m_pKeyMgr->StayKeyDown('S') && m_pKeyMgr->StayKeyDown('A') && m_iStateID < Teleport)
	{
		MyMat matView;
		m_pDevice->GetTransform(D3DTS_VIEW, &matView);
		D3DXMatrixInverse(&matView, 0 ,&matView);

		MyVec3 vLook;
		memcpy(&vLook, &matView.m[2][0], sizeof(MyVec3));
		vLook.y = 0.f;
		D3DXVec3Normalize(&vLook, &vLook);

		MyVec3 vRight;
		memcpy(&vRight, &matView.m[0][0], sizeof(MyVec3));
		vRight.y = 0.f;
		D3DXVec3Normalize(&vRight, &vRight);

		MyVec3 vDir = -vLook - vRight;
		D3DXVec3Normalize(&vDir, &vDir);

		m_iStateID = RunLower;
		m_pMeshCom->SetAnimation(m_iStateID);
		m_pTransformCom->Move((Engine::MyVec3*)&vDir, m_fSpeed * fDelta);	
	}
	else if (m_pKeyMgr->StayKeyDown('S') && m_pKeyMgr->StayKeyDown('D') && m_iStateID < Teleport)
	{
		MyMat matView;
		m_pDevice->GetTransform(D3DTS_VIEW, &matView);
		D3DXMatrixInverse(&matView, 0 ,&matView);

		MyVec3 vLook;
		memcpy(&vLook, &matView.m[2][0], sizeof(MyVec3));
		vLook.y = 0.f;
		D3DXVec3Normalize(&vLook, &vLook);

		MyVec3 vRight;
		memcpy(&vRight, &matView.m[0][0], sizeof(MyVec3));
		vRight.y = 0.f;
		D3DXVec3Normalize(&vRight, &vRight);

		MyVec3 vDir = -vLook + vRight;
		D3DXVec3Normalize(&vDir, &vDir);

		m_iStateID = RunLower;
		m_pMeshCom->SetAnimation(m_iStateID);
		m_pTransformCom->Move((Engine::MyVec3*)&vDir, m_fSpeed * fDelta);	
	}
	else if (m_pKeyMgr->StayKeyDown('S') && m_iStateID < Teleport)
	{
		MyMat matView;
		m_pDevice->GetTransform(D3DTS_VIEW, &matView);
		D3DXMatrixInverse(&matView, 0 ,&matView);

		MyVec3 vLook;
		memcpy(&vLook, &matView.m[2][0], sizeof(MyVec3));
		vLook.y = 0.f;
		D3DXVec3Normalize(&vLook, &vLook);
		vLook *= -1.f;

		m_iStateID = RunLower;
		m_pMeshCom->SetAnimation(m_iStateID);
		m_pTransformCom->Move((Engine::MyVec3*)&vLook, m_fSpeed * fDelta);		
	}
	else if (m_pKeyMgr->StayKeyDown('A') && m_iStateID < Teleport)
	{
		MyMat matView;
		m_pDevice->GetTransform(D3DTS_VIEW, &matView);
		D3DXMatrixInverse(&matView, 0 ,&matView);

		MyVec3 vRight;
		memcpy(&vRight, &matView.m[0][0], sizeof(MyVec3));
		vRight.y = 0.f;
		D3DXVec3Normalize(&vRight, &vRight);
		vRight *= -1.f;

		m_iStateID = RunLower;
		m_pMeshCom->SetAnimation(m_iStateID);
		m_pTransformCom->Move((Engine::MyVec3*)&vRight, m_fSpeed * fDelta);		
	}
	else if (m_pKeyMgr->StayKeyDown('D') && m_iStateID < Teleport)
	{
		MyMat matView;
		m_pDevice->GetTransform(D3DTS_VIEW, &matView);
		D3DXMatrixInverse(&matView, 0 ,&matView);

		MyVec3 vRight;
		memcpy(&vRight, &matView.m[0][0], sizeof(MyVec3));
		vRight.y = 0.f;
		D3DXVec3Normalize(&vRight, &vRight);

		m_iStateID = RunLower;
		m_pMeshCom->SetAnimation(m_iStateID);
		m_pTransformCom->Move((Engine::MyVec3*)&vRight, m_fSpeed * fDelta);	
	}
	else if (m_iStateID == RunLower)
	{
		m_iStateID = BattleIdle;
		m_pMeshCom->SetAnimation(m_iStateID);
	}
}

void CPlayer::Command( const _float& fDelta )
{
	if (m_pKeyMgr->KeyDown('4') && m_iStateID <= Attack01)
	{
		m_iEffectCnt = 0;;
		m_iStateID = RuinBlade;
		m_pMeshCom->SetAnimation(m_iStateID);

		CTrailEffect* Trail = CTrailEffect::Create(m_pDevice);
		Engine::Get_Management()->GetLayer(L"Layer_Player")->AddGameObject(Trail);
		Trail->SetLifeTime(5.f);
		Trail->SetSaveTime(4.f);

		CAttachEffect* pEffect = CAttachEffect::Create(m_pDevice, L"Component_Texture_MagicCircle");
		Engine::Get_Management()->GetLayer(L"Layer_Player")->AddGameObject(pEffect);
		pEffect->SetLifeTime(3.f);
		pEffect->SetInformation(0, &MyVec3(0.8f, 0.8f, 0.8f));
		pEffect->SetInformation(1, (MyVec3*)m_pTransformCom->GetInfomation(Engine::CTransform::INFO_ANGLE));
		pEffect->SetInformation(2, (MyVec3*)&(*m_pTransformCom->GetInfomation(Engine::CTransform::INFO_POS)));

		pEffect = CAttachEffect::Create(m_pDevice, L"Component_Texture_MagicCircle");
		Engine::Get_Management()->GetLayer(L"Layer_Player")->AddGameObject(pEffect);
		pEffect->SetLifeTime(3.f);
		pEffect->SetInformation(0, &MyVec3(0.8f, 0.8f, 0.8f));
		pEffect->SetInformation(1, (MyVec3*)&(*m_pTransformCom->GetInfomation(Engine::CTransform::INFO_ANGLE) + MyVec3(0.f, D3DX_PI / 2.f, 0.f)));
		pEffect->SetInformation(2, (MyVec3*)&(*m_pTransformCom->GetInfomation(Engine::CTransform::INFO_POS)));
	}

	if (m_pKeyMgr->KeyDown('1') && m_iStateID <= SharpenCombo)
	{
		m_iStateID = SharpenCombo;
		m_pMeshCom->SetAnimation(m_iStateID);
		m_vPrevPos = *(MyVec3*)&m_pTransformCom->GetWorldMatrix()->m[3][0];

		MyVec3 vPos = *m_pTransformCom->GetInfomation(Engine::CTransform::INFO_POS);

		CSharpenCombo* pSharpen = CSharpenCombo::Create(m_pDevice, m_pTransformCom->GetWorldMatrix());
		Engine::Get_Management()->GetLayer(L"Layer_Player")->AddGameObject(pSharpen);
	}

	if (m_pKeyMgr->KeyDown('5') && m_iStateID <= Attack01)
	{
		m_iStateID = DarkNight;
		m_pMeshCom->SetAnimation(m_iStateID);
		m_vPrevPos = *(MyVec3*)&m_pTransformCom->GetWorldMatrix()->m[3][0];
		Engine::Get_CameraMgr()->SetCamera(L"Cam_Action");
	}

	if (m_pKeyMgr->KeyDown(VK_F3))
	{
		Engine::Get_CameraMgr()->SetCamera(L"Cam_Intro");
	}

	if (m_pKeyMgr->KeyDown(VK_SPACE))
	{
		m_iStateID = Teleport;
		m_pMeshCom->SetAnimation(m_iStateID, 0.7f, 1.1);

		MyVec3 vPos = *m_pTransformCom->GetInfomation(Engine::CTransform::INFO_POS);
		MyVec3 vLook = *(MyVec3*)&m_pTransformCom->GetWorldMatrix()->m[2][0];
		D3DXVec3Normalize(&vLook, &vLook);

		CCommonEffect* pEffect = CCommonEffect::Create(m_pDevice, L"Component_Texture_MagicCircle", 1);
		Engine::Get_Management()->GetLayer(L"Layer_Player")->AddGameObject(pEffect);
		pEffect->SetType(1);
		pEffect->SetLifeTime(0.5f);
		pEffect->SetInformation(0, &MyVec3(2.f, 2.f, 2.f));
		pEffect->SetInformation(1, (MyVec3*)m_pTransformCom->GetInfomation(Engine::CTransform::INFO_ANGLE));
		pEffect->SetInformation(2, &MyVec3(vPos + MyVec3(0.f, 3.f, 0.f)));

		pEffect = CCommonEffect::Create(m_pDevice, L"Component_Texture_Ring_Outer_Wind", 0);
		Engine::Get_Management()->GetLayer(L"Layer_Player")->AddGameObject(pEffect);
		pEffect->SetType(1);
		pEffect->SetLifeTime(0.5f);
		pEffect->SetInformation(0, &MyVec3(3.f, 3.f, 3.f));
		pEffect->SetInformation(1, (MyVec3*)m_pTransformCom->GetInfomation(Engine::CTransform::INFO_ANGLE));
		pEffect->SetInformation(2, &MyVec3(vPos + MyVec3(0.f, 3.f, 0.f)));

		pEffect = CCommonEffect::Create(m_pDevice, L"Component_Texture_SlashPoint");
		Engine::Get_Management()->GetLayer(L"Layer_Player")->AddGameObject(pEffect);
		pEffect->SetLifeTime(0.5f);
		pEffect->SetInformation(0, &MyVec3(2.f, 10.f, 2.f));
		pEffect->SetInformation(1, &MyVec3(D3DXToRadian(90.f), m_pTransformCom->GetInfomation(Engine::CTransform::INFO_ANGLE)->y, 0.f));
		pEffect->SetInformation(2, &MyVec3(vPos + MyVec3(0.f, 3.f, 0.f) - vLook));

		CTeleport* pTeleport = CTeleport::Create(m_pDevice, m_pTransformCom->GetWorldMatrix());
		Engine::Get_Management()->GetLayer(L"Layer_Player")->AddGameObject(pTeleport);

		pTeleport = CTeleport::Create(m_pDevice, m_pTransformCom->GetWorldMatrix(), 1);
		Engine::Get_Management()->GetLayer(L"Layer_Player")->AddGameObject(pTeleport);
	}

	if (m_pKeyMgr->KeyDown(VK_RETURN))
	{
		CManaStone* pEffect = CManaStone::Create(m_pDevice);
		Engine::Get_Management()->GetLayer(L"Layer_Player")->AddGameObject(pEffect);
	}

	if (m_pKeyMgr->KeyDown(VK_RBUTTON))
	{
		if (m_iStateID >= Attack01 && m_iStateID <= Attack05)
			return;

		if (m_iStateID != BattleIdle && m_pMeshCom->ProgressPerAnimation() < 0.35)
			return;

		CTrailEffect* pEffect = CTrailEffect::Create(m_pDevice);
		Engine::Get_Management()->GetLayer(L"Layer_Player")->AddGameObject(pEffect);

		if (m_iStateID != DoomSayer)
		{
			m_iStateID = DoomSayer;
			pEffect->SetSaveTime(0.5f);
			pEffect->SetLifeTime(0.5f);
			m_pMeshCom->SetAnimation(m_iStateID);
		}
		else
		{
			m_iStateID = DoomSayerCombo;
			pEffect->SetSaveTime(0.2f);
			pEffect->SetLifeTime(1.2f);
			m_pMeshCom->SetAnimation(m_iStateID, 1.5f);
		}
	}

	if (m_pKeyMgr->KeyDown(VK_LBUTTON))
	{
		if (m_iStateID != BattleIdle && m_pMeshCom->ProgressPerAnimation() < 0.25)
			return;

		if (m_pMeshCom->GetAniIndex() == Attack05 && m_pMeshCom->ProgressPerAnimation() < 0.6)
			return;

		CTrailEffect* pEffect = CTrailEffect::Create(m_pDevice);
		Engine::Get_Management()->GetLayer(L"Layer_Player")->AddGameObject(pEffect);

		if ((m_iStateID >= Attack01 && m_iStateID <= Attack05) == false)
		{
			m_iStateID = Attack01;
			pEffect->SetSaveTime(0.3f);
			pEffect->SetLifeTime(0.1f);
		}
		else if (m_iStateID == Attack01)
		{
			m_iStateID = Attack02;
			pEffect->SetSaveTime(0.3f);
			pEffect->SetLifeTime(0.1f);
		}
		else if (m_iStateID == Attack02)
		{
			m_iStateID = Attack03;
			pEffect->SetSaveTime(0.3f);
			pEffect->SetLifeTime(0.1f);
		}
		else if (m_iStateID == Attack03)
		{
			m_iStateID = Attack04;
			pEffect->SetSaveTime(0.8f);
			pEffect->SetLifeTime(0.6f);
		}
		else if (m_iStateID == Attack04)
		{
			m_iStateID = Attack05;
			pEffect->SetSaveTime(0.7f);
			pEffect->SetLifeTime(0.5f);
		}
		m_pMeshCom->SetAnimation(m_iStateID);
	}
}
void CPlayer::AnimationCheck( void )
{
	if (m_pMeshCom->CheckAnimation() == true)
	{
		if (m_iStateID == RunLower)
			return;

		if (m_iStateID == DarkNight)
		{
			Engine::Safe_Release(m_pMeshCom);
			m_pMeshCom = (Engine::CDynamicMesh*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Mesh_DarkNight");
		}

		m_iStateID = BattleIdle;
		m_pMeshCom->SetAnimation(BattleIdle);
	}
}

HRESULT CPlayer::ReadyComponent( void )
{
	m_pRendererCom = (Engine::CRenderer*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STATIC, L"Component_Renderer");
	if (m_pRendererCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(CGameObject::UPDATE_NONE, L"Com_Renderer", m_pRendererCom)))
		return E_FAIL;

	m_pTransformCom = (Engine::CTransform*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STATIC, L"Component_Transform");
	if (m_pTransformCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(CGameObject::UPDATE_DO, L"Com_Transform", m_pTransformCom)))
		return E_FAIL;	

	m_pMtrCom = (Engine::CMaterial*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STATIC, L"Component_Material");
	if (m_pMtrCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(CGameObject::UPDATE_NONE, L"Com_Material", m_pMtrCom)))
		return E_FAIL;

	m_pMeshCom = (Engine::CDynamicMesh*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Mesh_Player");
	if (m_pMeshCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(CGameObject::UPDATE_NONE, L"Com_Mesh", m_pMeshCom)))
		return E_FAIL;

	m_pCollisionCom = (Engine::CCollisionBox*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STATIC, L"Component_CollisionBox");
	if (m_pCollisionCom == NULL)
		return E_FAIL;
	if (FAILED(m_pCollisionCom->ComputeBox(m_pMeshCom, 0)))
		return E_FAIL;
	if (FAILED(AddComponent(CGameObject::UPDATE_NONE, L"Com_CollisionBox", m_pCollisionCom)))
		return E_FAIL;

	m_pShaderCom = (Engine::CShader*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Shader_Mesh");
	if (m_pShaderCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(Engine::CGameObject::UPDATE_NONE, L"Com_Shader", m_pShaderCom)))
		return E_FAIL;

	m_pBlurShaderCom = (Engine::CShader*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Shader_Blur");
	if (m_pBlurShaderCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(Engine::CGameObject::UPDATE_NONE, L"Com_BlurShader", m_pBlurShaderCom)))
		return E_FAIL;

	m_pShaderBufferCom = (Engine::CShaderBuffer*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Buffer_Shader");
	if (m_pShaderBufferCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(Engine::CGameObject::UPDATE_NONE, L"Com_ShaderBuffer", m_pShaderBufferCom)))
		return E_FAIL;

	return S_OK;
}

void CPlayer::Free( void )
{
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pMtrCom);
	Engine::Safe_Release(m_pMeshCom);
	Engine::Safe_Release(m_pCollisionCom);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pBlurShaderCom);
	Engine::Safe_Release(m_pShaderBufferCom); 
	Engine::Safe_Release(m_pSatellite);

	for (_uint i = 0; i < 2; ++i)
	{
		Engine::Safe_Release(m_pTargetTexture[i]);
		Engine::Safe_Release(m_pTargetSurface[i]);
	}

	Engine::CGameObject::Free();
}

CPlayer* CPlayer::Create( LPDIRECT3DDEVICE9 pDevice )
{
	CPlayer* pPlayer = new CPlayer(pDevice);

	if (FAILED(pPlayer->Initialize()))
	{
		MSG_BOX("Player Created Failed");
		Engine::Safe_Release(pPlayer);
	}
	return pPlayer;
}


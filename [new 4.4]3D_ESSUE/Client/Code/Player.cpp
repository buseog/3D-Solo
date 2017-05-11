#include "StdAfx.h"
#include "Player.h"

#include "TrailEffect.h"
#include "Satellite.h"
#include "DiffusionShift.h"
#include "SharpenCombo.h"
#include "ParticleBuffer.h"
#include "Dust.h"
#include "DoomSayer.h"
#include "RuinBlade.h"
#include "DefaultCamera.h"
#include "ManaBlade.h"
#include "RuinWave.h"
#include "BladeEffect.h"
#include "Damage.h"
#include "LongBlade.h"
#include "DarkKnight.h"
#include "LordOfMana.h"
#include "FistOfBlade.h"
#include "Boss.h"
#include "SoundMgr.h"

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
, m_bManaBlade(false)
, m_bRim(true)
{
	ZeroMemory(m_bSkill, sizeof(_bool) * 10);
	ZeroMemory(m_vColor, sizeof(MyVec4));
}

CPlayer::~CPlayer( void )
{

}

HRESULT CPlayer::Initialize( void )
{
	if (FAILED(ReadyComponent()))
		return E_FAIL;

	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_SCALE, &Engine::MyVec3(0.1f, 0.1f, 0.1f));
	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_POS, &Engine::MyVec3(0.f, -3.5f, 500.f));
	m_pMeshCom->SetAnimation(0);

	m_fHp = 1000.f;
	m_fFullHp = m_fHp;
	m_fAttack = 50.f;
	m_fSpeed = 20.f;

	return S_OK;
}

_int CPlayer::Update( const _float& fDelta )
{
	KeyCheck(fDelta);
	Active(fDelta);

	m_fBladeTime -= fDelta;
	if (m_fBladeTime < 0.f)
	{
		((CLongBlade*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(1))->SetColliderSize(0);
		m_bManaBlade = false;
	}

	m_vColor.x -= 3.f * fDelta;
	if (m_vColor.x <= 0.f)
		m_vColor.x = 0.f;

	Collider(fDelta);

	m_pMeshCom->PlayAnimation(fDelta);
	AnimationCheck();

	Engine::CGameObject::Update(fDelta);
	m_pRendererCom->AddRenderList(Engine::CRenderer::RENDER_NONEALPHA, this);

	return 0;
}

void CPlayer::Render( void )
{
	LPD3DXEFFECT	pEffect = m_pShaderCom->GetEffect();
	if (pEffect == NULL)
		return;
	SetUpShader(pEffect);

	
	m_pCollisionCom->Render(Engine::CCollisionBox::COLL_OBB, m_pTransformCom->GetWorldMatrix());
	/*_tchar szBuf[128] = L"";
	swprintf_s(szBuf, L"%f", m_pMeshCom->ProgressPerAnimation());
	Engine::Get_FontMgr()->Render(L"Font_Batang", szBuf, MyVec3(500.f, 0.f, 0.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

	MyVec3 vPos = *m_pTransformCom->GetInfomation(Engine::CTransform::INFO_POS);
	swprintf_s(szBuf, L"x : %f", vPos.x);
	Engine::Get_FontMgr()->Render(L"Font_Batang", szBuf, MyVec3(250.f, 0.f, 0.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	swprintf_s(szBuf, L"y : %f", vPos.y);
	Engine::Get_FontMgr()->Render(L"Font_Batang", szBuf, MyVec3(250.f, 20.f, 0.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	swprintf_s(szBuf, L"z : %f", vPos.z);
	Engine::Get_FontMgr()->Render(L"Font_Batang", szBuf, MyVec3(250.f, 40.f, 0.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));*/

}

void CPlayer::SetUpShader( LPD3DXEFFECT pEffect )
{
	pEffect->AddRef();

	MyMat matLightView;
	MyVec3 vPos = MyVec3(80.f, 120.f, 120.f);
	MyVec3 vDir = *m_pTransformCom->GetInfomation(Engine::CTransform::INFO_POS);
	D3DXMatrixLookAtLH(&matLightView, &vPos, &vDir, &MyVec3(0.f, 1.f, 0.f));
	pEffect->SetMatrix("g_matLight", &matLightView);

	D3DLIGHT9	LightInfo;
	ZeroMemory(&LightInfo, sizeof(D3DLIGHT9));
	m_pDevice->GetLight(0, &LightInfo);

	pEffect->SetVector("g_vLightDir", (MyVec4*)&LightInfo.Direction);
	pEffect->SetVector("g_vLightDiffuse", (MyVec4*)&LightInfo.Diffuse);
	pEffect->SetVector("g_vLightAmbient", (MyVec4*)&LightInfo.Ambient);
	pEffect->SetVector("g_vLightSpecular", (MyVec4*)&LightInfo.Specular);
	pEffect->SetVector("g_vCamPos", (MyVec4*)Engine::Get_CameraMgr()->GetCameraEye());
	pEffect->SetVector("g_vColor", &m_vColor);

	MyMat matView, matProj;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);
	pEffect->SetVector("g_vColor", &m_vColor);

	m_pTransformCom->SetUpShader(pEffect);
	m_pMtrCom->SetUpShader(pEffect);

	Engine::Get_TargetMgr()->EndMRT(L"MRT_Deferred");
	Engine::Get_TargetMgr()->GetTarget(L"Target_Shadow")->ClearTarget();
	Engine::Get_TargetMgr()->GetTarget(L"Target_Shadow")->SetUpDevice(0);
	m_pMeshCom->SetUpShader(pEffect, 4);
	Engine::Get_TargetMgr()->GetTarget(L"Target_Shadow")->ResetDevice(0);
	Engine::Get_TargetMgr()->BeginMRT(L"MRT_Deferred", 1);

	if (m_bRim == true)
		m_pMeshCom->SetUpShader(pEffect, 1);
	else
		m_pMeshCom->SetUpShader(pEffect, 6);

	Engine::Safe_Release(pEffect);
}

void CPlayer::KeyCheck( const _float& fDelta )
{
	if (fDelta == 0.f)
		return;

	if (m_pKeyMgr->KeyDown(VK_F9))
	{
		if (m_bRim == true)
			m_bRim = false;
		else
			m_bRim = true;
	}

	Move(fDelta);
	Command(fDelta);
}

void CPlayer::Active( const _float& fDelta )
{
	if (m_iStateID >= Attack01 && m_iStateID <= SharpenMana06)
	{
		if (m_bSkill[0] == false)
		{
			switch (m_iStateID)
			{
			case Attack01:
				if (m_pMeshCom->ProgressAnimation() > 0.4)
				{
					((CLongBlade*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(1))->SetCollision();
					CSoundMgr::GetInstance()->PlaySound(L"arisha_attack_01.wav", 0);
					CSoundMgr::GetInstance()->PlaySound(L"LongBlade_01.wav", 1);
					m_bSkill[0] = true;
				}
				break;

			case Attack02:
				if (m_pMeshCom->ProgressAnimation() > 0.2)
				{
					((CLongBlade*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(1))->SetCollision();
					CSoundMgr::GetInstance()->PlaySound(L"arisha_attack_02.wav", 0);
					CSoundMgr::GetInstance()->PlaySound(L"LongBlade_02.wav", 1);
					m_bSkill[0] = true;
				}
				break;

			case Attack03:
				if (m_pMeshCom->ProgressAnimation() > 0.4)
				{
					((CLongBlade*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(1))->SetCollision();
					CSoundMgr::GetInstance()->PlaySound(L"arisha_attack_03.wav", 0);
					CSoundMgr::GetInstance()->PlaySound(L"LongBlade_03.wav", 1);
					m_bSkill[0] = true;
				}
				break;

			case Attack04:
				if (m_pMeshCom->ProgressAnimation() > 0.9)
				{
					((CLongBlade*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(1))->SetCollision();
					CSoundMgr::GetInstance()->PlaySound(L"arisha_attack_04.wav", 0);
					CSoundMgr::GetInstance()->PlaySound(L"LongBlade_04.wav", 1);
					m_bSkill[0] = true;
				}
				break;

			case Attack05:
				if (m_pMeshCom->ProgressAnimation() > 0.8)
				{
					((CLongBlade*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(1))->SetCollision();
					CSoundMgr::GetInstance()->PlaySound(L"arisha_attack_05.wav", 0);
					CSoundMgr::GetInstance()->PlaySound(L"LongBlade_05.wav", 1);
					m_bSkill[0] = true;
				}
				break;

			case SharpenMana01:
				if (m_pMeshCom->ProgressAnimation() > 0.2)
				{
					((CLongBlade*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(1))->SetCollision(1);
					CSoundMgr::GetInstance()->PlaySound(L"ManaBlade01.wav", 1);
					m_bSkill[0] = true;
				}
				break;

			case SharpenMana02:
				if (m_pMeshCom->ProgressAnimation() > 0.2)
				{
					((CLongBlade*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(1))->SetCollision(1);
					CSoundMgr::GetInstance()->PlaySound(L"ManaBlade02.wav", 1);
					m_bSkill[0] = true;
				}
				break;

			case SharpenMana03:
				if (m_pMeshCom->ProgressAnimation() > 0.2)
				{
					((CLongBlade*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(1))->SetCollision(1);
					CSoundMgr::GetInstance()->PlaySound(L"ManaBlade03.wav", 1);
					m_bSkill[0] = true;
				}
				break;

			case SharpenMana04:
				if (m_pMeshCom->ProgressAnimation() > 0.2)
				{
					((CLongBlade*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(1))->SetCollision(1);
					CSoundMgr::GetInstance()->PlaySound(L"ManaBlade04.wav", 1);
					m_bSkill[0] = true;
				}
				break;

			case SharpenMana05:
				if (m_pMeshCom->ProgressAnimation() > 0.2)
				{
					((CLongBlade*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(1))->SetCollision(1);
					CSoundMgr::GetInstance()->PlaySound(L"ManaBlade05.wav", 1);
					m_bSkill[0] = true;
				}
				break;

			case SharpenMana06:
				if (m_pMeshCom->ProgressAnimation() > 0.2)
				{
					((CLongBlade*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(1))->SetCollision(1);
					CSoundMgr::GetInstance()->PlaySound(L"ManaBlade01.wav", 1);
					m_bSkill[0] = true;
				}
				break;
			}
		}
	}

	if (m_iStateID == ManaBlade)
	{
		if (m_pMeshCom->ProgressPerAnimation() > 0.12 && m_bSkill[0] == false)
		{
			CManaBlade* pEffect = NULL;
			for (int i = 10; i < 14; ++i)
			{
				pEffect = CManaBlade::Create(m_pDevice, i, m_pTransformCom->GetWorldMatrix(), m_pMeshCom->GetPartsMatrix("ValveBiped_Anim_Attachment_LH"));
				Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
			}
			for (int i = 14; i < 17; ++i)
			{
				pEffect = CManaBlade::Create(m_pDevice, i, m_pTransformCom->GetWorldMatrix(), m_pMeshCom->GetPartsMatrix("ValveBiped_Anim_Attachment_RH"));
				Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
			}
			m_bSkill[0] = true;
		}

		if (m_pMeshCom->ProgressPerAnimation() > 0.4 && m_bSkill[1] == false)
		{
			for (_int i = 0; i < 4; ++i)
			{
				CManaBlade* pEffect = CManaBlade::Create(m_pDevice, i, m_pTransformCom->GetWorldMatrix(), m_pMeshCom->GetPartsMatrix("ValveBiped_Bip01_L_Hand"));
				Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
			}

			for (_int i = 4; i < 7; ++i)
			{
				CBladeEffect* pEffect = CBladeEffect::Create(m_pDevice, i);
				Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
			}
			m_bSkill[1] = true;
		}
	}
	if (m_iStateID == RuinBlade)
	{
		if (m_pMeshCom->ProgressPerAnimation() > 0.11 && m_bSkill[0] == false)
		{
			for (int i = 0; i < 3; ++i)
			{
				CRuinBlade* pEffect = CRuinBlade::Create(m_pDevice, i, m_pTransformCom->GetWorldMatrix(), m_pMeshCom->GetPartsMatrix("ValveBiped_Bip01_L_Hand"));
				Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
			}
			for (int i = 4; i < 9; ++i)
			{
				CRuinBlade* pEffect = CRuinBlade::Create(m_pDevice, i, m_pTransformCom->GetWorldMatrix(), m_pMeshCom->GetPartsMatrix("ValveBiped_Bip01_R_Hand"));
				Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
			}
			CSoundMgr::GetInstance()->PlaySound(L"RuinBlade_Encahnt.wav", 1);
			m_bSkill[0] = true;			
		}
		if (m_pMeshCom->ProgressPerAnimation() > 0.41 && m_bSkill[1] == false)
		{
			for (int i = 10; i < 12; ++i)
			{
				CRuinBlade* pEffect = CRuinBlade::Create(m_pDevice, i, m_pTransformCom->GetWorldMatrix(), m_pMeshCom->GetPartsMatrix("ValveBiped_Bip01_R_Toe0"));
				Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
			}

			for (int i = 20; i < 23; ++i)
			{
				CRuinBlade* pEffect = CRuinBlade::Create(m_pDevice, i, m_pTransformCom->GetWorldMatrix(), m_pMeshCom->GetPartsMatrix("ValveBiped_Bip01_R_Toe0"));
				Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
			}
			CSoundMgr::GetInstance()->PlaySound(L"RuinBlade_Slash.wav", 1);
			m_bSkill[1] = true;
		}
		if (m_pMeshCom->ProgressPerAnimation() > 0.42 && m_bSkill[2] == false)
		{
			for (int i = 40; i < 47; ++i)
			{
				CRuinBlade* pEffect = CRuinBlade::Create(m_pDevice, i, m_pTransformCom->GetWorldMatrix(), m_pMeshCom->GetPartsMatrix("ValveBiped_Bip01_Spine2"));
				Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
			}

			for (int i = 0; i < 3; ++i)
			{
				CRuinWave* pEffect = CRuinWave::Create(m_pDevice, i, m_pTransformCom->GetWorldMatrix(), m_pMeshCom->GetPartsMatrix("ValveBiped_Bip01_R_Toe0"));
				Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
			}

			for (int i = 10; i < 14; ++i)
			{
				CRuinWave* pEffect = CRuinWave::Create(m_pDevice, i, m_pTransformCom->GetWorldMatrix(), m_pMeshCom->GetPartsMatrix("ValveBiped_Bip01_R_Toe0"));
				Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
			}

			for (int i = 20; i < 27; ++i)
			{
				CRuinWave* pEffect = CRuinWave::Create(m_pDevice, i, m_pTransformCom->GetWorldMatrix(), m_pMeshCom->GetPartsMatrix("ValveBiped_Bip01_R_Toe0"));
				Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
			}
			CSoundMgr::GetInstance()->PlaySound(L"RuinBlade_Wave.wav", 4);
			m_bSkill[2] = true;
		}
		if (m_pMeshCom->ProgressPerAnimation() > 0.44 && m_bSkill[3] == false)
		{
			CSoundMgr::GetInstance()->PlaySound(L"RuinBlade_Wave.wav", 5);
			CSoundMgr::GetInstance()->PlaySound(L"env_campfire1.wav", 7);
			m_bSkill[3] = true;
		}

		if (m_pMeshCom->ProgressPerAnimation() > 0.44 && m_bSkill[4] == false)
		{
			CSoundMgr::GetInstance()->PlaySound(L"RuinBlade_Wave.wav", 6);
			CSoundMgr::GetInstance()->PlaySound(L"env_campfire1.wav", 8);
			m_bSkill[4] = true;
		}
		
	}

	if (m_iStateID == SharpenCombo)
	{
		MyVec3 vMove = *(MyVec3*)&m_pMeshCom->GetPartsMatrix("ValveBiped_Anim_Attachment_GRAPPLING")->m[3][0];
		MyMat matScale, matRot, matTrans;
		D3DXMatrixScaling(&matScale, 0.1f, 0.1f, 0.1f);
		D3DXMatrixRotationY(&matRot, m_pTransformCom->GetInfomation(Engine::CTransform::INFO_ANGLE)->y);
		D3DXVec3TransformCoord(&vMove, &vMove, &(matScale * matRot));
		vMove.x *= -1.f;
		vMove.z *= -1.f;

		m_pTransformCom->SetInfomation(Engine::CTransform::INFO_POS, &(m_vPrevPos + vMove));

		if (m_pMeshCom->ProgressPerAnimation() > 0.21 && m_bSkill[0] == false)
		{
			((CDefaultCamera*)Engine::Get_CameraMgr()->GetCamera(L"Cam_Default"))->ActiveBone(true);

			CSharpenCombo* pEffect = NULL;
			for (int i = 10; i < 15; ++i)
			{
				pEffect = CSharpenCombo::Create(m_pDevice, i, m_pTransformCom->GetWorldMatrix(), m_pMeshCom->GetPartsMatrix("ValveBiped_Bip01_R_ForeTwist1"));
				Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
			}
			for (int i = 15; i < 17; ++i)
			{
				pEffect = CSharpenCombo::Create(m_pDevice, i, m_pTransformCom->GetWorldMatrix(), m_pMeshCom->GetPartsMatrix("ValveBiped_Bip01_L_Hand"));
				Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
			}
			CSoundMgr::GetInstance()->PlaySound(L"arisha_attack_strong_01.wav", 0);
			m_bSkill[0] = true;
		}
		if (m_pMeshCom->ProgressPerAnimation() > 0.19 && m_bSkill[5] == false)
		{
			CSoundMgr::GetInstance()->PlaySound(L"arisha_skill_cast_mana_teleport_01.wav", 1);
			m_bSkill[5] = true;
		}
		if (m_pMeshCom->ProgressPerAnimation() > 0.24 && m_bSkill[1] == false)
		{
			m_pRendererCom->SetGaussian(true);

			for (int i = 20; i < 26; ++i)
			{
				CSharpenCombo* pEffect = CSharpenCombo::Create(m_pDevice, i, m_pTransformCom->GetWorldMatrix(), m_pMeshCom->GetPartsMatrix("ValveBiped_Bip01_R_ForeTwist1"));
				Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
			}
			((CLongBlade*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(1))->SetCollision(1);
			CSoundMgr::GetInstance()->PlaySound(L"arisha_skill_cast_mana_teleport_02.wav", 1);
			m_bSkill[1] = true;
		}
		if (m_pMeshCom->ProgressPerAnimation() > 0.31 && m_bSkill[2] == false)
		{
			for (int i = 0; i < 3; ++i)
			{
				CDust*	pEffect = CDust::Create(m_pDevice, i, m_pTransformCom->GetWorldMatrix(), m_pMeshCom->GetPartsMatrix("ValveBiped_Bip01_R_Foot"));
				Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
			}
			m_bSkill[2] = true;
		}
		if (m_pMeshCom->ProgressPerAnimation() > 0.34 && m_bSkill[3] == false)
		{
			for (int i = 0; i < 3; ++i)
			{
				CDust* pEffect = CDust::Create(m_pDevice, i, m_pTransformCom->GetWorldMatrix(), m_pMeshCom->GetPartsMatrix("ValveBiped_Bip01_R_Foot"));
				Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
			}
			m_bSkill[3] = true;
		}
		if (m_pMeshCom->ProgressPerAnimation() > 0.36 && m_bSkill[4] == false)
		{
			((CDefaultCamera*)Engine::Get_CameraMgr()->GetCamera(L"Cam_Default"))->ActiveBone(false);
			m_pRendererCom->SetGaussian(false);

			for (int i = 10; i < 13; ++i)
			{
				CDust* pEffect = CDust::Create(m_pDevice, i, m_pTransformCom->GetWorldMatrix(), m_pMeshCom->GetPartsMatrix("ValveBiped_Bip01_L_Foot"));
				Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
			}
			m_bSkill[4] = true;
		}
	}

	if (m_iStateID == DiffusionShift)
	{
		if (m_pMeshCom->ProgressAnimation() > 0.1 && m_pMeshCom->ProgressAnimation() < 1.0)
		{
			MyVec3 vLook = *(MyVec3*)&m_pTransformCom->GetWorldMatrix()->m[2][0];
			D3DXVec3Normalize(&vLook, &vLook);
			m_pTransformCom->Move((Engine::MyVec3*)&vLook, m_fSpeed * fDelta * 3.f);
		}

		if (m_pMeshCom->ProgressAnimation() >= 0.3 && m_bSkill[0] == false)
		{
			for (int i = 0; i < 7; ++i)
			{
				CDiffusionShift* pEffect = CDiffusionShift::Create(m_pDevice, i, m_pTransformCom->GetWorldMatrix() ,m_pMeshCom->GetPartsMatrix("ValveBiped_Bip01_L_Hand"));
				Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
			}
			CSoundMgr::GetInstance()->PlaySound(L"DiffusionShift.wav", 1);
			m_bSkill[0] = true;
		}

		if (m_pMeshCom->ProgressAnimation() > 0.5 && m_bSkill[1] == false)
		{
			for (int i = 0; i < 3; ++i)
			{
				CDust* pEffect = CDust::Create(m_pDevice, i, m_pTransformCom->GetWorldMatrix(), m_pMeshCom->GetPartsMatrix("ValveBiped_Bip01_R_Foot"));
				Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
			}
			m_bSkill[1] = true;
		}

		if (m_pMeshCom->ProgressAnimation() > 0.6 && m_bSkill[2] == false)
		{
			for (int i = 0; i < 3; ++i)
			{
				CDust* pEffect = CDust::Create(m_pDevice, i, m_pTransformCom->GetWorldMatrix(), m_pMeshCom->GetPartsMatrix("ValveBiped_Bip01_L_Foot"));
				Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
			}
			m_bSkill[2] = true;
		}
	}

	if (m_iStateID == DoomSayer)
	{
		MyVec3 vLook = *(MyVec3*)&m_pTransformCom->GetWorldMatrix()->m[2][0];
		D3DXVec3Normalize(&vLook, &vLook);

		if (m_pMeshCom->ProgressAnimation() > 0.2 && m_pMeshCom->ProgressAnimation() < 0.6)
			m_pTransformCom->Move((Engine::MyVec3*)&vLook, m_fSpeed * fDelta * 0.4f);

		else if (m_pMeshCom->ProgressAnimation() > 0.7 && m_pMeshCom->ProgressAnimation() < 1.2)
			m_pTransformCom->Move((Engine::MyVec3*)&vLook, m_fSpeed * fDelta * 1.1f);

		if (m_pMeshCom->ProgressPerAnimation() > 0.15 && m_bSkill[0] == false)
		{
			for (_int i = 0; i < 3; ++i)
			{
				CDust* pEffect = CDust::Create(m_pDevice, i, m_pTransformCom->GetWorldMatrix(), m_pMeshCom->GetPartsMatrix("ValveBiped_Bip01_R_Foot"));
				Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
			}
			m_bSkill[0] = true;
		}

		if (m_pMeshCom->ProgressPerAnimation() > 0.2 && m_bSkill[1] == false)
		{
			for (_int i = 0; i < 2; ++i)
			{
				CDoomSayer* pEffect = CDoomSayer::Create(m_pDevice, i, m_pTransformCom->GetWorldMatrix(), m_pMeshCom->GetPartsMatrix("ValveBiped_Bip01_R_Toe0"));
				Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
			}
			((CLongBlade*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(1))->SetCollision(1);
			CSoundMgr::GetInstance()->PlaySound(L"arisha_attack_strong_07.wav", 0);
			CSoundMgr::GetInstance()->PlaySound(L"LongBlade_05.wav", 1);
			m_bSkill[1] = true;
		}

		if (m_pMeshCom->ProgressPerAnimation() > 0.2 && m_bSkill[2] == false)
		{
			for (int i = 0; i < 3; ++i)
			{
				CDust* pEffect = CDust::Create(m_pDevice, i, m_pTransformCom->GetWorldMatrix(), m_pMeshCom->GetPartsMatrix("ValveBiped_Bip01_L_Foot"));
				Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
			}
			m_bSkill[2] = true;
		}
	}
	if (m_iStateID == DoomSayerCombo)
	{
		MyVec3 vLook = *(MyVec3*)&m_pTransformCom->GetWorldMatrix()->m[2][0];
		D3DXVec3Normalize(&vLook, &vLook);

		if (m_pMeshCom->ProgressAnimation() > 0.2 && m_pMeshCom->ProgressAnimation() < 1.6)
			m_pTransformCom->Straight((Engine::MyVec3*)&vLook, m_fSpeed * fDelta * 0.8f);

		else if (m_pMeshCom->ProgressAnimation() > 1.6 && m_pMeshCom->ProgressAnimation() < 2.8)
			m_pTransformCom->Straight((Engine::MyVec3*)&vLook, m_fSpeed * fDelta * 0.2f);

		if (m_pMeshCom->ProgressPerAnimation() > 0.08 && m_bSkill[0] == false)
		{
			for (int i = 0; i < 3; ++i)
			{
				CDust* pEffect = CDust::Create(m_pDevice, i, m_pTransformCom->GetWorldMatrix(), m_pMeshCom->GetPartsMatrix("ValveBiped_Bip01_L_Foot"));
				Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
			}
			m_bSkill[0] = true;
		}
		if (m_pMeshCom->ProgressPerAnimation() > 0.11 && m_bSkill[1] == false)
		{
			for (_int i = 10; i < 18; ++i)
			{
				CDoomSayer* pEffect = CDoomSayer::Create(m_pDevice, i, m_pTransformCom->GetWorldMatrix(), m_pMeshCom->GetPartsMatrix("ValveBiped_Bip01_R_Foot"));
				Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
			}
			CSoundMgr::GetInstance()->PlaySound(L"arisha_attack_strong_06.wav", 0);
			m_bSkill[1] = true;
		}
		if (m_pMeshCom->ProgressPerAnimation() > 0.28 && m_bSkill[2] == false)
		{
			for (_int i = 2; i < 4; ++i)
			{
				CDoomSayer* pEffect = CDoomSayer::Create(m_pDevice, i, m_pTransformCom->GetWorldMatrix(), m_pMeshCom->GetPartsMatrix("ValveBiped_Bip01_R_Toe0"));
				Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
			}
			((CLongBlade*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(1))->SetCollision(1);
			CSoundMgr::GetInstance()->PlaySound(L"LongBlade_06.wav", 1);
			m_bSkill[2] = true;
		}

		if (m_pMeshCom->ProgressPerAnimation() > 0.32 && m_bSkill[3] == false)
		{
			CSoundMgr::GetInstance()->PlaySound(L"LongBlade_04.wav", 4);
			m_bSkill[3] = true;
		}

		if (m_pMeshCom->ProgressPerAnimation() > 0.36 && m_bSkill[4] == false)
		{
			for (int i = 0; i < 3; ++i)
			{
				CDust* pEffect = CDust::Create(m_pDevice, i, m_pTransformCom->GetWorldMatrix(), m_pMeshCom->GetPartsMatrix("ValveBiped_Bip01_L_Foot"));
				Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
			}
			m_bSkill[4] = true;
		}
	}

	if (m_iStateID == LordOfMana)
	{
		if (m_pMeshCom->ProgressPerAnimation() > 0.04 && m_bSkill[4] == false)
		{
			CSoundMgr::GetInstance()->PlaySound(L"LordOfMana_Begin.wav", 1);
			m_bSkill[4] = true;
		}

		if (m_pMeshCom->ProgressPerAnimation() > 0.2 && m_bSkill[5] == false)
		{
			CSoundMgr::GetInstance()->PlaySound(L"LordOfMana_During.wav", 1);
			m_bSkill[5] = true;
		}

		if (m_pMeshCom->ProgressPerAnimation() > 0.67 && m_bSkill[6] == false)
		{
			CSoundMgr::GetInstance()->PlaySound(L"LordOfMana_End.wav", 1);
			m_bSkill[6] = true;
		}

		if (m_pMeshCom->ProgressPerAnimation() > 0.05 && m_bSkill[0] == false)
		{
			for (_uint i = 0; i < 10; ++i)
			{
				CLordOfMana* pEffect = CLordOfMana::Create(m_pDevice, i);
				Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
			}
			m_bSkill[0] = true;
		}

		
		if (m_pMeshCom->ProgressPerAnimation() > 0.27 && m_bSkill[1] == false)
		{
			for (_uint i = 10; i < 24; ++i)
			{
				CLordOfMana* pEffect = CLordOfMana::Create(m_pDevice, i);
				Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
			}
			CSoundMgr::GetInstance()->PlaySound(L"arisha_attack_strong_03.wav", 0);
			m_bSkill[1] = true;
		}

		if (m_pMeshCom->ProgressPerAnimation() > 0.52 && m_bSkill[2] == false)
		{
			for (_uint i = 30; i < 36; ++i)
			{
				CLordOfMana* pEffect = CLordOfMana::Create(m_pDevice, i);
				Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
			}
			CSoundMgr::GetInstance()->PlaySound(L"arisha_skill_resonance_hand_during.wav", 1);
			m_bSkill[2] = true;
		}

		if (m_pMeshCom->ProgressPerAnimation() > 0.68 && m_bSkill[3] == false)
		{
			for (_uint i = 40; i < 45; ++i)
			{
				CLordOfMana* pEffect = CLordOfMana::Create(m_pDevice, i);
				Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
			}
			((CBoss*)Engine::Get_Management()->GetLayer(L"Layer_Boss")->GetObject(0))->SetDamage(300.f);
			m_bSkill[3] = true;
		}
	}

	/*if (m_iStateID == DarkKnight)
	{
		if (m_pMeshCom->ProgressPerAnimation() > 0.04 && m_bSkill[0] == false)
		{
			for (_uint i = 5; i < 9; ++i)
			{
				CDarkKnight* pEffect = CDarkKnight::Create(m_pDevice, i);
				Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
			}
			m_bSkill[0] = true;
		}

		if (m_pMeshCom->ProgressPerAnimation() > 0.61 && m_bSkill[1] == false)
		{
			for (_uint i = 10; i < 13; ++i)
			{
				CDarkKnight* pEffect = CDarkKnight::Create(m_pDevice, i);
				Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
			}
			m_bSkill[1] = true;
		}
	}*/
}

void CPlayer::Move( const _float& fDelta )
{
	if (m_pKeyMgr->StayKeyDown('W') && m_pKeyMgr->StayKeyDown('A') && m_iStateID < DiffusionShift)
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
	else if (m_pKeyMgr->StayKeyDown('W') && m_pKeyMgr->StayKeyDown('D') && m_iStateID < DiffusionShift)
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
	else if (m_pKeyMgr->StayKeyDown('W') && m_iStateID < DiffusionShift)
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
	else if (m_pKeyMgr->StayKeyDown('S') && m_pKeyMgr->StayKeyDown('A') && m_iStateID < DiffusionShift)
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
	else if (m_pKeyMgr->StayKeyDown('S') && m_pKeyMgr->StayKeyDown('D') && m_iStateID < DiffusionShift)
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
	else if (m_pKeyMgr->StayKeyDown('S') && m_iStateID < DiffusionShift)
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
	else if (m_pKeyMgr->StayKeyDown('A') && m_iStateID < DiffusionShift)
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
	else if (m_pKeyMgr->StayKeyDown('D') && m_iStateID < DiffusionShift)
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
	if (m_pKeyMgr->KeyDown(VK_TAB))
	{
		ZeroMemory(m_bSkill, sizeof(_bool) * 10);

		m_iStateID = ManaBlade;
		m_pMeshCom->SetAnimation(m_iStateID);
		m_bManaBlade = true;
		m_fBladeTime = 10.f;

		((CLongBlade*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(1))->SetColliderSize(1);
		CSoundMgr::GetInstance()->PlaySound(L"ManaBlade_On.wav", 1);
	}

	if (m_pKeyMgr->KeyDown('1') && m_iStateID <= SharpenCombo)
	{
		ZeroMemory(m_bSkill, sizeof(_bool) * 10);

		m_iStateID = SharpenCombo;
		m_pMeshCom->SetAnimation(m_iStateID);
		m_vPrevPos = *(MyVec3*)&m_pTransformCom->GetWorldMatrix()->m[3][0];

		CSharpenCombo* pEffect = NULL;
		for (int i = 0; i < 6; ++i)
		{
			pEffect = CSharpenCombo::Create(m_pDevice, i, m_pTransformCom->GetWorldMatrix(), m_pMeshCom->GetPartsMatrix("ValveBiped_Bip01"));
			Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
		}
		CSoundMgr::GetInstance()->PlaySound(L"arisha_skill_cast_mana_teleport_01.wav", 1);
	}

	if (m_pKeyMgr->KeyDown('2') && m_iStateID <= Attack01)
	{
		ZeroMemory(m_bSkill, sizeof(_bool) * 10);

		m_iStateID = RuinBlade;
		m_pMeshCom->SetAnimation(m_iStateID);
	}

	if (m_pKeyMgr->KeyDown('3') && m_iStateID <= Attack01)
	{
		ZeroMemory(m_bSkill, sizeof(_bool) * 10);

		m_iStateID = LordOfMana;
		m_pMeshCom->SetAnimation(m_iStateID);
		Engine::Get_CameraMgr()->SetCamera(L"Cam_Action");
		CSoundMgr::GetInstance()->PlaySound(L"arisha_attack_01.wav", 0);
	}

	if (m_pKeyMgr->KeyDown('4') && m_iStateID <= Attack01)
	{
		ZeroMemory(m_bSkill, sizeof(_bool) * 10);

		m_iStateID = DarkKnight;
		m_pMeshCom->SetAnimation(m_iStateID);
		m_vPrevPos = *(MyVec3*)&m_pTransformCom->GetWorldMatrix()->m[3][0];
		Engine::Get_CameraMgr()->SetCamera(L"Cam_Action");

		for (_uint i = 0; i < 2; ++i)
		{
			CDarkKnight* pEffect = CDarkKnight::Create(m_pDevice, i);
			Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
		}
	}

	if (m_pKeyMgr->KeyDown(VK_SPACE))
	{
		ZeroMemory(m_bSkill, sizeof(_bool) * 10);
		m_iStateID = DiffusionShift;
		m_pMeshCom->SetAnimation(m_iStateID , 1.8f);
	}

	if (m_pKeyMgr->KeyDown(VK_RBUTTON))
	{
		if (m_iStateID >= Attack01 && m_iStateID <= Attack05)
			return;

		if (m_iStateID != BattleIdle && m_pMeshCom->ProgressPerAnimation() < 0.35)
			return;

		ZeroMemory(m_bSkill, sizeof(_bool) * 10);

		CTrailEffect* pEffect = CTrailEffect::Create(m_pDevice);
		Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);

		if (m_iStateID != DoomSayer)
		{
			m_iStateID = DoomSayer;
			pEffect->SetSaveTime(0.5f);
			pEffect->SetLifeTime(0.1f);
			m_pMeshCom->SetAnimation(m_iStateID);
		}
		else
		{
			m_iStateID = DoomSayerCombo;
			pEffect->SetSaveTime(0.5f);
			pEffect->SetLifeTime(0.7f);
			m_pMeshCom->SetAnimation(m_iStateID, 1.5f);
		}
	}

	if (m_pKeyMgr->KeyDown(VK_LBUTTON))
	{
		if (m_bManaBlade == true)
		{
			if (m_iStateID != BattleIdle && m_pMeshCom->ProgressPerAnimation() < 0.1)
				return;

			CTrailEffect* pEffect = CTrailEffect::Create(m_pDevice);
			Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);

			if ((m_iStateID >= SharpenMana01 && m_iStateID <= SharpenMana06) == false)
			{
				m_iStateID = SharpenMana01;
				ZeroMemory(m_bSkill, sizeof(_bool) * 10);

				for (_uint i = 0; i < 2; ++i)
				{
					CFistOfBlade* pTemp = CFistOfBlade::Create(m_pDevice, i);
					Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pTemp);
				}
			}
			else if (m_iStateID == SharpenMana01)
			{
				m_iStateID = SharpenMana02;
				ZeroMemory(m_bSkill, sizeof(_bool) * 10);

				for (_uint i = 2; i < 4; ++i)
				{
					CFistOfBlade* pTemp = CFistOfBlade::Create(m_pDevice, i);
					Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pTemp);
				}
			}
			else if (m_iStateID == SharpenMana02)
			{
				m_iStateID = SharpenMana03;
				ZeroMemory(m_bSkill, sizeof(_bool) * 10);

				for (_uint i = 4; i < 6; ++i)
				{
					CFistOfBlade* pTemp = CFistOfBlade::Create(m_pDevice, i);
					Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pTemp);
				}
				
			}
			else if (m_iStateID == SharpenMana03)
			{
				m_iStateID = SharpenMana04;
				ZeroMemory(m_bSkill, sizeof(_bool) * 10);

				for (_uint i = 6; i < 8; ++i)
				{
					CFistOfBlade* pTemp = CFistOfBlade::Create(m_pDevice, i);
					Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pTemp);
				}
			}
			else if (m_iStateID == SharpenMana04)
			{
				m_iStateID = SharpenMana05;
				ZeroMemory(m_bSkill, sizeof(_bool) * 10);
			
				for (_uint i = 8; i < 10; ++i)
				{
					CFistOfBlade* pTemp = CFistOfBlade::Create(m_pDevice, i);
					Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pTemp);
				}
			}
			else if (m_iStateID == SharpenMana05)
			{
				m_iStateID = SharpenMana06;
				ZeroMemory(m_bSkill, sizeof(_bool) * 10);

				for (_uint i = 10; i < 12; ++i)
				{
					CFistOfBlade* pTemp = CFistOfBlade::Create(m_pDevice, i);
					Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pTemp);
				}
			}
			m_pMeshCom->SetAnimation(m_iStateID, 1.5f);
		}
		else
		{
			if (m_iStateID != BattleIdle && m_pMeshCom->ProgressPerAnimation() < 0.25)
				return;

			if (m_pMeshCom->GetAniIndex() == Attack05 && m_pMeshCom->ProgressPerAnimation() < 0.6)
				return;

			CTrailEffect* pEffect = CTrailEffect::Create(m_pDevice);
			Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);

			if ((m_iStateID >= Attack01 && m_iStateID <= Attack05) == false)
			{
				m_iStateID = Attack01;
				ZeroMemory(m_bSkill, sizeof(_bool) * 10);
				pEffect->SetSaveTime(0.3f);
			}
			else if (m_iStateID == Attack01)
			{
				m_iStateID = Attack02;
				ZeroMemory(m_bSkill, sizeof(_bool) * 10);
			}
			else if (m_iStateID == Attack02)
			{
				m_iStateID = Attack03;
				ZeroMemory(m_bSkill, sizeof(_bool) * 10);
				pEffect->SetLifeTime(0.2f);
			}
			else if (m_iStateID == Attack03)
			{
				m_iStateID = Attack04;
				ZeroMemory(m_bSkill, sizeof(_bool) * 10);
				pEffect->SetSaveTime(0.8f);
				pEffect->SetLifeTime(0.2f);
			}
			else if (m_iStateID == Attack04)
			{
				m_iStateID = Attack05;
				ZeroMemory(m_bSkill, sizeof(_bool) * 10);
				pEffect->SetSaveTime(0.7f);
				pEffect->SetLifeTime(0.1f);
			}
			m_pMeshCom->SetAnimation(m_iStateID);
		}
	}
}
void CPlayer::AnimationCheck( void )
{
	if (m_pMeshCom->CheckAnimation() == true)
	{
		if (m_iStateID == RunLower)
			return;

		if (m_iStateID == SharpenCombo || m_iStateID == DarkKnight || m_iStateID == LordOfMana)
			Engine::Get_CameraMgr()->SetCamera(L"Cam_Default");

		ZeroMemory(m_bSkill, sizeof(_bool) * 10);

		m_iStateID = BattleIdle;
		m_pMeshCom->SetAnimation(BattleIdle);
	}
}

void CPlayer::Collider( const _float& fDelta )
{
	if (m_pCollisionCom->CollisionOBB((Engine::CCollisionBox*)Engine::Get_Management()->GetLayer(L"Layer_Boss")->GetObject(0)->GetComponent(L"Com_CollisionBox")) == true &&
		((CBoss*)Engine::Get_Management()->GetLayer(L"Layer_Boss")->GetObject(0))->GetExcept() == false)
	{
		MyVec3 vDif = *((Engine::CTransform*)Engine::Get_Management()->GetLayer(L"Layer_Boss")->GetObject(0)->GetComponent(L"Com_Transform"))->GetInfomation(Engine::CTransform::INFO_POS) - *m_pTransformCom->GetInfomation(Engine::CTransform::INFO_POS);
		D3DXVec3Normalize(&vDif, &vDif);
		vDif.y = 0.f;
		m_pTransformCom->Straight(&Engine::MyVec3(vDif * -1.f), m_fSpeed * fDelta);
	}
}

void CPlayer::SetDamage( const _float& fAttack )
{
	if (m_iStateID == DiffusionShift)
		return;
	else if (m_iStateID == RuinBlade || m_iStateID == LordOfMana || m_iStateID == SharpenCombo)
	{

	}
	else if (fAttack >= 20.f)
	{
		m_iStateID = DamageFront;
		m_pMeshCom->SetAnimation(m_iStateID);
		CSoundMgr::GetInstance()->PlaySound(L"arisha_hurt_medium_02.wav", 0);
	}

	m_fHp -= fAttack;
	m_vColor = MyVec4(1.f, 0.f, 0.f, 0.f);


	if (m_fHp <= 0.f)
	{
		m_iStateID = BattleIdle;
		m_pMeshCom->SetAnimation(m_iStateID);
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


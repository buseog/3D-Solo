#include "StdAfx.h"
#include "Boss.h"
#include "BossShield.h"
#include "BossSword.h"
#include "Teleport.h"
#include "SwingDust.h"
#include "Dash.h"
#include "ShockWave.h"
#include "ShockWaveBomb.h"
#include "Bombing.h"
#include "TrailEffect.h"
#include "JumpSmash.h"
#include "SoundMgr.h"
#include "UI.h"

CBoss::CBoss( LPDIRECT3DDEVICE9 pDevice )
: Engine::CGameObject(pDevice)
, m_pRendererCom(NULL)
, m_pTransformCom(NULL)
, m_pMtrCom(NULL)
, m_pMeshCom(NULL)
, m_pCollisionCom(NULL)
, m_iStateID(Idle)
, m_pMatPlayer(NULL)
, m_bStartPattern(false)
, m_bBlur(false)
, m_bHide(false)
, m_bDie(false)
, m_bAuto(false)
, m_fRefreshTime(0.f)
{
	 ZeroMemory(m_bSkill, sizeof(_bool) * 10);
	 ZeroMemory(m_vColor, sizeof(MyVec4));
}

CBoss::~CBoss( void )
{

}

HRESULT CBoss::Initialize( void )
{
	if (FAILED(ReadyComponent()))
		return E_FAIL;

	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_SCALE, &Engine::MyVec3(0.1f, 0.1f, 0.1f));
	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_POS, &Engine::MyVec3(0.f, -5.f, -20.f));
	m_pTransformCom->SetNaviIndex(17);

	m_pMeshCom->SetAnimation(Idle);

	m_fHp = 2000.f;
	m_fFullHp = m_fHp;
	m_fAttack = 50.f;
	m_fSpeed = 15.f;
	m_fPatternTime = 5.f;
	m_fDeathTime = 5.f;
	
	return S_OK;
}

_int CBoss::Update( const _float& fDelta )
{
	if (lstrcmp(Engine::Get_CameraMgr()->GetCurrentCameraKey(), L"Cam_Action") == false)
	{
		m_pRendererCom->AddRenderList(Engine::CRenderer::RENDER_NONEALPHA, this);
		return 0;
	}
	if (m_bDie == true)
	{
		if (m_fDeathTime > 0.f)
		{
			m_fDeathTime -= fDelta;
			m_pMeshCom->PlayAnimation(fDelta);
			m_pRendererCom->AddRenderList(Engine::CRenderer::RENDER_NONEALPHA, this);
		}
		return 0;
	}


	if (m_pMatPlayer == NULL)
		m_pMatPlayer = ((Engine::CTransform*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(0)->GetComponent(L"Com_Transform"))->GetWorldMatrix();

	if (m_bStartPattern == false)
		return 0;

	if (m_bAuto == true)
	{
		if (m_iStateID == Idle)
			m_fPatternTime -= fDelta;

		if (m_fPatternTime < 0.f)
		{
			m_iStateID = rand() % 5 + 1;
			m_pMeshCom->SetAnimation(m_iStateID, 1.3f);
			m_fPatternTime = rand() % 3 + 3.f;

			if (m_iStateID == Bombing)
				CSoundMgr::GetInstance()->PlaySound(L"lughlamhfada_spear_success_begin_01.wav", 2);
			else if (m_iStateID == Dash)
				CSoundMgr::GetInstance()->PlaySound(L"lughlamhfada_shout_02.wav", 2);
		}
	}

	if (GetAsyncKeyState(VK_F5) & 0x8000)
	{
		if (m_bAuto == true)
			m_bAuto = false;
		else
			m_bAuto = true;
	}
	
	if (GetAsyncKeyState('Z') & 0x8000)
	{
		m_iStateID = Dash;
		m_pMeshCom->SetAnimation(m_iStateID);
	}
	if (GetAsyncKeyState('X') & 0x8000)
	{
		m_iStateID = ShockWave;
		m_pMeshCom->SetAnimation(m_iStateID);
	}
	if (GetAsyncKeyState('C') & 0x8000)
	{
		m_iStateID = TripleAttack;
		m_pMeshCom->SetAnimation(m_iStateID);
	}
	if (GetAsyncKeyState('V') & 0x8000)
	{
		m_iStateID = JumpSamsh;
		m_pMeshCom->SetAnimation(m_iStateID);
	}
	if (GetAsyncKeyState('B') & 0x8000)
	{
		m_iStateID = Bombing;
		m_pMeshCom->SetAnimation(m_iStateID);
		CSoundMgr::GetInstance()->PlaySound(L"lughlamhfada_spear_success_begin_01.wav", 3);
	}

	m_vColor.x -= 3.f * fDelta;
	if (m_vColor.x <= 0.f)
		m_vColor.x = 0.f;

	Pattern(fDelta);

	m_pMeshCom->PlayAnimation(fDelta);
	AnimationCheck();
	
	if (m_bHide == true)
		return 0;

	Engine::CGameObject::Update(fDelta);
	m_pRendererCom->AddRenderList(Engine::CRenderer::RENDER_NONEALPHA, this);

	return 0;
}

_int CBoss::LastUpdate( const _float& fDelta )
{
	MyVec3 vLength = *(MyVec3*)&m_pMatPlayer->m[3][0] - *m_pTransformCom->GetInfomation(Engine::CTransform::INFO_POS);
	_float fDis = abs(D3DXVec3Length(&vLength));

	if (fDis < 150.f && m_bStartPattern == false)
	{
		CBossShield* pShield = CBossShield::Create(m_pDevice);
		Engine::Get_Management()->GetLayer(L"Layer_Boss")->AddGameObject(pShield);

		m_iStateID = Intro;
		m_pMeshCom->SetAnimation(m_iStateID);
		m_bStartPattern = true;
		Engine::Get_CameraMgr()->SetCamera(L"Cam_Intro");
		CSoundMgr::GetInstance()->PlayBGM(L"lughlamhfada_start_bg.wav");
		CSoundMgr::GetInstance()->PlaySound(L"lughlamhfada_start_vo.wav", 2);
	}

	return 0;	
}

void CBoss::Render( void )
{
	LPD3DXEFFECT	pEffect = m_pShaderCom->GetEffect();
	if (pEffect == NULL)
		return;
	SetUpShader(pEffect);
	m_pCollisionCom->Render(Engine::CCollisionBox::COLL_OBB, m_pTransformCom->GetWorldMatrix());

	//Engine::Get_FontMgr()->Render(L"Font_Gulim", L"루 라바다", MyVec3(350.f, 10.f, 0.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

	/*MyVec3 vLength = *(MyVec3*)&m_pMatPlayer->m[3][0] - *m_pTransformCom->GetInfomation(Engine::CTransform::INFO_POS);
	_float fDis = abs(D3DXVec3Length(&vLength));
	_tchar szBuf[128] = L"";
	swprintf_s(szBuf, L"Dis : %f", fDis);
	Engine::Get_FontMgr()->Render(L"Font_Batang", szBuf, MyVec3(0.f, 50.f, 0.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	swprintf_s(szBuf, L"Per : %f", m_pMeshCom->ProgressPerAnimation());
	Engine::Get_FontMgr()->Render(L"Font_Batang", szBuf, MyVec3(0.f, 70.f, 0.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));*/
}

void CBoss::SetUpShader( LPD3DXEFFECT pEffect )
{
	pEffect->AddRef();

	MyMat matView, matProj;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);
	pEffect->SetVector("g_vColor", &m_vColor);
	pEffect->SetFloat("g_fAlpha", 1.f * (m_fDeathTime / 5.f));

	m_pTransformCom->SetUpShader(pEffect);

	Engine::Get_TargetMgr()->EndMRT(L"MRT_Deferred");
	//Engine::Get_TargetMgr()->GetTarget(L"Target_Shadow")->ClearTarget();
	Engine::Get_TargetMgr()->GetTarget(L"Target_Shadow")->SetUpDevice(0);
	m_pMeshCom->SetUpShader(pEffect, 4);
	Engine::Get_TargetMgr()->GetTarget(L"Target_Shadow")->ResetDevice(0);
	Engine::Get_TargetMgr()->BeginMRT(L"MRT_Deferred", 1);

	if (m_bDie == true)
	{
		m_pMeshCom->SetUpShader(pEffect, 5);
		Engine::Safe_Release(pEffect);
		return;
	}


	if (m_bBlur == true && m_fRefreshTime <= 0.f)
	{
		Engine::Get_TargetMgr()->GetTarget(L"Target_Blur")->SetUpDevice(3);
		m_fRefreshTime = 0.1f;	
		m_pMeshCom->SetUpShader(pEffect, 3);
		Engine::Get_TargetMgr()->GetTarget(L"Target_Blur")->ResetDevice(3);
	}
	else
		m_pMeshCom->SetUpShader(pEffect, 0);


	Engine::Safe_Release(pEffect);
}

void CBoss::AnimationCheck( void )
{
	if (m_pMeshCom->CheckAnimation() == true)
	{
		if (m_iStateID == Intro)
		{
			Engine::Get_CameraMgr()->SetCamera(L"Cam_Default");
			CSoundMgr::GetInstance()->StopSoundAll();

			CGameObject* pGameObject = CUI::Create(m_pDevice, 0);
			Engine::Get_Management()->GetLayer(L"Layer_UI")->AddGameObject(pGameObject);

			pGameObject = CUI::Create(m_pDevice, 1);
			Engine::Get_Management()->GetLayer(L"Layer_UI")->AddGameObject(pGameObject);

			CSoundMgr::GetInstance()->PlayBGM(L"bgm_prologue_battle_inside_tower.wav");
		}
		if (m_iStateID == Dying)
		{
			m_iStateID = Death;
			m_pMeshCom->SetAnimation(m_iStateID);
			m_bDie = true;
			return ;
		}

		m_iStateID = Idle;
		m_pMeshCom->SetAnimation(m_iStateID);
		ZeroMemory(m_bSkill, sizeof(_bool) * 10);
	}
}

void CBoss::Pattern( const _float& fDelta )
{
	switch (m_iStateID)
	{
	case StepFront:
		StepPattern(fDelta);
		break;

	case Dash:
		DashPattern(fDelta);
		break;

	case ShockWave:
		ShockWavePattern(fDelta);
		break;

	case TripleAttack:
		TripleAttackPattern(fDelta);
		break;

	case JumpSamsh:
		JumpSmashPattern(fDelta);
		break;

	case Bombing:
		BombingPattern(fDelta);
		break;

	case Intro:
		IntroPattern(fDelta);
		break;

	}
}

void CBoss::StepPattern( const _float& fDelta )
{
	MyVec3 vTarget = *(MyVec3*)&m_pMatPlayer->m[3][0];
	MyVec3 vPos = *(MyVec3*)&m_pTransformCom->GetWorldMatrix()->m[3][0];
	MyVec3 vMove = vTarget - vPos;

	if (D3DXVec3Length(&vMove) <= 10.f)
		return;

	m_pTransformCom->Move((Engine::MyVec3*)D3DXVec3Normalize(&vMove, &vMove), m_fSpeed * fDelta);
}

void CBoss::DashPattern( const _float& fDelta )
{
	MyVec3 vDir = *(MyVec3*)&m_pMatPlayer->m[3][0] - *m_pTransformCom->GetInfomation(Engine::CTransform::INFO_POS);
	D3DXVec3Normalize(&vDir, &vDir);

	if (m_pMeshCom->ProgressAnimation() > 5.3 && m_pMeshCom->ProgressAnimation() < 5.5)
		m_pTransformCom->Move((Engine::MyVec3*)&vDir, m_fSpeed * fDelta * 11.f);

	else if (m_pMeshCom->ProgressAnimation() > 5.5)
		m_pTransformCom->Move((Engine::MyVec3*)&vDir, m_fSpeed * fDelta * 8.f);

	if (m_pMeshCom->ProgressPerAnimation() > 0.85 && m_bSkill[1] == false)
	{
		CSoundMgr::GetInstance()->PlaySound(L"lughlamhfada_attack_middle_02.wav", 2);
		CSoundMgr::GetInstance()->PlaySound(L"lughlamhfada_tathlum_wave.wav", 3);
		m_pTransformCom->SetRotation(&vDir);
		m_bSkill[1] = true;
	}

	if (m_pMeshCom->ProgressPerAnimation() > 0.88 && m_bSkill[2] == false)
	{
		m_pTransformCom->SetRotation(&vDir);
		m_bSkill[2] = true;
	}

	if (m_pMeshCom->ProgressPerAnimation() > 0.90 && m_bSkill[3] == false)
	{
		m_pTransformCom->SetRotation(&vDir);
		m_bSkill[3] = true;
	}

	if (m_pMeshCom->ProgressPerAnimation() > 0.85 && m_bSkill[4] == false)
	{
		for (int i = 0; i < 7; ++i)
		{
			CDash* pEffect = CDash::Create(m_pDevice, i);
			Engine::Get_Management()->GetLayer(L"Layer_Boss")->AddGameObject(pEffect);
		}

		for (int i = 10; i < 12; ++i)
		{
			CDash* pEffect = CDash::Create(m_pDevice, i);
			Engine::Get_Management()->GetLayer(L"Layer_Boss")->AddGameObject(pEffect);
		}

		CTrailEffect* pEffect = CTrailEffect::Create(m_pDevice, 4);
		Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);

		((CBossSword*)Engine::Get_Management()->GetLayer(L"Layer_Boss")->GetObject(2))->SetCollision(2);
		m_bSkill[4] = true;
	}
}

void CBoss::ShockWavePattern( const _float& fDelta )
{
	m_fRefreshTime -= fDelta;

	MyVec3 vLook = *(MyVec3*)&m_pTransformCom->GetWorldMatrix()->m[2][0];
	D3DXVec3Normalize(&vLook, &vLook);

	if (m_pMeshCom->ProgressAnimation() > 0.2 && m_pMeshCom->ProgressAnimation() < 1.5)
		m_pTransformCom->Move((Engine::MyVec3*)&vLook, -m_fSpeed * fDelta * 0.7f);

	else if (m_pMeshCom->ProgressAnimation() > 1.5 && m_pMeshCom->ProgressAnimation() < 2.0)
	{
		m_pTransformCom->Move((Engine::MyVec3*)&vLook, -m_fSpeed * fDelta * 4.f);
		Engine::Get_TargetMgr()->GetTarget(L"Target_Blur")->SetExcept(true);
		m_bBlur = true;
	}
	else if (m_pMeshCom->ProgressAnimation() > 2.0)
	{
		Engine::Get_TargetMgr()->GetTarget(L"Target_Blur")->SetExcept(false);
		m_bBlur = false;
	}

	else if (m_pMeshCom->ProgressAnimation() > 3.0 && m_pMeshCom->ProgressAnimation() < 3.5)
		m_pTransformCom->Move((Engine::MyVec3*)&vLook, -m_fSpeed * fDelta * 1.2f);

	else if (m_pMeshCom->ProgressAnimation() > 3.5 && m_pMeshCom->ProgressAnimation() < 4.0)
		m_pTransformCom->Move((Engine::MyVec3*)&vLook, -m_fSpeed * fDelta * 0.3f);

	else if (m_pMeshCom->ProgressAnimation() > 4.0 && m_pMeshCom->ProgressAnimation() < 5.3)
		m_pTransformCom->Move((Engine::MyVec3*)&vLook, -m_fSpeed * fDelta * 0.35f);

	if (m_bSkill[0] == false)
	{
		MyVec3 vDir = MyVec3(0.f, 0.f, 0.f) - *m_pTransformCom->GetInfomation(Engine::CTransform::INFO_POS);
		D3DXVec3Normalize(&vDir, &vDir);
		D3DXVec3Cross(&vDir, &MyVec3(0.f, 1.f, 0.f), &vDir);
		m_pTransformCom->SetRotation(&vDir);
		CSoundMgr::GetInstance()->PlaySound(L"lughlamhfada_spawn.wav", 2);
		m_bSkill[0] = true;
	}

	if (m_pMeshCom->ProgressPerAnimation() >= 0.33 && m_bSkill[1] == false)
	{
		for (int i = 0; i < 6; ++i)
		{
			CShockWave* pEffect = CShockWave::Create(m_pDevice, i);
			Engine::Get_Management()->GetLayer(L"Layer_Boss")->AddGameObject(pEffect);
		}
		CTrailEffect* pEffect = CTrailEffect::Create(m_pDevice, 4);
		Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
		pEffect->SetSaveTime(0.2f);
		pEffect->SetLifeTime(0.5f);
		m_bSkill[1] = true;
	}

	if (m_pMeshCom->ProgressPerAnimation() >= 0.455 && m_bSkill[2] == false)
	{
		for (int i = 10; i < 16; ++i)
		{
			CShockWave* pEffect = CShockWave::Create(m_pDevice, i);
			Engine::Get_Management()->GetLayer(L"Layer_Boss")->AddGameObject(pEffect);
		}
		CSoundMgr::GetInstance()->PlaySound(L"lughlamhfada_attack_short_02.wav", 2);
		m_bSkill[2] = true;
	}
	if (m_pMeshCom->ProgressPerAnimation() >= 0.55 && m_bSkill[3] == false)
	{
		for (int i = 0; i < 8; ++i)
		{
			CShockWaveBomb* pEffect = CShockWaveBomb::Create(m_pDevice, i);
			Engine::Get_Management()->GetLayer(L"Layer_Boss")->AddGameObject(pEffect);
		}
		CSoundMgr::GetInstance()->PlaySound(L"effect_bomb_explode.wav", 3);
		m_bSkill[3] = true;
	}
}

void CBoss::TripleAttackPattern( const _float& fDelta )
{
	MyVec3 vDir = *(MyVec3*)&m_pMatPlayer->m[3][0] - *m_pTransformCom->GetInfomation(Engine::CTransform::INFO_POS);
	D3DXVec3Normalize(&vDir, &vDir);

	if (m_pMeshCom->ProgressAnimation() > 0.7 && m_pMeshCom->ProgressAnimation() < 2.0)
		m_pTransformCom->Move((Engine::MyVec3*)&vDir, m_fSpeed * fDelta * 1.3f);

	else if (m_pMeshCom->ProgressAnimation() > 2.0 && m_pMeshCom->ProgressAnimation() < 3.6)
		m_pTransformCom->Move((Engine::MyVec3*)&vDir, m_fSpeed * fDelta * 1.7f);

	else if (m_pMeshCom->ProgressAnimation() > 3.7 && m_pMeshCom->ProgressAnimation() < 4.7)
		m_pTransformCom->Move((Engine::MyVec3*)&vDir, m_fSpeed * fDelta * 1.5f);

	else if (m_pMeshCom->ProgressAnimation() > 8.7 && m_pMeshCom->ProgressAnimation() < 10.0)
		m_pTransformCom->Move((Engine::MyVec3*)&vDir, m_fSpeed * fDelta * 0.3f);

	if (m_pMeshCom->ProgressPerAnimation() > 0.12 && m_bSkill[0] == false)
	{
		CTrailEffect* pEffect = CTrailEffect::Create(m_pDevice, 4);
		Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
		pEffect->SetLifeTime(0.3f);

		m_bExceptCollision = true;
		m_bSkill[0] = true;
	}

	if (m_pMeshCom->ProgressPerAnimation() > 0.142 && m_bSkill[1] == false)
	{
		for (_uint i = 0; i < 8; ++i)
		{
			CSwingDust* pEffect = CSwingDust::Create(m_pDevice, i);
			Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
		}

		((CBossSword*)Engine::Get_Management()->GetLayer(L"Layer_Boss")->GetObject(2))->SetCollision(1);
		CSoundMgr::GetInstance()->PlaySound(L"lughlamhfada_attack_short_03.wav", 2);
		CSoundMgr::GetInstance()->PlaySound(L"lughlamhfada_swing_lv1.wav", 3);
		m_bSkill[1] = true;
	}

	if (m_pMeshCom->ProgressPerAnimation() > 0.23 && m_bSkill[2] == false)
	{
		CTrailEffect* pEffect = CTrailEffect::Create(m_pDevice, 4);
		Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
		pEffect->SetLifeTime(0.7f);

		m_bSkill[2] = true;
	}

	if (m_pMeshCom->ProgressPerAnimation() > 0.28 && m_bSkill[3] == false)
	{
		for (_uint i = 0; i < 8; ++i)
		{
			CSwingDust* pEffect = CSwingDust::Create(m_pDevice, i);
			Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
		}

		((CBossSword*)Engine::Get_Management()->GetLayer(L"Layer_Boss")->GetObject(2))->SetCollision(1);
		CSoundMgr::GetInstance()->PlaySound(L"lughlamhfada_swing_lv2.wav", 3);
		m_bSkill[3] = true;
	}

	if (m_pMeshCom->ProgressPerAnimation() > 0.37 && m_bSkill[4] == false)
	{
		CTrailEffect* pEffect = CTrailEffect::Create(m_pDevice, 4);
		Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
		pEffect->SetLifeTime(0.7f);

		((CBossSword*)Engine::Get_Management()->GetLayer(L"Layer_Boss")->GetObject(2))->SetCollision(1);
		CSoundMgr::GetInstance()->PlaySound(L"lughlamhfada_swing_lv3.wav", 3);
		m_bSkill[4] = true;
	}

	if (m_pMeshCom->ProgressPerAnimation() > 0.39 && m_bSkill[5] == false)
	{
		for (_uint i = 10; i < 13; ++i)
		{
			CSwingDust* pEffect = CSwingDust::Create(m_pDevice, i);
			Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
		}
		m_bExceptCollision = false;
		m_bSkill[5] = true;
	}
}

void CBoss::JumpSmashPattern( const _float& fDelta )
{
	if (m_pMeshCom->ProgressPerAnimation() > 0.01 && m_bSkill[0] == false)
	{
		for (int i = 0; i < 3; ++i)
		{
			CTeleport* pEffect = CTeleport::Create(m_pDevice, i);
			Engine::Get_Management()->GetLayer(L"Layer_Boss")->AddGameObject(pEffect);
		}

		m_bHide = true;
		CSoundMgr::GetInstance()->PlaySound(L"arisha_skill_manastone_fire_02.wav", 3);
		m_bSkill[0] = true;
	}
	if (m_pMeshCom->ProgressPerAnimation() > 0.2 && m_bSkill[1] == false)
	{
		for (int i = 10; i < 14; ++i)
		{
			CTeleport* pEffect = CTeleport::Create(m_pDevice, i);
			Engine::Get_Management()->GetLayer(L"Layer_Boss")->AddGameObject(pEffect);
		}

		MyVec3 vPos = *(MyVec3*)&m_pMatPlayer->m[3][0];
		MyVec3 vLook = *(MyVec3*)&m_pMatPlayer->m[2][0];
		D3DXVec3Normalize(&vLook, &vLook);
		MyVec3 vTelPos = vPos + vLook * -15.f;

		m_pTransformCom->SetInfomation(Engine::CTransform::INFO_POS, &vTelPos);

		MyVec3 vDir = vPos - *m_pTransformCom->GetInfomation(Engine::CTransform::INFO_POS);
		D3DXVec3Normalize(&vDir, &vDir);

		m_pTransformCom->SetRotation(&vDir);
		m_bHide = false;
		m_bSkill[1] = true;
	}
	if (m_pMeshCom->ProgressPerAnimation() > 0.69 && m_bSkill[2] == false)
	{
		for (int i = 0; i < 6; ++i)
		{
			CJumpSmash* pEffect = CJumpSmash::Create(m_pDevice, i);
			Engine::Get_Management()->GetLayer(L"Layer_Boss")->AddGameObject(pEffect);
		}
		CSoundMgr::GetInstance()->PlaySound(L"lughlamhfada_attack_short_01.wav", 2);
		CSoundMgr::GetInstance()->PlaySound(L"lughlamhfada_swing_lv1.wav", 3);
		CSoundMgr::GetInstance()->PlaySound(L"lughlamhfada_tathlum_wave.wav", 4);
		m_bSkill[2] = true;
	}

}

void CBoss::BombingPattern( const _float& fDelta )
{
	if (m_pMeshCom->ProgressPerAnimation() > 0.23 && m_bSkill[0] == false)
	{
		for (int i = 0; i < 7; ++i)
		{
			CBombing* pEffect = CBombing::Create(m_pDevice, i);
			Engine::Get_Management()->GetLayer(L"Layer_Boss")->AddGameObject(pEffect);
		}
		for (int i = 10; i < 14; ++i)
		{
			CBombing* pEffect = CBombing::Create(m_pDevice, i);
			Engine::Get_Management()->GetLayer(L"Layer_Boss")->AddGameObject(pEffect);
		}
		for (int i = 0; i < 7; ++i)
		{
			CShockWaveBomb* pEffect = CShockWaveBomb::Create(m_pDevice, i);
			Engine::Get_Management()->GetLayer(L"Layer_Boss")->AddGameObject(pEffect);
		}
		CSoundMgr::GetInstance()->PlaySound(L"lughlamhfada_attack_short_02.wav", 2);
		CSoundMgr::GetInstance()->PlaySound(L"effect_bomb_explode.wav", 3);
		CSoundMgr::GetInstance()->PlaySound(L"effect_fireframe_end.wav", 4);

		m_bSkill[0] = true;
	}
	if (m_pMeshCom->ProgressPerAnimation() > 0.40 && m_bSkill[1] == false)
	{
		for (int i = 20; i < 25; ++i)
		{
			CBombing* pEffect = CBombing::Create(m_pDevice, i);
			Engine::Get_Management()->GetLayer(L"Layer_Boss")->AddGameObject(pEffect);
		}
		((CBossSword*)Engine::Get_Management()->GetLayer(L"Layer_Boss")->GetObject(2))->SetAttach(false);
		CSoundMgr::GetInstance()->PlaySound(L"lughlamhfada_attack_middle_05.wav", 2);
		m_bSkill[1] = true;
	}

	if (m_pMeshCom->ProgressPerAnimation() > 0.55 && m_bSkill[2] == false)
	{
		for (int i = 30; i < 37; ++i)
		{
			CBombing* pEffect = CBombing::Create(m_pDevice, i);
			Engine::Get_Management()->GetLayer(L"Layer_Boss")->AddGameObject(pEffect);
		}
		CSoundMgr::GetInstance()->PlaySound(L"effect_fireball_crash.wav", 3);
		m_bSkill[2] = true;
	}

	if (m_pMeshCom->ProgressPerAnimation() > 0.78 && m_bSkill[3] == false)
	{
		((CBossSword*)Engine::Get_Management()->GetLayer(L"Layer_Boss")->GetObject(2))->SetAttach(true);
		CTrailEffect* pEffect = CTrailEffect::Create(m_pDevice, 4);
		pEffect->SetLifeTime(0.6f);
		Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
		
		m_bSkill[3] = true;
	}

	if (m_pMeshCom->ProgressPerAnimation() > 0.82 && m_bSkill[4] == false)
	{
		CSoundMgr::GetInstance()->PlaySound(L"lughlamhfada_admiration_01.wav", 2);
		m_bSkill[4] = true;
	}
	
}

void CBoss::IntroPattern(const _float& fDelta)
{
	if (m_pMeshCom->ProgressPerAnimation() > 0.1 && m_bSkill[2] == false)
	{
		for (_int i = 10; i < 14; ++i)
		{
			CTeleport* pEffect = CTeleport::Create(m_pDevice, i);
			Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
		}

		m_bSkill[2] = true;
	}
	if (m_pMeshCom->ProgressPerAnimation() > 0.56 && m_bSkill[0] == false)
	{
		CBossSword* pSword = CBossSword::Create(m_pDevice);
		Engine::Get_Management()->GetLayer(L"Layer_Boss")->AddGameObject(pSword);
		
		m_bSkill[0] = true;
	}
	if (m_pMeshCom->ProgressPerAnimation() > 0.64 && m_bSkill[1] == false)
	{
		((CBossSword*)Engine::Get_Management()->GetLayer(L"Layer_Boss")->GetObject(2))->SetAttach(true);

		CTrailEffect* pEffect = CTrailEffect::Create(m_pDevice, 4);
		Engine::Get_Management()->GetLayer(L"Layer_Effect")->AddGameObject(pEffect);
		pEffect->SetLifeTime(0.5f);

		m_bSkill[1] = true;
	}
}

void CBoss::SetDamage( const _float& fAttack )
{
	if (m_iStateID == Idle)
	{
		m_iStateID = Damage;
		m_pMeshCom->SetAnimation(m_iStateID);

		_uint iRand = rand() % 2;
		if (iRand == 0)
			CSoundMgr::GetInstance()->PlaySound(L"lughlamhfada_damage_01.wav", 2);
		else
			CSoundMgr::GetInstance()->PlaySound(L"lughlamhfada_damage_01.wav", 2);
	}
	m_fHp -= fAttack;

	m_vColor = MyVec4(1.f, 0.f, 0.f, 0.f);

	if (m_fHp <= 0.f)
	{
		m_iStateID = Dying;
		m_pMeshCom->SetAnimation(m_iStateID);
	}
}

HRESULT CBoss::ReadyComponent( void )
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

	m_pMeshCom = (Engine::CDynamicMesh*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Mesh_Boss");
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

void CBoss::Free( void )
{
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pMtrCom);
	Engine::Safe_Release(m_pMeshCom);
	Engine::Safe_Release(m_pCollisionCom);
	Engine::Safe_Release(m_pShaderCom);

	Engine::CGameObject::Free();
}

CBoss* CBoss::Create( LPDIRECT3DDEVICE9 pDevice )
{
	CBoss* pPlayer = new CBoss(pDevice);

	if (FAILED(pPlayer->Initialize()))
	{
		MSG_BOX("Player Created Failed");
		Engine::Safe_Release(pPlayer);
	}
	return pPlayer;
}
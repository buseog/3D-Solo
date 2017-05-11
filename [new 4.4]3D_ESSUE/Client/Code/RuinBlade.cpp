#include "StdAfx.h"
#include "RuinBlade.h"
#include "LongBlade.h"

CRuinBlade::CRuinBlade( LPDIRECT3DDEVICE9 pDevice)
: CEffect(pDevice)
{
	
}

CRuinBlade::~CRuinBlade( void )
{

}

HRESULT CRuinBlade::Initialize( const _uint& iType, MyMat* pTarget, MyMat* pBone)
{
	m_iType = iType;
	if (m_iType == 8)
	{
		m_pBone = ((Engine::CDynamicMesh*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(1)->GetComponent(L"Com_Mesh"))->GetPartsMatrix("ValveBiped_Anim_Attachment_RH");
		m_pTarget = ((CLongBlade*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(1))->GetComputeMatrix();
	}
	else
	{
		m_pBone = pBone;
		m_pTarget = pTarget;
	}
	
	m_vOrigin = *(MyVec3*)&(*m_pBone * *m_pTarget).m[3][0];
	m_vAngle = *((Engine::CTransform*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(0)->GetComponent(L"Com_Transform"))->GetInfomation(Engine::CTransform::INFO_ANGLE);
	m_vAngle.y = D3DXToDegree(m_vAngle.y);

	DefaultSetting();

	if (FAILED(Ready_Component()))
		return E_FAIL;

	for (_uint i = 0; i < m_iCount; ++i)
		AddParticle(i);

	return S_OK;
}

_int CRuinBlade::Update( const _float& fDelta )
{
	if (isDead() == true)
	{
		m_bDestroy = true;
		return 0;
	}

	m_fDurationAcc += fDelta;
	m_fIntervalAcc += fDelta;

	if (m_fDurationAcc <= m_fDuration)
	{
		if (m_fIntervalAcc >= m_fInterval)
		{
			for (_uint i = 0; i < m_iCount; ++i)
				AddParticle(i);

			if (m_iType == 8 && m_iCount <= 23)
				++m_iCount;
			m_fIntervalAcc = 0.f;
		}
	}

	EFFECTLIST::iterator iter = m_EffectList.begin();
	EFFECTLIST::iterator iter_end = m_EffectList.end();

	for ( ; iter != iter_end; ++iter)
	{	
		if ((*iter)->bAlive == true)
		{
			(*iter)->fLifeTime += fDelta;
			(*iter)->fFrame += (*iter)->fFrameSpeed * fDelta;
			if ((*iter)->fFrame >= 25.f)
				(*iter)->fFrame = 0.f;

			(*iter)->vPos += (*iter)->vDir * (*iter)->fSpeed * fDelta;

			if ((*iter)->fLifeTime >= (*iter)->fMaxLife)
				(*iter)->bAlive = false;
		}
	}

	Engine::CGameObject::Update(fDelta);
	m_pRendererCom->AddRenderList(Engine::CRenderer::RENDER_ALPHA, this);

	return 0;
}

void CRuinBlade::Render( void )
{
	LPD3DXEFFECT pEffect = m_pShaderCom->GetEffect();
	if (pEffect == NULL)
		return;

	SetUpShader(pEffect);
}

void CRuinBlade::SetUpShader( LPD3DXEFFECT pEffect )
{
	pEffect->AddRef();

	MyMat matView, matProj;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	m_pTextureCom->SetUpShader(pEffect, m_iIndex);
	
	pEffect->Begin(NULL, 0);
	if (m_iType == 3 || m_iType == 10 || m_iType == 21 || m_iType == 25)
		pEffect->BeginPass(5);
	else
		pEffect->BeginPass(1);

	EFFECTLIST::iterator iter = m_EffectList.begin();
	EFFECTLIST::iterator iter_end = m_EffectList.end();
	MyMat matScale, matRotX, matRotY, matRotZ, matTrans, matWorld;
	MyMat matRot;

	for ( ; iter != iter_end; ++iter)
	{
		if ((*iter)->bAlive == true)
		{
			_float	fSizeDelta = GetSizeDelta(*iter);
			_float	fRollDelta = GetRollDelta(*iter);
			_uint	iAlphaDelta = GetAlphaDelta(*iter);
			MyVec3	vColorDelta = GetColorDelta(*iter);
			D3DXMatrixScaling(&matScale, (*iter)->vScale.x + fSizeDelta, (*iter)->vScale.y + fSizeDelta, (*iter)->vScale.z);
			D3DXMatrixRotationX(&matRotX, D3DXToRadian((*iter)->vAngle.x));
			D3DXMatrixRotationY(&matRotY, D3DXToRadian((*iter)->vAngle.y));
			D3DXMatrixRotationZ(&matRotZ, D3DXToRadian((*iter)->vAngle.z + fRollDelta));
			D3DXMatrixTranslation(&matTrans, (*iter)->vPos.x, (*iter)->vPos.y, (*iter)->vPos.z);
			matWorld = matScale * matRotZ * matRotX * matRotY *  matTrans;
			pEffect->SetMatrix("g_matWorld", &matWorld);
			pEffect->SetVector("g_vColor", &MyVec4(D3DXCOLOR(D3DCOLOR_ARGB(255 - iAlphaDelta, _int((*iter)->vColor.x + vColorDelta.x), _int((*iter)->vColor.y + vColorDelta.y), _int((*iter)->vColor.z + vColorDelta.z)))));
			pEffect->CommitChanges();
			m_pBufferCom->Render();
		}
	}
	
	pEffect->EndPass();
	pEffect->End();

	Engine::Safe_Release(pEffect);
}

void CRuinBlade::DefaultSetting( void )
{
	switch (m_iType)
	{
	case 0: // ½ºÆÄÅ©
		m_pEffectKey = L"Component_Texture_LightGlow";
		m_iCount = 1;
		m_iIndex = 2;

		m_fInterval = 0.03f;
		m_fDuration = 1.f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 1: // ¸µ1
		m_pEffectKey = L"Component_Texture_Ring_Outer_Wind";
		m_iCount = 1;
		m_iIndex = 3;

		m_fInterval = 0.01f;
		m_fDuration = 1.2f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;
	
	case 2: // ¸µ2
		m_pEffectKey = L"Component_Texture_WindBlade";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.01f;
		m_fDuration = 1.2f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 3: // ¸µ2
		m_pEffectKey = L"Component_Texture_Ring_Outer_Wind_n";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.01f;
		m_fDuration = 1.2f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 4: // Æ¢´ÂºÒ²É1
		m_pEffectKey = L"Component_Texture_YellowFlare";
		m_iCount = 1;
		m_iIndex = 7;

		m_fInterval = 0.004f;
		m_fDuration = 0.15f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 5: // Æ¢´ÂºÒ²É2
		m_pEffectKey = L"Component_Texture_YellowFlare";
		m_iCount = 1;
		m_iIndex = 7;

		m_fInterval = 0.004f;
		m_fDuration = 0.7f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 6: // Æ¢´ÂºÒ²É3
		m_pEffectKey = L"Component_Texture_YellowFlare";
		m_iCount = 1;
		m_iIndex = 7;

		m_fInterval = 0.006f;
		m_fDuration = 0.7f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 7: // Æ¢´ÂºÒ²É3
		m_pEffectKey = L"Component_Texture_Bomb";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.001f;
		m_fDuration = 0.6f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 8: // °Ë´Þ±ººû
		m_pEffectKey = L"Component_Texture_LightGlow";
		m_iCount = 1;
		m_iIndex = 2;

		m_fInterval = 0.001f;
		m_fDuration = 3.5f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 10: // »ÏÁ·±¼Àý1
		m_pEffectKey = L"Component_Texture_Ring_Outer_Wind_n";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.03f;
		m_fDuration = 0.2f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 11: // »ÏÁ·±¼Àý1
		m_pEffectKey = L"Component_Texture_WindBlade";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.03f;
		m_fDuration = 0.3f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	// Spark
	case 12: // Æ¢´ÂºÒ²É1
		m_pEffectKey = L"Component_Texture_YellowFlare";
		m_iCount = 3;
		m_iIndex = 0;

		m_fInterval = 0.01f;
		m_fDuration = 0.3f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 13: // Æ¢´ÂºÒ²É1
		m_pEffectKey = L"Component_Texture_YellowFlare";
		m_iCount = 3;
		m_iIndex = 0;

		m_fInterval = 0.02f;
		m_fDuration = 0.1f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 14: // Æ¢´ÂºÒ²É2
		m_pEffectKey = L"Component_Texture_BluntYellow";
		m_iCount = 3;
		m_iIndex = 0;

		m_fInterval = 0.01f;
		m_fDuration = 0.2f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	// Slash01
	case 20: // »ÏÁ·Æ®·¹ÀÏ 6
		m_pEffectKey = L"Component_Texture_YellowFlare";
		m_iCount = 3;
		m_iIndex = 3;

		m_fInterval = 0.05f;
		m_fDuration = 0.5f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 21: // »ÏÁ·Æ®·¹ÀÏ 6
		m_pEffectKey = L"Component_Texture_efxRipple";
		m_iCount = 2;
		m_iIndex = 0;

		m_fInterval = 0.05f;
		m_fDuration = 0.15f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 22: // »ÏÁ·Æ®·¹ÀÏ 6
		m_pEffectKey = L"Component_Texture_Blunt";
		m_iCount = 20;
		m_iIndex = 0;

		m_fInterval = 0.03f;
		m_fDuration = 0.08f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	// Slash02
	case 23: // »ÏÁ·Æ®·¹ÀÏ1
		m_pEffectKey = L"Component_Texture_CriticalEhen";
		m_iCount = 3;
		m_iIndex = 0;

		m_fInterval = 0.05f;
		m_fDuration = 0.15f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 24: // »ÏÁ·Æ®·¹ÀÏ1
		m_pEffectKey = L"Component_Texture_YellowFlare";
		m_iCount = 3;
		m_iIndex = 3;

		m_fInterval = 0.05f;
		m_fDuration = 0.15f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 25: // »ÏÁ·Æ®·¹ÀÏ2
		m_pEffectKey = L"Component_Texture_efxRipple";
		m_iCount = 3;
		m_iIndex = 0;

		m_fInterval = 0.05f;
		m_fDuration = 0.15f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 26: // »ÏÁ·Æ®·¹ÀÏ6
		m_pEffectKey = L"Component_Texture_Blunt";
		m_iCount = 5;
		m_iIndex = 0;

		m_fInterval = 0.03f;
		m_fDuration = 0.15f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 27: // »ÏÁ·Æ®·¹ÀÏ6
		m_pEffectKey = L"Component_Texture_BluntYellow";
		m_iCount = 5;
		m_iIndex = 2;

		m_fInterval = 0.07f;
		m_fDuration = 0.15f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 28: // »ÏÁ·Æ®·¹ÀÏ6
		m_pEffectKey = L"Component_Texture_BluntYellow";
		m_iCount = 1;
		m_iIndex = 7;

		m_fInterval = 0.07f;
		m_fDuration = 0.15f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 29: // »ÏÁ·Æ®·¹ÀÏ6
		m_pEffectKey = L"Component_Texture_BluntYellow";
		m_iCount = 5;
		m_iIndex = 1;

		m_fInterval = 0.03f;
		m_fDuration = 0.08f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	// RuinWave
	case 30: // »ÏÁ·Æ®·¹ÀÏ6
		m_pEffectKey = L"Component_Texture_Lightning";
		m_iCount = 2;
		m_iIndex = 4;

		m_fInterval = 0.1f;
		m_fDuration = 0.3f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 31: // »ÏÁ·Æ®·¹ÀÏ6
		m_pEffectKey = L"Component_Texture_Lightning";
		m_iCount = 1;
		m_iIndex = 3;

		m_fInterval = 0.1f;
		m_fDuration = 0.2f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 32: // »ÏÁ·Æ®·¹ÀÏ6
		m_pEffectKey = L"Component_Texture_Blunt";
		m_iCount = 2;
		m_iIndex = 0;

		m_fInterval = 0.04f;
		m_fDuration = 0.25f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 33: // »ÏÁ·Æ®·¹ÀÏ6
		m_pEffectKey = L"Component_Texture_FireSpark";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.03f;
		m_fDuration = 0.08f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 34: // »ÏÁ·Æ®·¹ÀÏ6
		m_pEffectKey = L"Component_Texture_FireStill";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.03f;
		m_fDuration = 0.08f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 35: // »ÏÁ·Æ®·¹ÀÏ6
		m_pEffectKey = L"Component_Texture_Bomb";
		m_iCount = 1;
		m_iIndex = 2;

		m_fInterval = 0.03f;
		m_fDuration = 0.08f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 36: // »ÏÁ·Æ®·¹ÀÏ6
		m_pEffectKey = L"Component_Texture_LightGlow";
		m_iCount = 1;
		m_iIndex = 2;

		m_fInterval = 0.05f;
		m_fDuration = 0.15f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 37: // »ÏÁ·Æ®·¹ÀÏ6
		m_pEffectKey = L"Component_Texture_RingFire";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.03f;
		m_fDuration = 0.08f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 40: // ºû1
		m_pEffectKey = L"Component_Texture_LightGlow";
		m_iCount = 2;
		m_iIndex = 2;

		m_fInterval = 0.03f;
		m_fDuration = 0.07f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 41: // ºû1
		m_pEffectKey = L"Component_Texture_LightRay";
		m_iCount = 4;
		m_iIndex = 0;

		m_fInterval = 0.03f;
		m_fDuration = 0.07f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 42: // ºû2
		m_pEffectKey = L"Component_Texture_ShipGray";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.04f;
		m_fDuration = 0.1f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 43: // ºû2
		m_pEffectKey = L"Component_Texture_RingFire";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.03f;
		m_fDuration = 0.15f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 44: // ºû2
		m_pEffectKey = L"Component_Texture_RingLine";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.03f;
		m_fDuration = 0.15f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 45: // ºû2
		m_pEffectKey = L"Component_Texture_LightGlow";
		m_iCount = (int)Engine::GetRandomFloat(2.f, 3.f);
		m_iIndex = 3;

		m_fInterval = 0.02f;
		m_fDuration = 0.2f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;
	
	case 46: // ºû2
		m_pEffectKey = L"Component_Texture_CriticalEhen";
		m_iCount = 1;
		m_iIndex = 2;

		m_fInterval = 0.02f;
		m_fDuration = 0.2f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;
	}
}

void CRuinBlade::AddParticle( const _uint& iIndex )
{
	EFFECT* pEffect= new EFFECT;
	ZeroMemory(pEffect, sizeof(EFFECT));

	pEffect->vScale = MyVec3(1.f, 1.f, 1.f);
	D3DXVECTOR3 vMin;
	D3DXVECTOR3 vMax;

	MyMat matCompute = *m_pBone * *m_pTarget;
	MyVec3 vPos = *(MyVec3*)&matCompute.m[3][0];
	MyVec3 vRight = *D3DXVec3Normalize(&vRight, (MyVec3*)&m_pTarget->m[0][0]);

	_float fSize = 0.f;

	switch (m_iType)
	{
	case 0:
		pEffect->vAngle = MyVec3(-90.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(131.f, 149.f, 250.f);
		pEffect->vColorDelta = MyVec3(255.f, 150.f, 121.f) - pEffect->vColor;
		pEffect->fMaxLife = Engine::GetRandomFloat(0.3f, 0.4f);
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(1.f, 2.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 4.f;
		
		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.5f;
		pEffect->fAlphaWayDelta = 120.f;

		pEffect->fRollDelta = Engine::GetRandomFloat(-100.f, 100.f);

		pEffect->bAlive = true;
		break;

	case 1:
		pEffect->vAngle = MyVec3(-90.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(131.f, 149.f, 250.f);
		pEffect->vColorDelta = MyVec3(255.f, 150.f, 121.f) - pEffect->vColor;
		pEffect->fMaxLife = Engine::GetRandomFloat(0.3f, 0.4f);
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = 15.f;
		pEffect->fRadius = 0.f;
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(0.5f, 0.7f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 0.6f;
		pEffect->fAlphaDelta = 0.f;
		pEffect->fRollDelta = 560.f;

		pEffect->bAlive = true;
		break;

	case 2:
		pEffect->vAngle = MyVec3(-90.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(131.f, 149.f, 250.f);
		pEffect->vColorDelta = MyVec3(255.f, 150.f, 121.f) - pEffect->vColor;
		pEffect->fMaxLife = Engine::GetRandomFloat(0.3f, 0.4f);
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = 5.f;
		pEffect->fRadius = 0.f;
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(0.5f, 0.7f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 1.f;
		pEffect->fAlphaDelta = 0.f;
		pEffect->fRollDelta = 560.f;

		pEffect->bAlive = true;
		break;

	case 3:
		pEffect->vAngle = MyVec3(-90.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(131.f, 149.f, 250.f);
		pEffect->vColorDelta = MyVec3(255.f, 150.f, 121.f) - pEffect->vColor;
		pEffect->fMaxLife = Engine::GetRandomFloat(0.3f, 0.4f);
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = 5.f;
		pEffect->fRadius = 0.f;
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(0.5f, 0.7f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 0.7f;
		pEffect->fAlphaDelta = 0.f;
		pEffect->fRollDelta = 560.f;

		pEffect->bAlive = true;
		break;

	case 4:
		pEffect->vAngle = MyVec3(-110.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 255.f, 255.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.3f, 0.4f);
		pEffect->fSpeed = Engine::GetRandomFloat(2.f, 4.f);
		pEffect->fAngle = 25.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = Engine::GetRandomFloat(0.f, 15.f);
		pEffect->vDir.x = pEffect->fRadius * cosf(pEffect->fAngle);
		pEffect->vDir.y = pEffect->fRadius * sinf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(2.f, 3.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->bAlive = true;
		break;

	case 5:
		pEffect->vAngle = MyVec3(-90, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 255.f, 255.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.4f, 0.6f);
		pEffect->fSpeed = Engine::GetRandomFloat(2.f, 4.f);
		pEffect->fAngle = 10.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = Engine::GetRandomFloat(0.f, 15.f);
		pEffect->vDir.x = pEffect->fRadius * cosf(pEffect->fAngle);
		pEffect->vDir.y = pEffect->fRadius * sinf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(2.f, 3.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.9f;
		pEffect->fAlphaWayDelta = 255.f;

		pEffect->bAlive = true;
		break;

	case 6:
		pEffect->vAngle = MyVec3(-90, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 255.f, 255.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.2f, 0.3f);
		pEffect->fSpeed = Engine::GetRandomFloat(5.f, 10.f);
		pEffect->fAngle = 180.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = Engine::GetRandomFloat(0.f, 15.f);
		pEffect->vDir.x = pEffect->fRadius * cosf(pEffect->fAngle);
		pEffect->vDir.y = pEffect->fRadius * sinf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(2.f, 4.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fAlphaDelta = 0.f;

		pEffect->bAlive = true;
		break;

	case 7:
		pEffect->vAngle = MyVec3(-90, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 155.f, 135.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.05f, 0.1f);
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = 180.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = Engine::GetRandomFloat(0.f, 15.f);
		pEffect->vDir.x = pEffect->fRadius * cosf(pEffect->fAngle);
		pEffect->vDir.y = pEffect->fRadius * sinf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(1.f, 1.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = -0.6f;
		pEffect->fSizeWayTime = 0.3f;
		pEffect->fSizeWayDelta = 0.5f;

		pEffect->bAlive = true;
		break;

	case 8:
		pEffect->vAngle = MyVec3(-90, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 150.f, 121.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.2f, 0.3f);
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = 180.f;
		pEffect->fRadius = Engine::GetRandomFloat(10.f, 65.f);
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = Engine::GetRandomFloat(0.f, 15.f);
		pEffect->vDir.x = pEffect->fRadius * cosf(pEffect->fAngle);
		pEffect->vDir.y = pEffect->fRadius * sinf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(3.f, 4.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = *(MyVec3*)&m_pTarget->m[3][0] + vRight * (_float)iIndex * 0.2f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.3f;
		pEffect->fAlphaWayDelta = 20.f;

		pEffect->bAlive = true;
		break;

	case 10:
		pEffect->vAngle = MyVec3(110.f, 0.f, 23.f + Engine::GetRandomFloat(160.f, 270.f));
		pEffect->vColor = MyVec3(117.f, 108.f, 91.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.2f, 0.25f);
		pEffect->fSpeed = 0.1f;
		pEffect->fAngle = 10.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 5.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(30.f, 35.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = m_vOrigin + MyVec3(0.f, 1.f, 0.f);

		pEffect->fSizeDelta = 35.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.6f;
		pEffect->fAlphaWayDelta = 255.f;

		pEffect->fRollDelta = 260.f;

		pEffect->bAlive = true;
		break;

	case 11:
		pEffect->vAngle = MyVec3(110.f, 0.f, 23.f + Engine::GetRandomFloat(240.f, 300.f));
		pEffect->vColor = MyVec3(255.f, 165.f, 135.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.25f, 0.4f);
		pEffect->fSpeed = 0.1f;
		pEffect->fAngle = 5.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 5.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(30.f, 35.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = m_vOrigin + MyVec3(0.f, 1.f, 0.f);

		pEffect->fSizeDelta = 35.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.6f;
		pEffect->fAlphaWayDelta = 255.f;

		pEffect->fRollDelta = 260.f;

		pEffect->bAlive = true;
		break;

	/*case 12:
		pEffect->vAngle = MyVec3(-45.f, 0.f, 0.f);
		pEffect->vColor = MyVec3(255.f, 255.f, 255.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.5f, 0.6f);
		pEffect->fSpeed = Engine::GetRandomFloat(18.f, 20.f);
		pEffect->fAngle = 20.f;
		pEffect->fRadius = 0.f;
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(2.f, 5.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.9f;
		pEffect->fAlphaWayDelta = 230.f;

		pEffect->bAlive = true;
		break;

	case 13:
		pEffect->vAngle = MyVec3(-135.f, 0.f, 0.f);
		pEffect->vColor = MyVec3(255.f, 255.f, 255.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.5f, 0.6f);
		pEffect->fSpeed = Engine::GetRandomFloat(70.f, 180.f);
		pEffect->fAngle = 20.f;
		pEffect->fRadius = 0.f;
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(2.f, 5.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.9f;
		pEffect->fAlphaWayDelta = 230.f;

		pEffect->bAlive = true;
		break;

	case 14:
		pEffect->vAngle = MyVec3(35.f, 0.f, Engine::GetRandomFloat(-210.f, -240.f));
		pEffect->vColor = MyVec3(255.f, 180.f, 180.f);
		pEffect->fMaxLife = 0.1f;
		pEffect->fSpeed = Engine::GetRandomFloat(180.f, 250.f);
		pEffect->fAngle = 25.f;
		pEffect->fRadius = 0.f;
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(2.f, 5.f);
		pEffect->vScale = MyVec3(Engine::GetRandomFloat(1.f, 5.f) * fSize, Engine::GetRandomFloat(10.f, 20.f) * fSize, 1.f);
		pEffect->vPos = *D3DXVec3Normalize(&pEffect->vPos, (MyVec3*)&m_pTarget->m[2][0]) * 15.f + vPos + MyVec3(0.f, 5.f, 0.f);

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.8f;
		pEffect->fAlphaWayDelta = 230.f;

		pEffect->bAlive = true;
		break;*/

	// Slash01
	case 20:
		pEffect->vAngle = MyVec3(35.f, 90.f, -5.f + Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 185.f, 145.f);
		pEffect->fMaxLife = 0.15f;
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = 1.f;
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		pEffect->vScale = MyVec3(Engine::GetRandomFloat(30.f, 33.f), Engine::GetRandomFloat(19.f, 22.f), 1.f);
		pEffect->vPos = *D3DXVec3Normalize(&pEffect->vPos, (MyVec3*)&m_pTarget->m[2][0]) * 30.f + vPos;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.8f;
		pEffect->fAlphaWayDelta = 250.f;

		pEffect->fRollDelta = 35.f;

		pEffect->bAlive = true;
		break;

	case 21:
		pEffect->vAngle = MyVec3(35.f, 90.f, -5.f + Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 185.f, 145.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.3f, 0.4f);
		pEffect->fSpeed = Engine::GetRandomFloat(4.f, 6.f);
		pEffect->fAngle = 10.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 30.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		pEffect->vScale = MyVec3(Engine::GetRandomFloat(3.f, 5.f), Engine::GetRandomFloat(8.f, 16.f), 1.f);
		pEffect->vPos = *D3DXVec3Normalize(&pEffect->vPos, (MyVec3*)&m_pTarget->m[2][0]) * 30.f + vPos;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.1f;
		pEffect->fAlphaWayDelta = 220.f;

		pEffect->fRollDelta = 35.f;

		pEffect->bAlive = true;
		break;

	case 22:
		pEffect->vAngle = MyVec3(35.f, 90.f, -5.f + Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 185.f, 145.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.15f, 0.3f);
		pEffect->fSpeed = Engine::GetRandomFloat(2.8f, 3.8f);
		pEffect->fAngle = 10.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 40.f);
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		pEffect->vScale = MyVec3(Engine::GetRandomFloat(5.f, 7.f), Engine::GetRandomFloat(24.f, 28.f), 1.f);
		pEffect->vPos = *D3DXVec3Normalize(&pEffect->vPos, (MyVec3*)&m_pTarget->m[2][0]) * 30.f + vPos;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.5f;
		pEffect->fAlphaWayDelta = 220.f;

		pEffect->fRollDelta = 35.f;

		pEffect->bAlive = true;
		break;

	// Slash02
	case 23:
		pEffect->vAngle = MyVec3(30.f, 140.f, -120.f + Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 155.f, 125.f);
		pEffect->fMaxLife = 0.45f;
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = 10.f;
		pEffect->fRadius = 1.f;
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		pEffect->vScale = MyVec3(Engine::GetRandomFloat(35.f, 37.f), Engine::GetRandomFloat(19.f, 20.f), 1.f);
		pEffect->vPos = *D3DXVec3Normalize(&pEffect->vPos, (MyVec3*)&m_pTarget->m[2][0]) * 30.f + vPos;

		pEffect->fSizeDelta = -10.f;
		pEffect->fSizeWayTime = 0.2f;
		pEffect->fSizeWayDelta = 10.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.1f;
		pEffect->fAlphaWayDelta = 220.f;

		pEffect->fRollDelta = 35.f;

		pEffect->bAlive = true;
		break;

	case 24:
		pEffect->vAngle = MyVec3(30.f, 90.f, -5.f + Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 155.f, 135.f);
		pEffect->fMaxLife = 0.15f;
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = 5.f;
		pEffect->fRadius = 1.f;
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		pEffect->vScale = MyVec3(Engine::GetRandomFloat(33.f, 33.f), Engine::GetRandomFloat(19.f, 20.f), 1.f);
		pEffect->vPos = *D3DXVec3Normalize(&pEffect->vPos, (MyVec3*)&m_pTarget->m[2][0]) * 30.f + vPos;

		pEffect->fSizeDelta = -10.f;
		pEffect->fSizeWayTime = 0.2f;
		pEffect->fSizeWayDelta = 10.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.1f;
		pEffect->fAlphaWayDelta = 220.f;

		pEffect->fRollDelta = 35.f;

		pEffect->bAlive = true;
		break;

	case 25:
		pEffect->vAngle = MyVec3(30.f, 90.f, -5.f + Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 155.f, 135.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.35f, 0.5f);
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = 10.f;
		pEffect->fRadius = 1.f;
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		pEffect->vScale = MyVec3(Engine::GetRandomFloat(33.f, 33.f), Engine::GetRandomFloat(7.f, 9.f), 1.f);
		pEffect->vPos = *D3DXVec3Normalize(&pEffect->vPos, (MyVec3*)&m_pTarget->m[2][0]) * 30.f + vPos;

		pEffect->fSizeDelta = -3.f;
		pEffect->fSizeWayTime = 0.2f;
		pEffect->fSizeWayDelta = 3.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.1f;
		pEffect->fAlphaWayDelta = 150.f;

		pEffect->fRollDelta = 35.f;

		pEffect->bAlive = true;
		break;

	case 26:
		pEffect->vAngle = MyVec3(30.f, 90.f, -5.f + Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 155.f, 115.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.15f, 0.2f);
		pEffect->fSpeed = Engine::GetRandomFloat(2.8f, 3.8f);
		pEffect->fAngle = 15.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 40.f);
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		pEffect->vScale = MyVec3(Engine::GetRandomFloat(5.f, 7.f), Engine::GetRandomFloat(9.f, 12.f), 1.f);
		pEffect->vPos = *D3DXVec3Normalize(&pEffect->vPos, (MyVec3*)&m_pTarget->m[2][0]) * 30.f + vPos;

		pEffect->fSizeDelta = -3.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.8f;
		pEffect->fAlphaWayDelta = 220.f;

		pEffect->fRollDelta = 35.f;

		pEffect->bAlive = true;
		break;

	case 27:
		pEffect->vAngle = MyVec3(30.f, 90.f, -5.f + Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 235.f, 185.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.1f, 0.2f);
		pEffect->fSpeed = Engine::GetRandomFloat(7.f, 9.f);
		pEffect->fAngle = 3.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 40.f);
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		pEffect->vScale = MyVec3(Engine::GetRandomFloat(3.f, 4.f), Engine::GetRandomFloat(5.6f, 8.8f), 1.f);
		pEffect->vPos = *D3DXVec3Normalize(&pEffect->vPos, (MyVec3*)&m_pTarget->m[2][0]) * 30.f + vPos;

		pEffect->fSizeDelta = -5.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.5f;
		pEffect->fAlphaWayDelta = 220.f;

		pEffect->fRollDelta = 35.f;

		pEffect->bAlive = true;
		break;

	case 28:
		pEffect->vAngle = MyVec3(30.f, 90.f, -5.f + Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 235.f, 185.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.1f, 0.2f);
		pEffect->fSpeed = Engine::GetRandomFloat(7.f, 9.f);
		pEffect->fAngle = 3.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 40.f);
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		pEffect->vScale = MyVec3(Engine::GetRandomFloat(3.f, 4.f), Engine::GetRandomFloat(5.6f, 8.8f), 1.f);
		pEffect->vPos = *D3DXVec3Normalize(&pEffect->vPos, (MyVec3*)&m_pTarget->m[2][0]) * 30.f + vPos;

		pEffect->fSizeDelta = -5.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.5f;
		pEffect->fAlphaWayDelta = 220.f;

		pEffect->fRollDelta = 35.f;

		pEffect->bAlive = true;
		break;

	case 29:
		pEffect->vAngle = MyVec3(30.f, 90.f, -5.f + Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 235.f, 185.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.08f, 0.1f);
		pEffect->fSpeed = Engine::GetRandomFloat(9.f, 11.f);
		pEffect->fAngle = 5.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 40.f);
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		pEffect->vScale = MyVec3(Engine::GetRandomFloat(2.f, 4.f), Engine::GetRandomFloat(5.6f, 14.3f), 1.f);
		pEffect->vPos = *D3DXVec3Normalize(&pEffect->vPos, (MyVec3*)&m_pTarget->m[2][0]) * 30.f + vPos;

		pEffect->fSizeDelta = 0.1f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.5f;
		pEffect->fAlphaWayDelta = 220.f;

		pEffect->fRollDelta = 35.f;

		pEffect->bAlive = true;
		break;

	case 30:
		pEffect->vAngle = MyVec3(-90.f, 90.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 155.f, 115.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.2f, 0.25f);
		pEffect->fSpeed = Engine::GetRandomFloat(7.f, 9.f);
		pEffect->fAngle = 15.f;
		pEffect->fRadius = 1.f;
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		pEffect->vScale = MyVec3(Engine::GetRandomFloat(2.f, 3.f), Engine::GetRandomFloat(190.f, 195.f), 1.f);
		pEffect->vPos = *D3DXVec3Normalize(&pEffect->vPos, (MyVec3*)&m_pTarget->m[2][0]) * 30.f + vPos;

		pEffect->fSizeDelta = -55.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.4f;
		pEffect->fAlphaWayDelta = 250.f;

		pEffect->fRollDelta = 35.f;

		pEffect->bAlive = true;
		break;

	case 31:
		pEffect->vAngle = MyVec3(-90.f, 90.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 155.f, 115.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.2f, 0.25f);
		pEffect->fSpeed = Engine::GetRandomFloat(7.f, 9.f);
		pEffect->fAngle = 15.f;
		pEffect->fRadius = 1.f;
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		pEffect->vScale = MyVec3(Engine::GetRandomFloat(2.f, 3.f), Engine::GetRandomFloat(19.f, 20.f), 1.f);
		pEffect->vPos = *D3DXVec3Normalize(&pEffect->vPos, (MyVec3*)&m_pTarget->m[2][0]) * 30.f + vPos;

		pEffect->fSizeDelta = -5.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.4f;
		pEffect->fAlphaWayDelta = 250.f;

		pEffect->fRollDelta = 35.f;

		pEffect->bAlive = true;
		break;

	case 32:
		pEffect->vAngle = MyVec3(0.f, 0.f, 180.f);
		pEffect->vColor = MyVec3(255.f, 155.f, 115.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.15f, 0.2f);
		pEffect->fSpeed = Engine::GetRandomFloat(14.f, 15.f);
		pEffect->fAngle = 25.f;
		pEffect->fRadius = Engine::GetRandomFloat(10.f, 30.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		pEffect->vScale = MyVec3(Engine::GetRandomFloat(20.f, 30.f), Engine::GetRandomFloat(45.f, 57.f), 1.f);
		pEffect->vPos = *D3DXVec3Normalize(&pEffect->vPos, (MyVec3*)&m_pTarget->m[2][0]) * 30.f + vPos + MyVec3(0.f, 20.f, 0.f);

		pEffect->fSizeDelta = -40.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.5f;
		pEffect->fAlphaWayDelta = 220.f;

		pEffect->fRollDelta = 35.f;

		pEffect->bAlive = true;
		break;

	case 33:
		pEffect->vAngle = MyVec3(-90.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 155.f, 115.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.1f, 0.15f);
		pEffect->fSpeed = Engine::GetRandomFloat(14.f, 15.f);
		pEffect->fAngle = 25.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 5.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(10.f, 13.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = *D3DXVec3Normalize(&pEffect->vPos, (MyVec3*)&m_pTarget->m[2][0]) * 30.f + vPos;

		pEffect->fSizeDelta = 11.f;
		pEffect->fSizeWayTime = 0.4f;
		pEffect->fSizeWayDelta = 27.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.5f;
		pEffect->fAlphaWayDelta = 250.f;

		pEffect->fRollDelta = 135.f;

		pEffect->bAlive = true;
		break;

	case 34:
		pEffect->vAngle = MyVec3(-90.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 155.f, 115.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.1f, 0.15f);
		pEffect->fSpeed = Engine::GetRandomFloat(14.f, 15.f);
		pEffect->fAngle = 25.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 5.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(18.f, 24.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = *D3DXVec3Normalize(&pEffect->vPos, (MyVec3*)&m_pTarget->m[2][0]) * 30.f + vPos;

		pEffect->fSizeDelta = 25.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.5f;
		pEffect->fAlphaWayDelta = 250.f;

		pEffect->fRollDelta = 135.f;

		pEffect->bAlive = true;
		break;

	case 35:
		pEffect->vAngle = MyVec3(-90.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 155.f, 115.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.1f, 0.15f);
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = 25.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 5.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(18.f, 24.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = *D3DXVec3Normalize(&pEffect->vPos, (MyVec3*)&m_pTarget->m[2][0]) * 30.f + vPos;

		pEffect->fSizeDelta = 11.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.5f;
		pEffect->fAlphaWayDelta = 250.f;

		pEffect->fRollDelta = 135.f;

		pEffect->bAlive = true;
		break;

	case 36:
		pEffect->vAngle = MyVec3(-90.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 155.f, 115.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.4f, 0.6f);
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = 25.f;
		pEffect->fRadius = Engine::GetRandomFloat(10.f, 35.f);
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = Engine::GetRandomFloat(0.f, 135.f);
		pEffect->vDir.x = pEffect->fRadius * cosf(pEffect->fAngle);
		pEffect->vDir.y = pEffect->fRadius * sinf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(48.f, 64.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = *D3DXVec3Normalize(&pEffect->vPos, (MyVec3*)&m_pTarget->m[2][0]) * 30.f + vPos;

		pEffect->fSizeDelta = 51.f;
		pEffect->fSizeWayTime = 0.4f;
		pEffect->fSizeWayDelta = 47.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.2f;
		pEffect->fAlphaWayDelta = 80.f;

		pEffect->fRollDelta = 135.f;

		pEffect->bAlive = true;
		break;

	case 37:
		pEffect->vAngle = MyVec3(-90.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 155.f, 115.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.25f, 0.35f);
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = 10.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 5.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(15.f, 21.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = *D3DXVec3Normalize(&pEffect->vPos, (MyVec3*)&m_pTarget->m[2][0]) * 30.f + vPos;

		pEffect->fSizeDelta = 35.f;
		pEffect->fSizeWayTime = 0.4f;
		pEffect->fSizeWayDelta = 27.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.5f;
		pEffect->fAlphaWayDelta = 220.f;

		pEffect->fRollDelta = 135.f;

		pEffect->bAlive = true;
		break;

	case 40:
		pEffect->vAngle = MyVec3(-90.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 155.f, 115.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.3f, 0.4f);
		pEffect->fSpeed = 0.f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = 0.f;
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(4.f, 5.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 4.f;
		pEffect->fSizeWayTime = 0.2f;
		pEffect->fSizeWayDelta = 6.f;

		pEffect->fAlphaDelta = 0.f;

		pEffect->fRollDelta = 10.f;

		pEffect->bAlive = true;
		break;

	case 41:
		pEffect->vAngle = MyVec3(-90.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 155.f, 115.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.2f, 0.25f);
		pEffect->fSpeed = 0.f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = 0.f;
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(3.f, 5.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 3.f;
		pEffect->fSizeWayTime = 0.2f;
		pEffect->fSizeWayDelta = 14.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.6f;
		pEffect->fAlphaWayDelta = 250.f;

		pEffect->fRollDelta = 10.f;

		pEffect->bAlive = true;
		break;

	case 42:
		pEffect->vAngle = MyVec3(-90.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 155.f, 115.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.3f, 0.35f);
		pEffect->fSpeed = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fAngle = 180.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(5.f, 7.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 11.f;
		pEffect->fSizeWayTime = 0.4f;
		pEffect->fSizeWayDelta = 8.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.6f;
		pEffect->fAlphaWayDelta = 250.f;

		pEffect->fRollDelta = 180.f;

		pEffect->bAlive = true;
		break;

	case 43:
		pEffect->vAngle = MyVec3(-90.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 155.f, 115.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.3f, 0.35f);
		pEffect->fSpeed = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fAngle = 180.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(4.f, 7.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 11.f;
		pEffect->fSizeWayTime = 0.4f;
		pEffect->fSizeWayDelta = 6.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.3f;
		pEffect->fAlphaWayDelta = 250.f;

		pEffect->fRollDelta = 30.f;

		pEffect->bAlive = true;
		break;

	case 44:
		pEffect->vAngle = MyVec3(-90.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 255.f, 255.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.25f, 0.3f);
		pEffect->fSpeed = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fAngle = 180.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(5.f, 8.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 11.f;
		pEffect->fSizeWayTime = 0.4f;
		pEffect->fSizeWayDelta = 6.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.3f;
		pEffect->fAlphaWayDelta = 250.f;

		pEffect->fRollDelta = 30.f;

		pEffect->bAlive = true;
		break;

	case 45:
		pEffect->vAngle = MyVec3(-90.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 155.f, 115.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.3f, 0.5f);
		pEffect->fSpeed = Engine::GetRandomFloat(15.f, 25.f);
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		pEffect->vScale = MyVec3(Engine::GetRandomFloat(1.f, 1.5f), Engine::GetRandomFloat(2.f, 3.f), 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 0.25f;
		pEffect->fSizeWayTime = 0.4f;
		pEffect->fSizeWayDelta = 0.2f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.4f;
		pEffect->fAlphaWayDelta = 250.f;

		pEffect->fRollDelta = 180.f;

		pEffect->bAlive = true;
		break;

	case 46:
		pEffect->vAngle = MyVec3(-90.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 155.f, 115.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.5f, 0.7f);
		pEffect->fSpeed = Engine::GetRandomFloat(2.f, 5.f);
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(5.f, 15.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		pEffect->vScale = MyVec3(Engine::GetRandomFloat(10.5f, 10.7f), Engine::GetRandomFloat(5.f, 7.f), 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 0.25f;
		pEffect->fSizeWayTime = 0.4f;
		pEffect->fSizeWayDelta = 0.2f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.4f;
		pEffect->fAlphaWayDelta = 250.f;

		pEffect->fRollDelta = 180.f;

		pEffect->bAlive = true;
		break;
	}

	pEffect->vAngle += m_vAngle;
	pEffect->fAlphaDelta -= 255.f;
	m_EffectList.push_back(pEffect);
}


HRESULT CRuinBlade::Ready_Component( void )
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

	m_pTextureCom = (Engine::CTexture*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, m_pEffectKey);
	if (m_pTextureCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(Engine::CGameObject::UPDATE_NONE, L"Com_Texture", m_pTextureCom)))
		return E_FAIL;

	m_pBufferCom = (Engine::CRcTex*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STATIC, L"Component_Buffer_RcTex");
	if (m_pBufferCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(Engine::CGameObject::UPDATE_NONE, L"Com_Buffer", m_pBufferCom)))
		return E_FAIL;

	m_pShaderCom = (Engine::CShader*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Shader_Particle");
	if (m_pShaderCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(Engine::CGameObject::UPDATE_NONE, L"Com_Shader", m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CRuinBlade::Free( void )
{
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pTextureCom);
	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pShaderCom);

	Engine::CGameObject::Free();
}

CRuinBlade* CRuinBlade::Create( LPDIRECT3DDEVICE9 pDevice, const _uint& iType, MyMat* pTarget, MyMat* pBone)
{
	CRuinBlade* pRuinBlade = new CRuinBlade(pDevice);

	if (FAILED(pRuinBlade->Initialize(iType, pTarget, pBone)))
	{
		MSG_BOX("RuinBlade Created Failed");
		Engine::Safe_Release(pRuinBlade);
	}
	return pRuinBlade;
}



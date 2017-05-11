#include "StdAfx.h"
#include "LordOfMana.h"
#include "DefaultCamera.h"

CLordOfMana::CLordOfMana( LPDIRECT3DDEVICE9 pDevice)
: CEffect(pDevice)
, m_fRadius(0.f)
, m_fRadiusDelta(0.f)
, m_iCountDelta(0)
{
	
}

CLordOfMana::~CLordOfMana( void )
{

}

HRESULT CLordOfMana::Initialize( const _uint& iType)
{
	m_iType = iType;
	m_pBone = ((Engine::CDynamicMesh*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(0)->GetComponent(L"Com_Mesh"))->GetPartsMatrix("ValveBiped_Bip01_L_Hand");
	m_pTarget = ((Engine::CTransform*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(0)->GetComponent(L"Com_Transform"))->GetWorldMatrix();
	m_vAngle = *((CDefaultCamera*)Engine::Get_CameraMgr()->GetCamera(L"Cam_Default"))->GetAngle();

	m_vOrigin = *(MyVec3*)&(*m_pBone * *m_pTarget).m[3][0];


	DefaultSetting();

	if (FAILED(Ready_Component()))
		return E_FAIL;

	for (_uint i = 0; i < m_iCount; ++i)
		AddParticle();

	return S_OK;
}

_int CLordOfMana::Update( const _float& fDelta )
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
				AddParticle();

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

void CLordOfMana::Render( void )
{
	LPD3DXEFFECT pEffect = m_pShaderCom->GetEffect();
	if (pEffect == NULL)
		return;

	SetUpShader(pEffect);
}

void CLordOfMana::SetUpShader( LPD3DXEFFECT pEffect )
{
	pEffect->AddRef();

	MyMat matView, matProj;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	m_pTextureCom->SetUpShader(pEffect, m_iIndex);
	
	pEffect->Begin(NULL, 0);
	if (m_iType == 8 || m_iType == 16 || m_iType == 17 || m_iType == 30 || m_iType == 40)
		pEffect->BeginPass(5);
	else if (m_iType <= 10)
		pEffect->BeginPass(2);
	else
		pEffect->BeginPass(1);

	EFFECTLIST::iterator iter = m_EffectList.begin();
	EFFECTLIST::iterator iter_end = m_EffectList.end();
	MyMat matScale, matRotX, matRotY, matRotZ, matTrans, matWorld;

	MyMat matBill;
	D3DXMatrixIdentity(&matBill);
	matBill._11 = matView._11;
	matBill._13 = matView._13;
	matBill._31 = matView._31;
	matBill._33 = matView._33;
	D3DXMatrixInverse(&matBill, NULL, &matBill);

	for ( ; iter != iter_end; ++iter)
	{
		if ((*iter)->bAlive == true)
		{
			_float	fSizeDelta = GetSizeDelta(*iter);
			_float	fRollDelta = GetRollDelta(*iter);
			_uint	iAlphaDelta = GetAlphaDelta(*iter);
			MyVec3	vScaleDelta = GetScaleDelta(*iter);
			D3DXMatrixScaling(&matScale, (*iter)->vScale.x + vScaleDelta.x + fSizeDelta, (*iter)->vScale.y +  vScaleDelta.y + fSizeDelta, (*iter)->vScale.z);
			D3DXMatrixRotationX(&matRotX, D3DXToRadian((*iter)->vAngle.x));
			D3DXMatrixRotationY(&matRotY, D3DXToRadian((*iter)->vAngle.y));
			D3DXMatrixRotationZ(&matRotZ, D3DXToRadian((*iter)->vAngle.z + fRollDelta));
			D3DXMatrixTranslation(&matTrans, (*iter)->vPos.x, (*iter)->vPos.y, (*iter)->vPos.z);
			if (m_iType == 8 || m_iType >= 40)
				matRotY = matBill;
			matWorld = matScale * matRotZ * matRotX * matRotY *  matTrans;
			pEffect->SetMatrix("g_matWorld", &matWorld);
			pEffect->SetVector("g_vColor", &MyVec4(D3DXCOLOR(D3DCOLOR_ARGB(255 - iAlphaDelta, (_int)(*iter)->vColor.x, (_int)(*iter)->vColor.y, (_int)(*iter)->vColor.z))));
			pEffect->CommitChanges();
			m_pBufferCom->Render();
		}
	}
	
	pEffect->EndPass();
	pEffect->End();

	Engine::Safe_Release(pEffect);
}

void CLordOfMana::DefaultSetting( void )
{
	switch (m_iType)
	{
	// MirrorHand 01
	case 0: // 빛1
		m_pEffectKey = L"Component_Texture_LightGlow";
		m_iCount = 1;
		m_iIndex = 2;

		m_fInterval = 0.04f;
		m_fDuration = 3.5f;
		break;

	case 1: // 빛1
		m_pEffectKey = L"Component_Texture_LightGlow";
		m_iCount = 1;
		m_iIndex = 2;

		m_fInterval = 0.04f;
		m_fDuration = 4.f;
		break;

	case 2: // 번쩍오라1
		m_pEffectKey = L"Component_Texture_ShipGray";
		m_iCount = 2;
		m_iIndex = 1;

		m_fInterval = 0.05f;
		m_fDuration = 1.5f;
		break;

	case 3: // 번쩍오라2
		m_pEffectKey = L"Component_Texture_LightRay";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.08f;
		m_fDuration = 1.5f;
		break;

	case 4: // 스파크
		m_pEffectKey = L"Component_Texture_PartiSpray";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.08f;
		m_fDuration = 1.5f;
		break;

	case 5: // 범위1
		m_pEffectKey = L"Component_Texture_IceLight";
		m_iCount = 5;
		m_iIndex = 1;

		m_fInterval = 0.05f;
		m_fDuration = 4.3f;
		break;

	case 6: // 범위2
		m_pEffectKey = L"Component_Texture_IceLight";
		m_iCount = 4;
		m_iIndex = 1;

		m_fInterval = 0.06f;
		m_fDuration = 4.3f;
		break;

	case 7: // 범위3
		m_pEffectKey = L"Component_Texture_Ring_Outer_Wind";
		m_iCount = rand() % 7 + 7;
		m_iIndex = 3;

		m_fInterval = 0.015f;
		m_fDuration = 4.5f;
		break;

	case 8: // 범위1
		m_pEffectKey = L"Component_Texture_Light_n";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.02f;
		m_fDuration = 0.5f;
		break;

	case 9: // 마법진3
		m_pEffectKey = L"Component_Texture_AmingPoint";
		m_iCount = 1;
		m_iIndex = 6;

		m_fInterval = 0.06f;
		m_fDuration = 1.8f;
		break;

	// MirrorGround01
	case 10: // 링1
		m_pEffectKey = L"Component_Texture_Ring";
		m_iCount = 1;
		m_iIndex = 5;

		m_fInterval = 0.6f;
		m_fDuration = 0.5f;
		break;

	case 11: // 마법진1
		m_pEffectKey = L"Component_Texture_AmingPoint";
		m_iCount = 1;
		m_iIndex = 4;

		m_fInterval = 0.6f;
		m_fDuration = 0.5f;
		break;

	case 12: // 마법진2
		m_pEffectKey = L"Component_Texture_AmingPoint";
		m_iCount = 1;
		m_iIndex = 3;

		m_fInterval = 0.6f;
		m_fDuration = 0.5f;
		break;

	case 13: // 마법진2
		m_pEffectKey = L"Component_Texture_AmingPoint";
		m_iCount = 1;
		m_iIndex = 3;

		m_fInterval = 0.6f;
		m_fDuration = 0.5f;
		break;

	case 14: // 마법진2
		m_pEffectKey = L"Component_Texture_Ring";
		m_iCount = 2;
		m_iIndex = 4;

		m_fInterval = 0.6f;
		m_fDuration = 0.5f;
		break;

	case 15: // 마법진2
		m_pEffectKey = L"Component_Texture_Ring";
		m_iCount = 2;
		m_iIndex = 6;

		m_fInterval = 0.6f;
		m_fDuration = 0.5f;
		break;

	case 16: // 퍼지는링굴절1
		m_pEffectKey = L"Component_Texture_Light_n";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.14f;
		m_fDuration = 0.15f;
		break;

	case 17: // 퍼지는링굴절2
		m_pEffectKey = L"Component_Texture_Ring_Outer_Wind_n";
		m_iCount = 2;
		m_iIndex = 0;

		m_fInterval = 0.03f;
		m_fDuration = 0.8f;
		break;

	case 18: // 퍼지는링1
		m_pEffectKey = L"Component_Texture_Ring";
		m_iCount = 3;
		m_iIndex = 5;

		m_fInterval = 0.17f;
		m_fDuration = 0.15f;
		break;

	case 19: // 퍼지는링2
		m_pEffectKey = L"Component_Texture_Ring_Outer_Wind";
		m_iCount = 1;
		m_iIndex = 2;

		m_fInterval = 0.03f;
		m_fDuration = 0.5f;
		break;

	case 20: // 퍼지는링3
		m_pEffectKey = L"Component_Texture_RingLine";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.03f;
		m_fDuration = 0.25f;
		break;

	case 21: // 퍼지는링4
		m_pEffectKey = L"Component_Texture_IceLight";
		m_iCount = 1;
		m_iIndex = 2;

		m_fInterval = 0.05f;
		m_fDuration = 0.5f;
		break;

	case 22: // 퍼지는링5
		m_pEffectKey = L"Component_Texture_IceLight";
		m_iCount = 1;
		m_iIndex = 1;

		m_fInterval = 0.07f;
		m_fDuration = 0.5f;
		break;

	case 23: // 바닥빛
		m_pEffectKey = L"Component_Texture_Light";
		m_iCount = 2;
		m_iIndex = 0;

		m_fInterval = 0.6f;
		m_fDuration = 0.5f;
		break;

	// MirrorHand 02
	case 30: // 빛1
		m_pEffectKey = L"Component_Texture_Ring_Outer_Wind_n";
		m_iCount = rand() % 2 + 1;
		m_iIndex = 0;

		m_fInterval = 0.04f;
		m_fDuration = 3.5f;
		break;

	case 31: // 빛1
		m_pEffectKey = L"Component_Texture_Ring_Outer_Wind";
		m_iCount = 5;
		m_iIndex = 3;

		m_fInterval = 0.04f;
		m_fDuration = 3.5f;
		break;

	case 32: // 빛1
		m_pEffectKey = L"Component_Texture_WindBlade";
		m_iCount = 4;
		m_iIndex = 0;

		m_fInterval = 0.04f;
		m_fDuration = 3.5f;
		break;

	case 33: // 빛1
		m_pEffectKey = L"Component_Texture_LightRay";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.05f;
		m_fDuration = 3.5f;
		break;

	case 34: // 마법진2
		m_pEffectKey = L"Component_Texture_AmingPoint";
		m_iCount = 1;
		m_iIndex = 6;

		m_fInterval = 0.06f;
		m_fDuration = 1.5f;
		break;

	case 35: // 마법진2
		m_pEffectKey = L"Component_Texture_AmingPoint";
		m_iCount = 1;
		m_iIndex = 6;

		m_fInterval = 0.6f;
		m_fDuration = 0.5f;
		break;

	// resonance bomb
	case 40: // 굴절
		m_pEffectKey = L"Component_Texture_efxRipple";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.6f;
		m_fDuration = 0.5f;
		break;

	case 41: // 불꽃덩어리1
		m_pEffectKey = L"Component_Texture_Bomb";
		m_iCount = 2;
		m_iIndex = 1;

		m_fInterval = 0.6f;
		m_fDuration = 0.5f;
		break;

	case 42: // 불꽃덩어리2
		m_pEffectKey = L"Component_Texture_ShipGray";
		m_iCount = 2;
		m_iIndex = 1;

		m_fInterval = 0.02f;
		m_fDuration = 0.15f;
		break;

	case 43: // 불꽃덩어리2
		m_pEffectKey = L"Component_Texture_IceLight";
		m_iCount = rand() % 9 + 3;
		m_iIndex = 1;

		m_fInterval = 0.04f;
		m_fDuration = 0.15f;
		break;

	case 44: // 플래쉬
		m_pEffectKey = L"Component_Texture_Flash";
		m_iCount = 15;
		m_iIndex = 2;

		m_fInterval = 0.04f;
		m_fDuration = 0.15f;
		break;
	}
}

void CLordOfMana::AddParticle( void )
{
	EFFECT* pEffect= new EFFECT;
	ZeroMemory(pEffect, sizeof(EFFECT));

	pEffect->bAlive = true;
	pEffect->vScale = MyVec3(1.f, 1.f, 1.f);
	D3DXVECTOR3 vMin(131.f, 149.f, 250.f);
	D3DXVECTOR3 vMax(181.f, 150.f, 250.f);
	Engine::GetRandomVector(&pEffect->vColor, &vMin, &vMax);

	MyMat matCompute = *m_pBone * *m_pTarget;
	MyVec3 vPos = *(MyVec3*)&matCompute.m[3][0];
	MyVec3 vUp = *(MyVec3*)&m_pTarget->m[1][0];

	_float fSize = 0.f;

	switch (m_iType)
	{
	case 0:
		pEffect->vColor = MyVec3(131.f, 149.f, 250.f);
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.25f, 0.3f);
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 6.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(1.f, 4.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 1.f;
		pEffect->fSizeWayTime = 0.3f;
		pEffect->fSizeWayDelta = 4.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.2f;
		pEffect->fAlphaWayDelta = 100.f;
		break;

	case 1:
		pEffect->vColor = MyVec3(131.f, 149.f, 250.f);
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.15f, 0.2f);
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 5.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(15.f, 20.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 0.f;
		pEffect->fSizeWayTime = 0.3f;
		pEffect->fSizeWayDelta = 4.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.2f;
		pEffect->fAlphaWayDelta = 40.f;
		break;

	case 2:
		pEffect->vColor = MyVec3(131.f, 149.f, 250.f);
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.15f, 0.2f);
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = 180.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(1.f, 3.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 1.5f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.01f;
		pEffect->fAlphaWayDelta = 100.f;
		break;

	case 3:
		pEffect->vColor = MyVec3(131.f, 149.f, 250.f);
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.25f, 0.3f);
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = 180.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(2.f, 4.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 5.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.01f;
		pEffect->fAlphaWayDelta = 70.f;
		break;

	case 4:
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.7f, 1.f);
		pEffect->fSpeed = Engine::GetRandomFloat(5.f, 15.f);
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 3.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(2.f, 3.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 5.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.5f;
		pEffect->fAlphaWayDelta = 100.f;

		pEffect->fRollDelta = 10.f;
		break;

	case 5:
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.3f, 0.5f);
		pEffect->fSpeed = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 3.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(3.f, 5.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 2.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.7f;
		pEffect->fAlphaWayDelta = 60.f;

		pEffect->fRollDelta = 10.f;
		break;

	case 6:
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.15f, 0.25f);
		pEffect->fSpeed = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 3.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(8.f, 9.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = -7.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.01f;
		pEffect->fAlphaWayDelta = 100.f;

		pEffect->fRollDelta = 10.f;
		break;

	case 7:
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.25f, 0.35f);
		pEffect->fSpeed = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 3.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(1.f, 1.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 0.2f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.7f;
		pEffect->fAlphaWayDelta = 100.f;

		pEffect->fRollDelta = 90.f;
		break;

	case 8:
		pEffect->vAngle = MyVec3(0.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.2f, 0.3f);
		pEffect->fSpeed = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 3.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(3.f, 4.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = -3.5f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.7f;
		pEffect->fAlphaWayDelta = 70.f;

		pEffect->fRollDelta = 10.f;
		break;

	case 9:
		pEffect->vAngle = MyVec3(90.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = 0.7f;
		pEffect->fSpeed = 0.f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(1.f, 5.f);
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(0.f, 0.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = m_vOrigin + vUp * 2.f;

		pEffect->fSizeDelta = -1.f;
		pEffect->fSizeWayTime = 0.7f;
		pEffect->fSizeWayDelta = 1.5f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.5f;
		pEffect->fAlphaWayDelta = -155.f;

		pEffect->fRollDelta = 390.f;
		break;

	case 10:
		pEffect->vAngle = MyVec3(90.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(131.f, 149.f, 250.f);
		pEffect->fMaxLife = 0.4f;
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = 0.f;
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(9.f, 9.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = m_vOrigin + MyVec3(0.f, 0.5f, 0.f);

		pEffect->fSizeDelta = 5.f;
		pEffect->fSizeWayTime = 0.3f;
		pEffect->fSizeWayDelta = 4.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.01f;
		pEffect->fAlphaWayDelta = 100.f;
		break;

	case 11:
		pEffect->vAngle = MyVec3(90.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(131.f, 149.f, 250.f);
		pEffect->fMaxLife = 9.f;
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = 0.f;
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(16.f, 16.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = m_vOrigin + MyVec3(0.f, 0.5f, 0.f);

		pEffect->fSizeDelta = 3.f;
		pEffect->fSizeWayTime = 0.15f;
		pEffect->fSizeWayDelta = 2.5f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.6f;
		pEffect->fAlphaWayDelta = 90.f;

		pEffect->fRollDelta = 40.f;
		break;

	case 12:
		pEffect->vAngle = MyVec3(90.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = 8.5f;
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = 0.f;
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(26.f, 26.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = m_vOrigin + MyVec3(0.f, 0.5f, 0.f);

		pEffect->fSizeDelta = 5.f;
		pEffect->fSizeWayTime = 0.1f;
		pEffect->fSizeWayDelta = 4.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.6f;
		pEffect->fAlphaWayDelta = 90.f;

		pEffect->fRollDelta = 40.f;
		break;

	case 13:
		pEffect->vAngle = MyVec3(90.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = 8.5f;
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = 0.f;
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(48.f, 48.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = m_vOrigin + MyVec3(0.f, 0.5f, 0.f);

		pEffect->fSizeDelta = 5.f;
		pEffect->fSizeWayTime = 0.1f;
		pEffect->fSizeWayDelta = 4.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.6f;
		pEffect->fAlphaWayDelta = 90.f;

		pEffect->fRollDelta = 40.f;
		break;

	case 14:
		pEffect->vAngle = MyVec3(90.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = 8.5f;
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = 0.f;
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(15.f, 15.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = m_vOrigin + MyVec3(0.f, 0.5f, 0.f);

		pEffect->fSizeDelta = 5.f;
		pEffect->fSizeWayTime = 0.1f;
		pEffect->fSizeWayDelta = 4.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.6f;
		pEffect->fAlphaWayDelta = 90.f;

		pEffect->fRollDelta = 40.f;
		break;

	case 15:
		pEffect->vAngle = MyVec3(90.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = 8.5f;
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = 0.f;
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(24.f, 24.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = m_vOrigin + MyVec3(0.f, 0.5f, 0.f);

		pEffect->fSizeDelta = 5.f;
		pEffect->fSizeWayTime = 0.1f;
		pEffect->fSizeWayDelta = 4.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.6f;
		pEffect->fAlphaWayDelta = 90.f;

		pEffect->fRollDelta = 40.f;
		break;

	case 16:
		pEffect->vAngle = MyVec3(90.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 255.f, 255.f);
		pEffect->fMaxLife = 0.4f;
		pEffect->fSpeed = Engine::GetRandomFloat(5.f, 15.f);
		pEffect->fAngle = 0.f;
		pEffect->fRadius = 0.f;
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(15.f, 15.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = m_vOrigin + MyVec3(0.f, 0.5f, 0.f);

		pEffect->fSizeDelta = 90.f;
		pEffect->fSizeWayTime = 0.4f;
		pEffect->fSizeWayDelta = 60.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.01f;
		pEffect->fAlphaWayDelta = 250.f;

		pEffect->fRollDelta = 180.f;
		break;

	case 17:
		pEffect->vAngle = MyVec3(90.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 255.f, 255.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.4f, 0.5f);
		pEffect->fSpeed = Engine::GetRandomFloat(5.f, 15.f);
		pEffect->fAngle = 15.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(5.f, 15.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = m_vOrigin + MyVec3(0.f, 0.5f, 0.f);

		pEffect->fSizeDelta = 250.f;
		pEffect->fSizeWayTime = 0.01f;
		pEffect->fSizeWayDelta = 0.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.01f;
		pEffect->fAlphaWayDelta = 250.f;

		pEffect->fRollDelta = 60.f;
		break;

	case 18:
		pEffect->vAngle = MyVec3(90.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 255.f, 255.f);
		pEffect->fMaxLife = 0.8f;
		pEffect->fSpeed = Engine::GetRandomFloat(5.f, 15.f);
		pEffect->fAngle = 0.f;
		pEffect->fRadius = 0.f;
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(15.f, 15.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = m_vOrigin + MyVec3(0.f, 0.5f, 0.f);

		pEffect->fSizeDelta = 200.f;
		pEffect->fSizeWayTime = 0.5f;
		pEffect->fSizeWayDelta = 140.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.01f;
		pEffect->fAlphaWayDelta = 110.f;

		pEffect->fRollDelta = 180.f;
		break;

	case 19:
		pEffect->vAngle = MyVec3(90.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 255.f, 255.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.2f, 0.3f);
		pEffect->fSpeed = Engine::GetRandomFloat(5.f, 15.f);
		pEffect->fAngle = 15.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(10.f, 30.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = m_vOrigin + MyVec3(0.f, 0.5f, 0.f);

		pEffect->fSizeDelta = 210.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.01f;
		pEffect->fAlphaWayDelta = 70.f;

		pEffect->fRollDelta = 180.f;
		break;

	case 20:
		pEffect->vAngle = MyVec3(90.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 255.f, 255.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.2f, 0.3f);
		pEffect->fSpeed = Engine::GetRandomFloat(5.f, 15.f);
		pEffect->fAngle = 0.f;
		pEffect->fRadius = 0.f;
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(5.f, 15.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = m_vOrigin + MyVec3(0.f, 0.5f, 0.f);

		pEffect->fSizeDelta = 220.f;
		pEffect->fSizeWayTime = 0.5f;
		pEffect->fSizeWayDelta = 130.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.01f;
		pEffect->fAlphaWayDelta = 110.f;

		pEffect->fRollDelta = 180.f;
		break;

	case 21:
		pEffect->vAngle = MyVec3(90.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 255.f, 255.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.2f, 0.3f);
		pEffect->fSpeed = Engine::GetRandomFloat(5.f, 15.f);
		pEffect->fAngle = 20.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(20.f, 40.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = m_vOrigin + MyVec3(0.f, 0.5f, 0.f);

		pEffect->fSizeDelta = 220.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.01f;
		pEffect->fAlphaWayDelta = 70.f;

		pEffect->fRollDelta = 40.f;
		break;

	case 22:
		pEffect->vAngle = MyVec3(90.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 255.f, 255.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.2f, 0.3f);
		pEffect->fSpeed = Engine::GetRandomFloat(5.f, 15.f);
		pEffect->fAngle = 20.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(20.f, 40.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = m_vOrigin + MyVec3(0.f, 0.5f, 0.f);

		pEffect->fSizeDelta = 210.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.01f;
		pEffect->fAlphaWayDelta = 70.f;

		pEffect->fRollDelta = 40.f;
		break;

	case 23:
		pEffect->vAngle = MyVec3(90.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 255.f, 255.f);
		pEffect->fMaxLife = 0.3f;
		pEffect->fSpeed = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(320.f, 340.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = m_vOrigin + MyVec3(0.f, 0.5f, 0.f);

		pEffect->fSizeDelta = 50.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.01f;
		pEffect->fAlphaWayDelta = 110.f;

		pEffect->fRollDelta = 180.f;
		break;

	case 30:
		pEffect->vAngle = MyVec3(90.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(131.f, 149.f, 250.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.25f, 0.3f);
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(1.f, 1.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos + vUp * 4.f;

		pEffect->fSizeDelta = -0.5f;
		pEffect->fSizeWayTime = 0.01f;
		pEffect->fSizeWayDelta = -0.1f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.8f;
		pEffect->fAlphaWayDelta = 100.f;

		pEffect->fRollDelta = 200.f;
		break;

	case 31:
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(131.f, 149.f, 250.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.25f, 0.3f);
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(1.f, 1.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos + vUp * 4.f;

		pEffect->fSizeDelta = -0.5f;
		pEffect->fSizeWayTime = 0.01f;
		pEffect->fSizeWayDelta = -0.1f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.8f;
		pEffect->fAlphaWayDelta = 100.f;

		pEffect->fRollDelta = 200.f;
		break;

	case 32:
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(131.f, 149.f, 250.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.25f, 0.3f);
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = Engine::GetRandomFloat(-360.f, 360.f);
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fRadiusAngle = Engine::GetRandomFloat(-360.f, 360.f);
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(1.f, 1.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos + vUp * 4.f;

		pEffect->fSizeDelta = -0.5f;
		pEffect->fSizeWayTime = 0.01f;
		pEffect->fSizeWayDelta = -0.2f;

		pEffect->fAlphaDelta = 100.f;
		pEffect->fAlphaWayTime = 0.8f;
		pEffect->fAlphaWayDelta = 100.f;

		pEffect->fRollDelta = 200.f;
		break;

	case 33:
		pEffect->vAngle = MyVec3(90.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(131.f, 149.f, 250.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.25f, 0.3f);
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = 0.f;
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(1.f, 1.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos + vUp * 4.f;

		pEffect->fSizeDelta = -0.3f;
		pEffect->fSizeWayTime = 0.01f;
		pEffect->fSizeWayDelta = -0.7f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.1f;
		pEffect->fAlphaWayDelta = 100.f;
		break;

	case 34:
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(80.f, 90.f), Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.2f, 0.3f);
		pEffect->fSpeed = 0.f;
		pEffect->fAngle = 390.f;
		pEffect->fRadius = 0.f;
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(0.f, 0.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos + vUp * 6.f;

		pEffect->fSizeDelta = 0.3f;
		pEffect->fSizeWayTime = 0.01f;
		pEffect->fSizeWayDelta = 0.6f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.9f;
		pEffect->fAlphaWayDelta = -5.f;

		pEffect->fRollDelta = 190.f;
		break;

	case 35:
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(80.f, 90.f), Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = 2.1f;
		pEffect->fSpeed = 0.f;
		pEffect->fAngle = 390.f;
		pEffect->fRadius = 0.f;
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(0.f, 0.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos + vUp * 6.f;

		pEffect->fSizeDelta = 0.f;
		pEffect->fSizeWayTime = 0.85f;
		pEffect->fSizeWayDelta = 0.9f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.9f;
		pEffect->fAlphaWayDelta = -155.f;

		pEffect->fRollDelta = 190.f;
		break;

	case 40:
		pEffect->vAngle = MyVec3(0.f, 0.f, 0.f);
		pEffect->vColor = MyVec3(255.f, 255.f, 255.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.5f, 0.7f);
		pEffect->fSpeed = Engine::GetRandomFloat(20.f, 30.f);
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(10.f, 20.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(15.f, 27.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 77.f;
		pEffect->fSizeWayTime = 0.6f;
		pEffect->fSizeWayDelta = 57.f;

		pEffect->fAlphaDelta = 0.f;
		break;

	case 41:
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(0.f, 10.f), Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.3f, 0.4f);
		pEffect->fSpeed = Engine::GetRandomFloat(10.f, 50.f);
		pEffect->fAngle = 120.f;
		pEffect->fRadius = Engine::GetRandomFloat(30.f, 50.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(3.f, 5.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 10.f;
		pEffect->fSizeWayTime = 0.5f;
		pEffect->fSizeWayDelta = 35.f;

		pEffect->fAlphaDelta = 100.f;
		break;

	case 42:
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(0.f, 10.f), Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.6f, 0.7f);
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = 180.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(12.f, 17.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 55.f;
		pEffect->fSizeWayTime = 0.5f;
		pEffect->fSizeWayDelta = 33.;

		pEffect->fAlphaDelta = 0.f;
		break;

	case 43:
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.6f, 0.9f);
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = 180.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(12.f, 17.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 23.f;
		pEffect->fSizeWayTime = 0.5f;
		pEffect->fSizeWayDelta = 25.f;

		pEffect->fAlphaDelta = 0.f;

		pEffect->fRollDelta = 15.f;
		break;

	case 44:
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(2.2f, 3.f);
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = Engine::GetRandomFloat(-360.f, 360.f);
		pEffect->fRadius = Engine::GetRandomFloat(15.f, 150.f);
		pEffect->fRadiusAngle = Engine::GetRandomFloat(-360.f, 360.f);
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = 0.f;
		fSize = Engine::GetRandomFloat(12.f, 17.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos + pEffect->vDir;

		pEffect->fSizeDelta = 23.f;
		pEffect->fSizeWayTime = 0.5f;
		pEffect->fSizeWayDelta = 25.f;

		pEffect->fAlphaDelta = 0.f;

		pEffect->fRollDelta = 15.f;
		break;
	}

	D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
	pEffect->fAlphaDelta -= 255.f;
	m_EffectList.push_back(pEffect);
}

HRESULT CLordOfMana::Ready_Component( void )
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

void CLordOfMana::Free( void )
{
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pTextureCom);
	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pShaderCom);

	Engine::CGameObject::Free();
}

CLordOfMana* CLordOfMana::Create( LPDIRECT3DDEVICE9 pDevice, const _uint& iType)
{
	CLordOfMana* pLordOfMana = new CLordOfMana(pDevice);

	if (FAILED(pLordOfMana->Initialize(iType)))
	{
		MSG_BOX("LordOfMana Created Failed");
		Engine::Safe_Release(pLordOfMana);
	}
	return pLordOfMana;
}



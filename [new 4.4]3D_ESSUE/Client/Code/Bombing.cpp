#include "StdAfx.h"
#include "Bombing.h"
#include "DefaultCamera.h"
#include "BossSword.h"
#include "Player.h"

CBombing::CBombing( LPDIRECT3DDEVICE9 pDevice)
: CEffect(pDevice)
, m_fRadius(0.f)
, m_fRadiusDelta(0.f)
, m_iCountDelta(0)
, m_pPlayer(NULL)
, m_bColl(true)
{
	
}

CBombing::~CBombing( void )
{

}

HRESULT CBombing::Initialize( const _uint& iType)
{
	m_iType = iType;
	if (m_iType >= 20)
		m_pBone = ((Engine::CDynamicMesh*)Engine::Get_Management()->GetLayer(L"Layer_Boss")->GetObject(0)->GetComponent(L"Com_Mesh"))->GetPartsMatrix("ValveBiped_Anim_Attachment_RH");
	else if (m_iType >= 10)
		m_pBone = ((Engine::CDynamicMesh*)Engine::Get_Management()->GetLayer(L"Layer_Boss")->GetObject(0)->GetComponent(L"Com_Mesh"))->GetPartsMatrix("ValveBiped_Bip01_R_Toe0");
	else if (m_iType == 6)
		m_pBone = ((Engine::CDynamicMesh*)Engine::Get_Management()->GetLayer(L"Layer_Boss")->GetObject(0)->GetComponent(L"Com_Mesh"))->GetPartsMatrix("ValveBiped_Bip01_L_Finger22");
	else
		m_pBone = ((Engine::CDynamicMesh*)Engine::Get_Management()->GetLayer(L"Layer_Boss")->GetObject(0)->GetComponent(L"Com_Mesh"))->GetPartsMatrix("ValveBiped_Bip01_L_Hand");

	m_pTarget = ((Engine::CTransform*)Engine::Get_Management()->GetLayer(L"Layer_Boss")->GetObject(0)->GetComponent(L"Com_Transform"))->GetWorldMatrix();

	m_vOrigin = *(MyVec3*)&(*m_pBone * *m_pTarget).m[3][0];
	m_vAngle = *((Engine::CTransform*)Engine::Get_Management()->GetLayer(L"Layer_Boss")->GetObject(0)->GetComponent(L"Com_Transform"))->GetInfomation(Engine::CTransform::INFO_ANGLE);
	m_vAngle.y = D3DXToDegree(m_vAngle.y);

	if (m_iType == 36)
	{
		m_bColl = false;
		m_pPlayer = ((Engine::CTransform*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(0)->GetComponent(L"Com_Transform"))->GetWorldMatrix();
	}

	DefaultSetting();

	if (FAILED(Ready_Component()))
		return E_FAIL;

	for (_uint i = 0; i < m_iCount; ++i)
		AddParticle();

	return S_OK;
}

_int CBombing::Update( const _float& fDelta )
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
			if (m_bColl == false)
			{
				if (m_iType == 36)
				{
					MyVec3 vDif = *(MyVec3*)&m_pPlayer->m[3][0] - (*iter)->vPos;
					_float fDis = fabs(D3DXVec3Length(&vDif));
					_float fRadius = ((*iter)->vScale.x + (*iter)->vScale.y);

					if (fDis <= fRadius)
					{
						((CPlayer*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(0))->SetDamage(200.f);
						m_bColl = true;
					}
				}
			}
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

void CBombing::Render( void )
{
	LPD3DXEFFECT pEffect = m_pShaderCom->GetEffect();
	if (pEffect == NULL)
		return;

	SetUpShader(pEffect);
}

void CBombing::SetUpShader( LPD3DXEFFECT pEffect )
{
	pEffect->AddRef();

	MyMat matView, matProj;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	m_pTextureCom->SetUpShader(pEffect, m_iIndex);
	
	pEffect->Begin(NULL, 0);
	if (m_iType >= 30)
		pEffect->BeginPass(2);
	else
		pEffect->BeginPass(1);

	EFFECTLIST::iterator iter = m_EffectList.begin();
	EFFECTLIST::iterator iter_end = m_EffectList.end();
	MyMat matScale, matRotX, matRotY, matRotZ, matTrans, matWorld;

	//MyMat matBill;
	//D3DXMatrixIdentity(&matBill);
	//matBill._11 = matView._11;
	//matBill._13 = matView._13;
	//matBill._31 = matView._31;
	//matBill._33 = matView._33;
	//D3DXMatrixInverse(&matBill, NULL, &matBill);

	for ( ; iter != iter_end; ++iter)
	{
		if ((*iter)->bAlive == true)
		{
			_float	fSizeDelta = GetSizeDelta(*iter);
			_float	fRollDelta = GetRollDelta(*iter);
			_uint	iAlphaDelta = GetAlphaDelta(*iter);
			MyVec3	ScaleDelta = GetScaleDelta(*iter);
			D3DXMatrixScaling(&matScale, (*iter)->vScale.x + fSizeDelta + ScaleDelta.x, (*iter)->vScale.y + fSizeDelta + ScaleDelta.y, (*iter)->vScale.z + ScaleDelta.z);
			D3DXMatrixRotationX(&matRotX, D3DXToRadian((*iter)->vAngle.x));
			D3DXMatrixRotationY(&matRotY, D3DXToRadian((*iter)->vAngle.y));
			D3DXMatrixRotationZ(&matRotZ, D3DXToRadian((*iter)->vAngle.z + fRollDelta));
			D3DXMatrixTranslation(&matTrans, (*iter)->vPos.x, (*iter)->vPos.y, (*iter)->vPos.z);
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

void CBombing::DefaultSetting( void )
{
	switch (m_iType)
	{
	// bombinghand 01 Ä® ÆøÆÄÀü ¸¶¹ýÁø
	case 0:
		m_pEffectKey = L"Component_Texture_ShapeFire";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.02f;
		m_fDuration = 0.15f;
		break;

	case 1:
		m_pEffectKey = L"Component_Texture_ShapeFire";
		m_iCount = 1;
		m_iIndex = 1;

		m_fInterval = 0.02f;
		m_fDuration = 0.15f;
		break;

	case 2:
		m_pEffectKey = L"Component_Texture_LightGlow";
		m_iCount = 2;
		m_iIndex = 3;

		m_fInterval = 0.02f;
		m_fDuration = 0.15f;
		break;

	case 3:
		m_pEffectKey = L"Component_Texture_Ring_Outer_Wind";
		m_iCount = 2;
		m_iIndex = 6;

		m_fInterval = 0.02f;
		m_fDuration = 0.15f;
		break;

	case 4:
		m_pEffectKey = L"Component_Texture_Ring_Outer_Wind";
		m_iCount = 3;
		m_iIndex = 3;

		m_fInterval = 0.02f;
		m_fDuration = 0.15f;
		break;

	case 5:
		m_pEffectKey = L"Component_Texture_Ring";
		m_iCount = 1;
		m_iIndex = 5;

		m_fInterval = 0.02f;
		m_fDuration = 0.15f;
		break;

	case 6: // ¼Õ°¡¶ô
		m_pEffectKey = L"Component_Texture_YellowFlare";
		m_iCount = 1;
		m_iIndex = 6;

		m_fInterval = 0.001f;
		m_fDuration = 2.7f;
		break;

	// ¹Ù´Ú
	case 10:
		m_pEffectKey = L"Component_Texture_IceLight";
		m_iCount = 8;
		m_iIndex = 1;

		m_fInterval = 0.01f;
		m_fDuration = 0.15f;
		break;

	case 11:
		m_pEffectKey = L"Component_Texture_ShapeFire";
		m_iCount = 4;
		m_iIndex = 0;

		m_fInterval = 0.01f;
		m_fDuration = 0.15f;
		break;

	case 12:
		m_pEffectKey = L"Component_Texture_Ring_Outer_Wind";
		m_iCount = 2;
		m_iIndex = 6;

		m_fInterval = 0.02f;
		m_fDuration = 0.25f;
		break;

	case 13:
		m_pEffectKey = L"Component_Texture_LightGlow";
		m_iCount = 1;
		m_iIndex = 3;

		m_fInterval = 0.02f;
		m_fDuration = 0.25f;
		break;

	// bomgbing wp01 Ä® Áö¸é²Å±â
	case 20:
		m_pEffectKey = L"Component_Texture_Ring_Outer_Wind";
		m_iCount = 1;
		m_iIndex = 6;

		m_fInterval = 0.02f;
		m_fDuration = 0.15f;
		break;

	case 21:
		m_pEffectKey = L"Component_Texture_ShapeFire";
		m_iCount = 2;
		m_iIndex = 0;

		m_fInterval = 0.02f;
		m_fDuration = 0.15f;
		break;

	case 22:
		m_pEffectKey = L"Component_Texture_Blunt";
		m_iCount = 4;
		m_iIndex = 0;

		m_fInterval = 0.01f;
		m_fDuration = 0.15f;
		break;

	case 23:
		m_pEffectKey = L"Component_Texture_CriticalEhen";
		m_iCount = 1;
		m_iIndex = 2;

		m_fInterval = 0.05f;
		m_fDuration = 0.3f;
		break;

	case 24:
		m_pEffectKey = L"Component_Texture_LightGlow";
		m_iCount = 1;
		m_iIndex = 3;

		m_fInterval = 0.04f;
		m_fDuration = 1.f;
		break;

	// bombing wp02 µüÄá ÆøÆÄ
	case 30:
		m_pEffectKey = L"Component_Texture_IceLight";
		m_iCount = 3;
		m_iIndex = 1;

		m_fInterval = 0.015f;
		m_fDuration = 0.15f;
		break;

	case 31:
		m_pEffectKey = L"Component_Texture_ShapeFire";
		m_iCount = 2;
		m_iIndex = 0;

		m_fInterval = 0.01f;
		m_fDuration = 0.5f;
		break;

	case 32:
		m_pEffectKey = L"Component_Texture_LightGlow";
		m_iCount = 3;
		m_iIndex = 3;

		m_fInterval = 0.01f;
		m_fDuration = 0.3f;
		break;

	case 33:
		m_pEffectKey = L"Component_Texture_Light";
		m_iCount = 2;
		m_iIndex = 0;

		m_fInterval = 0.01f;
		m_fDuration = 0.3f;
		break;

	case 34:
		m_pEffectKey = L"Component_Texture_LightRay";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.15f;
		m_fDuration = 0.8f;
		break;

	case 35:
		m_pEffectKey = L"Component_Texture_ShipGray";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.02f;
		m_fDuration = 0.5f;
		break;

	case 36:
		m_pEffectKey = L"Component_Texture_ShipGray";
		m_iCount = 1;
		m_iIndex = 1;

		m_fInterval = 0.02f;
		m_fDuration = 0.5f;
		break;
	}
}

void CBombing::AddParticle( void )
{
	EFFECT* pEffect= new EFFECT;
	ZeroMemory(pEffect, sizeof(EFFECT));

	pEffect->vScale = MyVec3(1.f, 1.f, 1.f);
	pEffect->bAlive = true;
	D3DXVECTOR3 vMin(255.f, 155.f, 115.f);
	D3DXVECTOR3 vMax(255.f, 185.f, 155.f);
	Engine::GetRandomVector(&pEffect->vColor, &vMin, &vMax);

	MyMat matCompute = *m_pBone * *m_pTarget;
	MyVec3 vPos = *(MyVec3*)&matCompute.m[3][0];
	MyVec3 vLook = *D3DXVec3Normalize(&vLook, (MyVec3*)&m_pTarget->m[2][0]);

	_float fSize = 0.f;

	switch (m_iType)
	{
	case 0:
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(-10.f, 10.f), 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.6f, 0.8f);
		pEffect->fSpeed = Engine::GetRandomFloat(0.f, 5.f);
		pEffect->fAngle = 180.f;
		pEffect->fRadius = Engine::GetRandomFloat(10.f, 25.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(2.f, 3.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = m_vOrigin + vLook * 3.f;

		pEffect->fSizeDelta = 4.f;
		pEffect->fSizeWayTime = 0.2f;
		pEffect->fSizeWayDelta = 3.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.3f;
		pEffect->fAlphaWayDelta = 250.f;

		pEffect->fRollDelta = 10.f;
		break;

	case 1:
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(-10.f, 10.f), 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.6f, 0.8f);
		pEffect->fSpeed = Engine::GetRandomFloat(0.f, 5.f);
		pEffect->fAngle = 180.f;
		pEffect->fRadius = Engine::GetRandomFloat(10.f, 25.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(3.f, 4.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = m_vOrigin + vLook * 3.f;

		pEffect->fSizeDelta = 4.f;
		pEffect->fSizeWayTime = 0.2f;
		pEffect->fSizeWayDelta = 3.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.3f;
		pEffect->fAlphaWayDelta = 250.f;

		pEffect->fRollDelta = 10.f;
		break;

	case 2:
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(-10.f, 10.f), 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.25f, 0.3f);
		pEffect->fSpeed = Engine::GetRandomFloat(1.f, 5.f);
		pEffect->fAngle = 15.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(15.f, 15.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = m_vOrigin + vLook * 3.f;

		pEffect->fSizeDelta = 16.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.3f;
		pEffect->fAlphaWayDelta = 250.f;

		pEffect->fRollDelta = 35.f;
		break;

	case 3:
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(-10.f, 10.f), 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.4f, 0.6f);
		pEffect->fSpeed = Engine::GetRandomFloat(1.f, 5.f);
		pEffect->fAngle = 15.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(5.f, 5.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = m_vOrigin + vLook * 3.f;

		pEffect->fSizeDelta = 6.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.3f;
		pEffect->fAlphaWayDelta = 250.f;

		pEffect->fRollDelta = 35.f;
		break;

	case 4:
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(-10.f, 10.f), 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = 0.5f;
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(6.f, 6.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = m_vOrigin + vLook * 3.f;

		pEffect->fSizeDelta = 6.f;
		pEffect->fSizeWayTime = 0.4f;
		pEffect->fSizeWayDelta = 5.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.3f;
		pEffect->fAlphaWayDelta = 250.f;

		pEffect->fRollDelta = 35.f;
		break;

	case 5:
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(-10.f, 10.f), 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = 0.5f;
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(4.f, 4.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = m_vOrigin + vLook * 3.f;

		pEffect->fSizeDelta = 6.f;
		pEffect->fSizeWayTime = 0.4f;
		pEffect->fSizeWayDelta = 5.f;

		pEffect->fAlphaDelta = 0.f;

		pEffect->fRollDelta = 35.f;
		break;

	case 6:
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(-10.f, 10.f), Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.5f, 0.7f);
		pEffect->fSpeed = Engine::GetRandomFloat(2.f, 5.f);
		pEffect->fAngle = 180.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(3.f, 5.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = -0.5f;

		pEffect->fAlphaDelta = 0.f;

		pEffect->fRollDelta = 15.f;
		break;

	case 10:
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.4f, 0.6f);
		pEffect->fSpeed = Engine::GetRandomFloat(10.f, 25.f);
		pEffect->fAngle = Engine::GetRandomFloat(80.f, 90.f);
		pEffect->fRadius = Engine::GetRandomFloat(10.f, 15.f);
		pEffect->fRadiusAngle = Engine::GetRandomFloat(-360.f, 360.f);
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(19.f, 23.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos + pEffect->vDir;

		pEffect->fSizeDelta = 8.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.3f;
		pEffect->fAlphaWayDelta = 250.f;

		pEffect->fRollDelta = 10.f;
		break;

	case 11:
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.6f, 0.8f);
		pEffect->fSpeed = Engine::GetRandomFloat(1.f, 5.f);
		pEffect->fAngle = Engine::GetRandomFloat(80.f, 90.f);
		pEffect->fRadius = Engine::GetRandomFloat(9.f, 15.f);
		pEffect->fRadiusAngle = Engine::GetRandomFloat(-360.f, 360.f);
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(19.f, 23.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos + pEffect->vDir;

		pEffect->fSizeDelta = 8.f;
		pEffect->fSizeWayTime = 0.2f;
		pEffect->fSizeWayDelta = 6.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.3f;
		pEffect->fAlphaWayDelta = 250.f;

		pEffect->fRollDelta = 10.f;
		break;

	case 12:
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(80.f, 90.f), 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.3f, 0.5f);
		pEffect->fSpeed = Engine::GetRandomFloat(1.f, 5.f);
		pEffect->fAngle = 20.f;
		pEffect->fRadius = Engine::GetRandomFloat(10.f, 25.f);
		pEffect->fRadiusAngle = 120.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(19.f, 23.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 21.f;
		pEffect->fSizeWayTime = 0.3f;
		pEffect->fSizeWayDelta = 11.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.3f;
		pEffect->fAlphaWayDelta = 250.f;

		pEffect->fRollDelta = 30.f;
		break;

	case 13:
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(80.f, 90.f), 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.6f, 0.8f);
		pEffect->fSpeed = Engine::GetRandomFloat(1.f, 5.f);
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(80.f, 210.f);
		pEffect->fRadiusAngle = 120.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(19.f, 23.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 35.f;
		pEffect->fSizeWayTime = 0.3f;
		pEffect->fSizeWayDelta = 50.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.3f;
		pEffect->fAlphaWayDelta = 250.f;

		pEffect->fRollDelta = 30.f;
		break;

	case 20:
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(80.f, 100.f), 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.4f, 0.6f);
		pEffect->fSpeed = Engine::GetRandomFloat(1.f, 5.f);
		pEffect->fAngle = 15.f;
		pEffect->fRadius = Engine::GetRandomFloat(10.f, 15.f);
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle);
		pEffect->vDir.x = pEffect->fRadius * cosf(pEffect->fAngle);
		pEffect->vDir.y = 0.f;
		fSize = Engine::GetRandomFloat(15.f, 23.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos + MyVec3(0.f, -10.f, 0.f);

		pEffect->fSizeDelta = 16.f;
		pEffect->fSizeWayTime = 0.4f;
		pEffect->fSizeWayDelta = 12.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.3f;
		pEffect->fAlphaWayDelta = 250.f;

		pEffect->fRollDelta = 35.f;
		break;

	case 21:
		pEffect->vAngle = MyVec3(90.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.6f, 0.8f);
		pEffect->fSpeed = Engine::GetRandomFloat(1.f, 8.f);
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(20.f, 55.f);
		pEffect->fRadiusAngle = 120.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(19.f, 23.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos + MyVec3(0.f, -10.f, 0.f);

		pEffect->fSizeDelta = 8.f;
		pEffect->fSizeWayTime = 0.2f;
		pEffect->fSizeWayDelta = 6.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.3f;
		pEffect->fAlphaWayDelta = 250.f;

		pEffect->fRollDelta = 10.f;
		break;

	case 22:
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(0.f, 70.f), Engine::GetRandomFloat(-360.f, 360.f), 0.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.2f, 0.3f);
		pEffect->fSpeed = Engine::GetRandomFloat(4.f, 6.f);
		pEffect->fAngle = 25.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 15.f);
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle);
		pEffect->vDir.x = pEffect->fRadius * cosf(pEffect->fAngle);
		pEffect->vDir.y = 0.f;
		pEffect->vScale = MyVec3(Engine::GetRandomFloat(15.f, 25.f), Engine::GetRandomFloat(65.f, 88.f), 1.f);
		pEffect->vPos = vPos + MyVec3(0.f, -10.f, 0.f);

		pEffect->fSizeDelta = -35.f;
		pEffect->fSizeWayTime = 0.3f;
		pEffect->fSizeWayDelta = 15.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.5f;
		pEffect->fAlphaWayDelta = 250.f;
		break;

	case 23:
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(0.f, 70.f), Engine::GetRandomFloat(-360.f, 360.f), 0.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.3f, 0.5f);
		pEffect->fSpeed = Engine::GetRandomFloat(3.f, 4.f);
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 5.f);
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle);
		pEffect->vDir.x = pEffect->fRadius * cosf(pEffect->fAngle);
		pEffect->vDir.y = 0.f;
		pEffect->vScale = MyVec3(Engine::GetRandomFloat(15.f, 25.f), Engine::GetRandomFloat(25.f, 48.f), 1.f);
		pEffect->vPos = vPos + MyVec3(0.f, -10.f, 0.f);

		pEffect->fSizeDelta = 15.f;
		pEffect->fSizeWayTime = 0.3f;
		pEffect->fSizeWayDelta = 50.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.3f;
		pEffect->fAlphaWayDelta = 190.f;
		break;

	case 24:
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(-10.f, 10.f), Engine::GetRandomFloat(-360.f, 360.f), 0.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.6f, 0.9f);
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 100.f);
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle);
		pEffect->vDir.x = pEffect->fRadius * cosf(pEffect->fAngle);
		pEffect->vDir.y = 0.f;
		fSize = 5 + m_fDurationAcc / m_fDuration * Engine::GetRandomFloat(20.f, 30.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos + MyVec3(0.f, -10.f, 0.f);

		pEffect->fSizeDelta = -3.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.2f;
		pEffect->fAlphaWayDelta = 250.f;

		pEffect->fRollDelta = 10.f;
		break;

	case 30:
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(-10.f, 10.f), Engine::GetRandomFloat(-360.f, 360.f), 0.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.4f, 0.6f);
		pEffect->fSpeed = Engine::GetRandomFloat(30.f, 55.f);
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(150.f, 2500.f);
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle);
		pEffect->vDir.x = pEffect->fRadius * cosf(pEffect->fAngle);
		pEffect->vDir.y = Engine::GetRandomFloat(0.f, 300.f);
		fSize = Engine::GetRandomFloat(49.f, 53.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 38.f;
		pEffect->fSizeWayTime = 0.01f;
		pEffect->fSizeWayDelta = 20.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.3f;
		pEffect->fAlphaWayDelta = 250.f;

		pEffect->fRollDelta = 10.f;
		break;

	case 31:
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(-10.f, 10.f), Engine::GetRandomFloat(-360.f, 360.f), 0.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.6f, 0.8f);
		pEffect->fSpeed = Engine::GetRandomFloat(10.f, 25.f);
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(300.f, 3000.f);
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle);
		pEffect->vDir.x = pEffect->fRadius * cosf(pEffect->fAngle);
		pEffect->vDir.y = Engine::GetRandomFloat(0.f, 300.f);
		fSize = Engine::GetRandomFloat(30.f, 40.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 32.f;
		pEffect->fSizeWayTime = 0.2f;
		pEffect->fSizeWayDelta = 26.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.3f;
		pEffect->fAlphaWayDelta = 250.f;

		pEffect->fRollDelta = 10.f;
		break;

	case 32:
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(-10.f, 10.f), Engine::GetRandomFloat(-360.f, 360.f), 0.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(1.f, 1.5f);
		pEffect->fSpeed = Engine::GetRandomFloat(50.f, 80.f);
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(400.f, 4500.f);
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle);
		pEffect->vDir.x = pEffect->fRadius * cosf(pEffect->fAngle);
		pEffect->vDir.y = Engine::GetRandomFloat(0.f, 800.f);
		fSize = Engine::GetRandomFloat(33.f, 37.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 150.f;
		pEffect->fSizeWayTime = 0.01f;
		pEffect->fSizeWayDelta = 100.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.3f;
		pEffect->fAlphaWayDelta = 250.f;

		pEffect->fRollDelta = 10.f;
		break;

	case 33:
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(-10.f, 10.f), Engine::GetRandomFloat(-360.f, 360.f), 0.f);
		pEffect->fMaxLife = 2.f;
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = 1.f;
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle);
		pEffect->vDir.x = pEffect->fRadius * cosf(pEffect->fAngle);
		pEffect->vDir.y = Engine::GetRandomFloat(0.f, 1.f);
		fSize = Engine::GetRandomFloat(50.f, 50.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 300.f;
		pEffect->fSizeWayTime = 0.01f;
		pEffect->fSizeWayDelta = 280.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.3f;
		pEffect->fAlphaWayDelta = 250.f;

		pEffect->fRollDelta = 10.f;
		break;

	case 34:
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(-10.f, 10.f), Engine::GetRandomFloat(-360.f, 360.f), 0.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(1.f, 1.5f);
		pEffect->fSpeed = 5.f;
		pEffect->fAngle = 360.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle);
		pEffect->vDir.x = pEffect->fRadius * cosf(pEffect->fAngle);
		pEffect->vDir.y = Engine::GetRandomFloat(0.f, 10.f);
		fSize = Engine::GetRandomFloat(30.f, 40.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 70.f;
		pEffect->fSizeWayTime = 0.01f;
		pEffect->fSizeWayDelta = 40.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.1f;
		pEffect->fAlphaWayDelta = 230.f;

		pEffect->fRollDelta = 10.f;
		break;

	case 35:
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(-10.f, 10.f), Engine::GetRandomFloat(-360.f, 360.f), 0.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.6f, 0.8f);
		pEffect->fSpeed = 5.f;
		pEffect->fAngle = 90.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle);
		pEffect->vDir.x = pEffect->fRadius * cosf(pEffect->fAngle);
		pEffect->vDir.y = Engine::GetRandomFloat(0.f, 10.f);
		fSize = Engine::GetRandomFloat(33.f, 37.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 300.f;
		pEffect->fSizeWayTime = 0.01f;
		pEffect->fSizeWayDelta = 20.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.1f;
		pEffect->fAlphaWayDelta = 230.f;

		pEffect->fRollDelta = 10.f;
		break;

	case 36:
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(-10.f, 10.f), Engine::GetRandomFloat(-360.f, 360.f), 0.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.4f, 0.6f);
		pEffect->fSpeed = 5.f;
		pEffect->fAngle = 30.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle);
		pEffect->vDir.x = pEffect->fRadius * cosf(pEffect->fAngle);
		pEffect->vDir.y = Engine::GetRandomFloat(0.f, 10.f);
		fSize = Engine::GetRandomFloat(40.f, 60.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 450.f;
		pEffect->fSizeWayTime = 0.01f;
		pEffect->fSizeWayDelta = 20.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.1f;
		pEffect->fAlphaWayDelta = 230.f;

		pEffect->fRollDelta = 10.f;
		break;
	}

	D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
	pEffect->vAngle += m_vAngle;
	pEffect->fAlphaDelta -= 255.f;
	m_EffectList.push_back(pEffect);
}

HRESULT CBombing::Ready_Component( void )
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

void CBombing::Free( void )
{
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pTextureCom);
	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pShaderCom);

	Engine::CGameObject::Free();
}

CBombing* CBombing::Create( LPDIRECT3DDEVICE9 pDevice, const _uint& iType)
{
	CBombing* pBombing = new CBombing(pDevice);

	if (FAILED(pBombing->Initialize(iType)))
	{
		MSG_BOX("Bombing Created Failed");
		Engine::Safe_Release(pBombing);
	}
	return pBombing;
}



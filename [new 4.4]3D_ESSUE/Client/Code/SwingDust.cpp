#include "StdAfx.h"
#include "SwingDust.h"
#include "DefaultCamera.h"
#include "BossSword.h"

CSwingDust::CSwingDust( LPDIRECT3DDEVICE9 pDevice)
: CEffect(pDevice)
{
	
}

CSwingDust::~CSwingDust( void )
{

}

HRESULT CSwingDust::Initialize( const _uint& iType)
{
	m_iType = iType;
	m_pBone = ((Engine::CDynamicMesh*)Engine::Get_Management()->GetLayer(L"Layer_Boss")->GetObject(2)->GetComponent(L"Com_Mesh"))->GetPartsMatrix("ValveBiped_Anim_Attachment_RH");
	m_pTarget = ((CBossSword*)Engine::Get_Management()->GetLayer(L"Layer_Boss")->GetObject(2))->GetComputeMatrix();
	m_vOrigin = *(MyVec3*)&(*m_pBone * *m_pTarget).m[3][0];


	DefaultSetting();

	if (FAILED(Ready_Component()))
		return E_FAIL;

	for (_uint i = 0; i < m_iCount; ++i)
		AddParticle();

	return S_OK;
}

_int CSwingDust::Update( const _float& fDelta )
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
			(*iter)->vPos.y -= (*iter)->fGravity;

			if ((*iter)->fLifeTime >= (*iter)->fMaxLife)
				(*iter)->bAlive = false;
		}
	}

	Engine::CGameObject::Update(fDelta);
	m_pRendererCom->AddRenderList(Engine::CRenderer::RENDER_ALPHA, this);

	return 0;
}

void CSwingDust::Render( void )
{
	LPD3DXEFFECT pEffect = m_pShaderCom->GetEffect();
	if (pEffect == NULL)
		return;

	SetUpShader(pEffect);
}

void CSwingDust::SetUpShader( LPD3DXEFFECT pEffect )
{
	pEffect->AddRef();

	MyMat matView, matProj;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	m_pTextureCom->SetUpShader(pEffect, m_iIndex);
	
	pEffect->Begin(NULL, 0);
	pEffect->BeginPass(3);

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
			MyVec3	ScaleDelta = GetScaleDelta(*iter);
			D3DXMatrixScaling(&matScale, (*iter)->vScale.x + fSizeDelta + ScaleDelta.x, (*iter)->vScale.y + fSizeDelta + ScaleDelta.y, (*iter)->vScale.z + ScaleDelta.z);
			D3DXMatrixRotationX(&matRotX, D3DXToRadian((*iter)->vAngle.x));
			D3DXMatrixRotationY(&matRotY, D3DXToRadian((*iter)->vAngle.y + fRollDelta));
			D3DXMatrixRotationZ(&matRotZ, D3DXToRadian((*iter)->vAngle.z));
			D3DXMatrixTranslation(&matTrans, (*iter)->vPos.x, (*iter)->vPos.y, (*iter)->vPos.z);
			matWorld = matScale * matRotX * matRotY * matRotZ *  matTrans;
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

void CSwingDust::DefaultSetting( void )
{
	switch (m_iType)
	{
	case 0: // 가루1
		m_pEffectKey = L"Component_Texture_YellowFlare";
		m_iCount = 4;
		m_iIndex = 0;

		m_fInterval = 0.005f;
		m_fDuration = 0.3f;
		break;

	case 1: // 가루2
		m_pEffectKey = L"Component_Texture_Stone01b";
		m_iCount = 3;
		m_iIndex = 0;

		m_fInterval = 0.005f;
		m_fDuration = 0.3f;
		break;

	case 2: // 가루3
		m_pEffectKey = L"Component_Texture_Stone";
		m_iCount = 3;
		m_iIndex = 0;

		m_fInterval = 0.001f;
		m_fDuration = 0.3f;
		break;

	case 3: // 느린먼지1
		m_pEffectKey = L"Component_Texture_DustWhite";
		m_iCount = 3;
		m_iIndex = 1;

		m_fInterval = 0.001f;
		m_fDuration = 0.3f;
		break;

	case 4: // 느린먼지2
		m_pEffectKey = L"Component_Texture_Dust";
		m_iCount = 4;
		m_iIndex = 0;

		m_fInterval = 0.001f;
		m_fDuration = 0.4f;
		break;

	case 5: // 뾰족먼지
		m_pEffectKey = L"Component_Texture_Dust";
		m_iCount = 4;
		m_iIndex = 0;

		m_fInterval = 0.001f;
		m_fDuration = 0.4f;
		break;

	case 6: // 뾰족먼지1
		m_pEffectKey = L"Component_Texture_Dust";
		m_iCount = 5;
		m_iIndex = 2;

		m_fInterval = 0.001f;
		m_fDuration = 0.4f;
		break;

	case 7: // 뾰족먼지1
		m_pEffectKey = L"Component_Texture_SimpleAura";
		m_iCount = 2;
		m_iIndex = 2;

		m_fInterval = 0.001f;
		m_fDuration = 0.3f;
		break;

	// Dust Back
	case 10: // 퍼지는먼지1
		m_pEffectKey = L"Component_Texture_DustWhite";
		m_iCount = 3;
		m_iIndex = 1;

		m_fInterval = 0.04f;
		m_fDuration = 0.3f;
		break;

	case 11: // 돌가루1
		m_pEffectKey = L"Component_Texture_Ice";
		m_iCount = 1;
		m_iIndex = 2;

		m_fInterval = 0.06f;
		m_fDuration = 0.15f;
		break;

	case 12: // 퍼지는먼지1
		m_pEffectKey = L"Component_Texture_DustWhite";
		m_iCount = 3;
		m_iIndex = 3;

		m_fInterval = 0.03f;
		m_fDuration = 0.3f;
		break;
	}
}

void CSwingDust::AddParticle( void )
{
	EFFECT* pEffect= new EFFECT;
	ZeroMemory(pEffect, sizeof(EFFECT));

	pEffect->vScale = MyVec3(1.f, 1.f, 1.f);
	pEffect->bAlive = true;
	D3DXVECTOR3 vMin(131.f, 149.f, 250.f);
	D3DXVECTOR3 vMax(181.f, 150.f, 250.f);
	Engine::GetRandomVector(&pEffect->vColor, &vMin, &vMax);

	MyMat matCompute = *m_pBone * *m_pTarget;
	MyVec3 vPos = *(MyVec3*)&matCompute.m[3][0];
	MyVec3 vRight = *D3DXVec3Normalize(&vRight, (MyVec3*)&m_pTarget->m[0][0]);
	MyVec3 vLook = *D3DXVec3Normalize(&vLook, (MyVec3*)&m_pTarget->m[2][0]);

	_float fSize = 0.f;

	switch (m_iType)
	{
	case 0:
		pEffect->vColor = MyVec3(39.f, 35.f, 27.f);
		pEffect->vAngle = MyVec3(-30.f, Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.3f, 0.5f);
		pEffect->fSpeed = Engine::GetRandomFloat(1.f, 2.f);
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(110.f, 140.f);
		pEffect->fRadiusAngle = Engine::GetRandomFloat(-360.f, 360.f);
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(2.f, 4.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos + vRight * Engine::GetRandomFloat(13.f, 20.f);

		pEffect->fSizeDelta = 1.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.7f;
		pEffect->fAlphaWayDelta = -70.f;
		
		pEffect->fRollDelta = 60.f;
		break;

	case 1:
		pEffect->vColor = MyVec3(39.f, 35.f, 27.f);
		pEffect->vAngle = MyVec3(-30.f, Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.4f, 0.6f);
		pEffect->fSpeed = Engine::GetRandomFloat(5.f, 6.f);
		pEffect->fGravity = 5.f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(110.f, 140.f);
		pEffect->fRadiusAngle = Engine::GetRandomFloat(-360.f, 360.f);
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(1.f, 2.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos + vRight * Engine::GetRandomFloat(13.f, 20.f);

		pEffect->fSizeDelta = 0.5f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.7f;
		pEffect->fAlphaWayDelta = -70.f;

		pEffect->fRollDelta = 190.f;
		break;

	case 2:
		pEffect->vColor = MyVec3(39.f, 35.f, 27.f);
		pEffect->vAngle = MyVec3(-30.f, Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.5f, 0.7f);
		pEffect->fSpeed = Engine::GetRandomFloat(3.f, 4.f);
		pEffect->fGravity = 5.f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(110.f, 140.f);
		pEffect->fRadiusAngle = Engine::GetRandomFloat(-360.f, 360.f);
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(1.f, 2.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos + vRight * Engine::GetRandomFloat(13.f, 20.f);

		pEffect->fSizeDelta = 2.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.7f;
		pEffect->fAlphaWayDelta = -70.f;

		pEffect->fRollDelta = 160.f;
		break;

	case 3:
		pEffect->vColor = MyVec3(117.f, 108.f, 91.f);
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(0.f, 10.f), Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.8f, 1.3f);
		pEffect->fSpeed = Engine::GetRandomFloat(5.f, 8.f);
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(70.f, 150.f);
		pEffect->fRadiusAngle = Engine::GetRandomFloat(-360.f, 360.f);
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(2.f, 4.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos + vRight * Engine::GetRandomFloat(13.f, 20.f);

		pEffect->fSizeDelta = 14.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.7f;
		pEffect->fAlphaWayDelta = -70.f;

		pEffect->fRollDelta = 40.f;
		break;

	case 4:
		pEffect->vColor = MyVec3(117.f, 108.f, 91.f);
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(0.f, 10.f), Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(1.1f, 1.4f);
		pEffect->fSpeed = Engine::GetRandomFloat(5.f, 6.f);
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(70.f, 150.f);
		pEffect->fRadiusAngle = Engine::GetRandomFloat(-360.f, 360.f);
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(2.f, 4.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos + vRight * Engine::GetRandomFloat(13.f, 20.f);

		pEffect->fSizeDelta = 11.f;
		pEffect->fSizeWayTime = 0.5f;
		pEffect->fSizeWayDelta = 7.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.7f;
		pEffect->fAlphaWayDelta = -70.f;

		pEffect->fRollDelta = 40.f;
		break;

	case 5:
		pEffect->vColor = MyVec3(117.f, 108.f, 91.f);
		pEffect->vAngle = MyVec3(30.f, Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vAngleDelta = MyVec3(30.f, 360.f, 0.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.3f, 0.4f);
		pEffect->fSpeed = Engine::GetRandomFloat(3.f, 7.f);
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(70.f, 150.f);
		pEffect->fRadiusAngle = Engine::GetRandomFloat(-360.f, 360.f);
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		pEffect->vScale = MyVec3(Engine::GetRandomFloat(2.f, 3.f), Engine::GetRandomFloat(10.f, 15.f), 1.f);
		pEffect->vScaleDelta = MyVec3(0.f, -8.f, 0.f);
		pEffect->vPos = vPos + vRight * Engine::GetRandomFloat(13.f, 20.f);

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.3f;
		pEffect->fAlphaWayDelta = -5.f;

		pEffect->fRollDelta = 40.f;
		break;

	case 6:
		pEffect->vColor = MyVec3(117.f, 108.f, 91.f);
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(0.f, 10.f), Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(60.f, 360.f));
		pEffect->vAngleDelta = MyVec3(30.f, 360.f, 0.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.2f, 0.3f);
		pEffect->fSpeed = Engine::GetRandomFloat(4.f, 6.f);
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 15.f);
		pEffect->fRadiusAngle = Engine::GetRandomFloat(-360.f, 360.f);
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(9.f, 13.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos + vRight * Engine::GetRandomFloat(13.f, 20.f);

		pEffect->fSizeDelta = 9.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.1f;
		pEffect->fAlphaWayDelta = -100.f;

		pEffect->fRollDelta = 60.f;
		break;

	case 7:
		pEffect->vColor = MyVec3(117.f, 108.f, 91.f);
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(80.f, 90.f), Engine::GetRandomFloat(-360.f, 360.f), 0.f);
		pEffect->vAngleDelta = MyVec3(30.f, 360.f, 0.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.2f, 0.3f);
		pEffect->fSpeed = Engine::GetRandomFloat(4.f, 8.f);
		pEffect->fAngle = 25.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 15.f);
		pEffect->fRadiusAngle = Engine::GetRandomFloat(-360.f, 360.f);
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(25.f, 35.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos + vRight * Engine::GetRandomFloat(13.f, 20.f);

		pEffect->fSizeDelta = 15.f;

		pEffect->fAlphaDelta = 0.f;

		pEffect->fRollDelta = -60.f;
		break;

	case 10:
		pEffect->vColor = MyVec3(70.f, 57.f, 0.f);//MyVec3(117.f, 108.f, 91.f);
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(0.f, 10.f), Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(1.f, 2.f);
		pEffect->fSpeed = Engine::GetRandomFloat(4.f, 8.f);
		pEffect->fAngle = Engine::GetRandomFloat(-10.f, 10.f);
		pEffect->fRadius = 1.f;
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * cosf(pEffect->fAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle);
		pEffect->vDir.y = Engine::GetRandomFloat(5.f, 10.f);
		fSize = Engine::GetRandomFloat(5.f, 8.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos + vRight * Engine::GetRandomFloat(3.f, 15.f) + vLook * Engine::GetRandomFloat(-5.f, 5.f);

		pEffect->fSizeDelta = 22.f;
		pEffect->fSizeWayTime = 0.1f;
		pEffect->fSizeWayDelta = 15.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.5f;
		pEffect->fAlphaWayDelta = -70.f;

		pEffect->fRollDelta = -60.f;
		break;

	case 11:
		pEffect->vColor = MyVec3(117.f, 108.f, 91.f);
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(0.f, 10.f), Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(1.f, 1.3f);
		pEffect->fSpeed = Engine::GetRandomFloat(8.f, 23.f);
		pEffect->fAngle = Engine::GetRandomFloat(-10.f, 10.f);
		pEffect->fRadius = 1.f;
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * cosf(pEffect->fAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle);
		pEffect->vDir.y = Engine::GetRandomFloat(5.f, 10.f);
		fSize = Engine::GetRandomFloat(2.f, 4.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos + vRight * Engine::GetRandomFloat(3.f, 15.f) + vLook * Engine::GetRandomFloat(-5.f, 5.f);

		pEffect->fSizeDelta = 3.f;
		pEffect->fSizeWayTime = 0.3f;
		pEffect->fSizeWayDelta = 3.f;

		pEffect->fAlphaDelta = 0.f;

		pEffect->fRollDelta = 265.f;
		break;

	case 12:
		pEffect->vColor = MyVec3(117.f, 108.f, 91.f);
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(0.f, 10.f), Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.5f, 0.7f);
		pEffect->fSpeed = Engine::GetRandomFloat(4.f, 8.f);
		pEffect->fAngle = Engine::GetRandomFloat(-10.f, 10.f);
		pEffect->fRadius = 1.f;
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * cosf(pEffect->fAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle);
		pEffect->vDir.y = Engine::GetRandomFloat(5.f, 10.f);
		fSize = Engine::GetRandomFloat(2.f, 4.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos + vRight * Engine::GetRandomFloat(3.f, 15.f) + vLook * Engine::GetRandomFloat(-5.f, 5.f);

		pEffect->fSizeDelta = 7.f;
		pEffect->fSizeWayTime = 0.1f;
		pEffect->fSizeWayDelta = 3.f;

		pEffect->fAlphaDelta = 0.f;

		pEffect->fRollDelta = -60.f;
		break;
	}

	D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
	pEffect->vPos.y = 0.5f;
	pEffect->fAlphaDelta -= 255.f;
	m_EffectList.push_back(pEffect);
}

HRESULT CSwingDust::Ready_Component( void )
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

void CSwingDust::Free( void )
{
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pTextureCom);
	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pShaderCom);

	Engine::CGameObject::Free();
}

CSwingDust* CSwingDust::Create( LPDIRECT3DDEVICE9 pDevice, const _uint& iType)
{
	CSwingDust* pSwingDust = new CSwingDust(pDevice);

	if (FAILED(pSwingDust->Initialize(iType)))
	{
		MSG_BOX("SwingDust Created Failed");
		Engine::Safe_Release(pSwingDust);
	}
	return pSwingDust;
}



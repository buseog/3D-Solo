
#include "StdAfx.h"
#include "ManaBlade.h"
#include "DefaultCamera.h"

CManaBlade::CManaBlade( LPDIRECT3DDEVICE9 pDevice)
: CEffect(pDevice)
{
	
}

CManaBlade::~CManaBlade( void )
{

}

HRESULT CManaBlade::Initialize( const _uint& iType, MyMat* pTarget, MyMat* pBone)
{
	m_iType = iType;
	m_pBone = pBone;
	m_pTarget = pTarget;
	m_vAngle = *((CDefaultCamera*)Engine::Get_CameraMgr()->GetCamera(L"Cam_Default"))->GetAngle();

	m_vOrigin = *(MyVec3*)&(*m_pBone * *m_pTarget).m[3][0];


	DefaultSetting();

	if (FAILED(Ready_Component()))
		return E_FAIL;

	for (_uint i = 0; i < m_iCount; ++i)
		AddParticle();

	return S_OK;
}

_int CManaBlade::Update( const _float& fDelta )
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

void CManaBlade::Render( void )
{
	LPD3DXEFFECT pEffect = m_pShaderCom->GetEffect();
	if (pEffect == NULL)
		return;

	SetUpShader(pEffect);
}

void CManaBlade::SetUpShader( LPD3DXEFFECT pEffect )
{
	pEffect->AddRef();

	MyMat matView, matProj;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	m_pTextureCom->SetUpShader(pEffect, m_iIndex);
	
	pEffect->Begin(NULL, 0);
	if (m_iType == 2 || m_iType == 3 || m_iType == 10 || m_iType == 11 || m_iType == 14)
		pEffect->BeginPass(4);
	else
		pEffect->BeginPass(1);

	EFFECTLIST::iterator iter = m_EffectList.begin();
	EFFECTLIST::iterator iter_end = m_EffectList.end();
	MyMat matScale, matRotX, matRotY, matRotZ, matTrans, matWorld;

	for ( ; iter != iter_end; ++iter)
	{
		if ((*iter)->bAlive == true)
		{
			_float	fSizeDelta = GetSizeDelta(*iter);
			_float	fRollDelta = GetRollDelta(*iter);
			_uint	iAlphaDelta = GetAlphaDelta(*iter);
			D3DXMatrixScaling(&matScale, (*iter)->vScale.x + fSizeDelta, (*iter)->vScale.y + fSizeDelta, (*iter)->vScale.z);
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

void CManaBlade::DefaultSetting( void )
{
	switch (m_iType)
	{
	// idle fire 
	case 0:	// ½ºÆÄÅ©
		m_pEffectKey = L"Component_Texture_LightGlow";
		m_iCount = 1;
		m_iIndex = 2;

		m_fInterval = 0.6f;
		m_fDuration = 0.5f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 1:	// ½ºÆÄÅ©
		m_pEffectKey = L"Component_Texture_LightGlow";
		m_iCount = 1;
		m_iIndex = 2;

		m_fInterval = 0.04f;
		m_fDuration = 0.5f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 2:	// ½ºÆÄÅ©
		m_pEffectKey = L"Component_Texture_Fire_Seed_D_a_s";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.15f;
		m_fDuration = 0.5f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 3:	// ½ºÆÄÅ©
		m_pEffectKey = L"Component_Texture_Fire_Seed_C_a_s";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.25f;
		m_fDuration = 0.5f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	// mb begin hand
	case 10: // ¼­ºêºÒ
		m_pEffectKey = L"Component_Texture_Fire_Seed_D_a_s";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.03f;
		m_fDuration = 1.1f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 11: // ¼­ºêºÒ
		m_pEffectKey = L"Component_Texture_Fire_Seed_C_a_s";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.05f;
		m_fDuration = 1.1f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 12: // ¼Õºû
		m_pEffectKey = L"Component_Texture_LightRay";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.02f;
		m_fDuration = 0.7f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 13: // ¼Õºû
		m_pEffectKey = L"Component_Texture_LightGlow";
		m_iCount = 1;
		m_iIndex = 2;

		m_fInterval = 0.02f;
		m_fDuration = 0.65f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 14: // Ä®³¯ºÒ
		m_pEffectKey = L"Component_Texture_Fire_Seed_C_a_s";
		m_iCount = 1;
		m_iIndex = 2;

		m_fInterval = 0.03f;
		m_fDuration = 0.6f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 15: // Æ¢´ÂºÒ²É1
		m_pEffectKey = L"Component_Texture_YellowFlare";
		m_iCount = (_int)Engine::GetRandomFloat(1.f, 3.f);
		m_iIndex = 0;

		m_fInterval = 0.01f;
		m_fDuration = 0.5f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 16: // Æ¢´ÂºÒ²É2
		m_pEffectKey = L"Component_Texture_YellowFlare";
		m_iCount = (_int)Engine::GetRandomFloat(1.f, 3.f);
		m_iIndex = 0;

		m_fInterval = 0.01f;
		m_fDuration = 0.6f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;
	}
}

void CManaBlade::AddParticle( void )
{
	EFFECT* pEffect= new EFFECT;
	ZeroMemory(pEffect, sizeof(EFFECT));

	pEffect->vScale = MyVec3(1.f, 1.f, 1.f);
	D3DXVECTOR3 vMin(131.f, 149.f, 250.f);
	D3DXVECTOR3 vMax(181.f, 150.f, 250.f);
	Engine::GetRandomVector(&pEffect->vColor, &vMin, &vMax);

	MyMat matCompute = *m_pBone * *m_pTarget;
	MyVec3 vPos = *(MyVec3*)&matCompute.m[3][0];

	_float fSize = 0.f;

	switch (m_iType)
	{
	case 0:
		pEffect->vAngle = MyVec3(-90.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(131.f, 149.f, 250.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.7f, 0.9f);
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(3.f, 5.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 4.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.5f;
		pEffect->fAlphaWayDelta = 80.f;

		pEffect->fRollDelta = Engine::GetRandomFloat(-100.f, 100.f);

		pEffect->bAlive = true;
		break;

	case 1:
		pEffect->vAngle = MyVec3(-90.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.5f, 0.7f);
		pEffect->fSpeed = Engine::GetRandomFloat(1.f, 3.f);
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 3.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(1.f, 1.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = -0.6f;
		pEffect->fSizeWayTime = 0.01f;
		pEffect->fSizeWayDelta = -0.2f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.2f;
		pEffect->fAlphaWayDelta = 220.f;

		pEffect->fRollDelta = 60.f;

		pEffect->bAlive = true;
		break;

	case 2:
		pEffect->vAngle = MyVec3(-90.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(131.f, 149.f, 250.f);
		pEffect->fMaxLife = 0.7f;
		pEffect->fSpeed = Engine::GetRandomFloat(0.f, 5.f);
		pEffect->fFrameSpeed = 30.f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 2.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(3.f, 3.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 1.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.3f;
		pEffect->fAlphaWayDelta = 250.f;

		pEffect->fRollDelta = 30.f;

		pEffect->bAlive = true;
		break;

	case 3:
		pEffect->vAngle = MyVec3(-90.f, 0.f, Engine::GetRandomFloat(-15.f, 15.f));
		pEffect->vColor = MyVec3(131.f, 149.f, 250.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.5f, 0.7f);
		pEffect->fSpeed = Engine::GetRandomFloat(5.f, 10.f);
		pEffect->fFrameSpeed = 40.f;
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

		pEffect->fSizeDelta = -1.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.3f;
		pEffect->fAlphaWayDelta = 250.f;

		pEffect->fRollDelta = 5.f;

		pEffect->bAlive = true;
		break;

	case 10:
		pEffect->vAngle = MyVec3(-90.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(131.f, 149.f, 250.f);
		pEffect->fMaxLife = 0.6f;
		pEffect->fSpeed = Engine::GetRandomFloat(0.f, 3.f);
		pEffect->fFrameSpeed = 30.f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(2.f, 4.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(3.f, 4.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 1.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.3f;
		pEffect->fAlphaWayDelta = 255.f;

		pEffect->fRollDelta = 30.f;

		pEffect->bAlive = true;
		break;

	case 11:
		pEffect->vAngle = MyVec3(-90.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(131.f, 149.f, 250.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.5f, 0.7f);
		pEffect->fSpeed = Engine::GetRandomFloat(2.f, 3.f);
		pEffect->fFrameSpeed = 40.f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(2.f, 5.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(2.f, 3.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 1.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.3f;
		pEffect->fAlphaWayDelta = 255.f;

		pEffect->fRollDelta = 5.f;

		pEffect->bAlive = true;
		break;

	case 12:
		pEffect->vAngle = MyVec3(-90.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(131.f, 149.f, 250.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.2f, 0.3f);
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

		pEffect->fSizeDelta = 1.5f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.3f;
		pEffect->fAlphaWayDelta = 105.f;

		pEffect->fRollDelta = 5.f;

		pEffect->bAlive = true;
		break;

	case 13:
		pEffect->vAngle = MyVec3(-90.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(131.f, 149.f, 250.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.2f, 0.3f);
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(4.f, 8.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 1.5f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.3f;
		pEffect->fAlphaWayDelta = 25.f;

		pEffect->fRollDelta = 5.f;

		pEffect->bAlive = true;
		break;

	case 14:
		pEffect->vAngle = MyVec3(-90.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(131.f, 149.f, 250.f);
		pEffect->fMaxLife = 0.7f;
		pEffect->fSpeed = Engine::GetRandomFloat(0.f, 3.f);
		pEffect->fFrameSpeed = 0.f;
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

		pEffect->fSizeDelta = 1.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.3f;
		pEffect->fAlphaWayDelta = 255.f;

		pEffect->fRollDelta = 30.f;

		pEffect->bAlive = true;
		break;
	
	case 15:
		pEffect->vAngle = MyVec3(-110.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 255.f, 255.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.3f, 0.4f);
		pEffect->fSpeed = Engine::GetRandomFloat(1.f, 2.f);
		pEffect->fAngle = Engine::GetRandomFloat(-10.f, 10.f);
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(1.f, 2.f);
		pEffect->vScale = MyVec3(Engine::GetRandomFloat(0.2f, 0.3f) + fSize, Engine::GetRandomFloat(0.6f, 0.9f) + fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = -0.08f;
		pEffect->fAlphaDelta = 255.f;

		pEffect->bAlive = true;
		break;

	case 16:
		pEffect->vAngle = MyVec3(-90.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 255.f, 255.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.3f, 0.4f);
		pEffect->fSpeed = Engine::GetRandomFloat(4.f, 5.f);
		pEffect->fAngle = Engine::GetRandomFloat(-10.f, 10.f);
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(1.f, 2.f);
		pEffect->vScale = MyVec3(Engine::GetRandomFloat(0.2f, 0.3f) + fSize, Engine::GetRandomFloat(0.6f, 0.9f) + fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = -0.08f;
		pEffect->fAlphaDelta = 255.f;

		pEffect->bAlive = true;
		break;

	}

	pEffect->vAngle = m_vAngle;
	pEffect->fAlphaDelta -= 255.f;
	m_EffectList.push_back(pEffect);
}

HRESULT CManaBlade::Ready_Component( void )
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

void CManaBlade::Free( void )
{
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pTextureCom);
	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pShaderCom);

	Engine::CGameObject::Free();
}

CManaBlade* CManaBlade::Create( LPDIRECT3DDEVICE9 pDevice, const _uint& iType, MyMat* pTarget, MyMat* pBone)
{
	CManaBlade* pManaBlade = new CManaBlade(pDevice);

	if (FAILED(pManaBlade->Initialize(iType, pTarget, pBone)))
	{
		MSG_BOX("ManaBlade Created Failed");
		Engine::Safe_Release(pManaBlade);
	}
	return pManaBlade;
}



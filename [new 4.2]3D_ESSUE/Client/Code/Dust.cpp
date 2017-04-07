#include "StdAfx.h"
#include "Dust.h"
#include "DefaultCamera.h"

CDust::CDust( LPDIRECT3DDEVICE9 pDevice)
: CEffect(pDevice)
{
	
}

CDust::~CDust( void )
{

}

HRESULT CDust::Initialize( const _uint& iType, MyMat* pTarget, MyMat* pBone)
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

_int CDust::Update( const _float& fDelta )
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

			(*iter)->vPos += (*iter)->vDir * (*iter)->fSpeed * fDelta;

			if ((*iter)->fLifeTime >= (*iter)->fMaxLife)
				(*iter)->bAlive = false;
		}
	}

	Engine::CGameObject::Update(fDelta);
	m_pRendererCom->AddRenderList(Engine::CRenderer::RENDER_ALPHA, this);

	return 0;
}

void CDust::Render( void )
{
	LPD3DXEFFECT pEffect = m_pShaderCom->GetEffect();
	if (pEffect == NULL)
		return;

	SetUpShader(pEffect);
}

void CDust::SetUpShader( LPD3DXEFFECT pEffect )
{
	pEffect->AddRef();

	MyMat matView, matProj;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	m_pTextureCom->SetUpShader(pEffect, m_iIndex);
	
	pEffect->Begin(NULL, 0);
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

void CDust::DefaultSetting( void )
{
	switch (m_iType)
	{
	case 0: // »ÏÁ·¸ÕÁö
		m_pEffectKey = L"Component_Texture_Dust";
		m_iCount = 3;
		m_iIndex = 0;

		m_fInterval = 0.04f;
		m_fDuration = 0.1f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 1: // ¸ù±Û¸ÕÁö µÚ 1
		m_pEffectKey = L"Component_Texture_Dust";
		m_iCount = 2;
		m_iIndex = 0;

		m_fInterval = 0.03f;
		m_fDuration = 0.3f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 2: // ¹Ù´Ú ÆÄÆí µÚ 1
		m_pEffectKey = L"Component_Texture_YellowFlare";
		m_iCount = 2;
		m_iIndex = 0;

		m_fInterval = 0.03f;
		m_fDuration = 0.3f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 10: // »ÏÁ·¸ÕÁö
		m_pEffectKey = L"Component_Texture_Dust";
		m_iCount = 3;
		m_iIndex = 0;

		m_fInterval = 0.04f;
		m_fDuration = 0.1f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 11: // ¸ù±Û¸ÕÁö 1
		m_pEffectKey = L"Component_Texture_Dust";
		m_iCount = 2;
		m_iIndex = 0;

		m_fInterval = 0.03f;
		m_fDuration = 0.15f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 12: // ¹Ù´Ú ÆÄÆí 1
		m_pEffectKey = L"Component_Texture_YellowFlare";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.03f;
		m_fDuration = 0.2f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	}
}

void CDust::AddParticle( void )
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
		pEffect->vColor = MyVec3(131.f, 149.f, 250.f);
		pEffect->vAngle = MyVec3(-30.f, 0.f, 0.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.2f, 0.3f);
		pEffect->fSpeed = Engine::GetRandomFloat(5.f, 6.f);
		pEffect->fAngle = 25.f;
		pEffect->fRadius = 1.f;
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		pEffect->vScale = MyVec3(Engine::GetRandomFloat(3.f, 4.f), 8.f, 1.f);
		pEffect->vPos = vPos;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.5f;
		pEffect->fAlphaWayDelta = 180.f;

		pEffect->bAlive = true;
		break;

	case 1:
		pEffect->vColor = MyVec3(131.f, 149.f, 250.f);
		pEffect->vAngle = MyVec3(-40.f, 0.f, 0.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.3f, 0.7f);
		pEffect->fSpeed = Engine::GetRandomFloat(3.f, 5.f);
		pEffect->fAngle = 50.f;
		pEffect->fRadius = Engine::GetRandomFloat(10.f, 30.f);
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(2.f, 4.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 2.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.5f;
		pEffect->fAlphaWayDelta = 100.f;

		pEffect->fRollDelta = 40.f;

		pEffect->bAlive = true;
		break;

	case 2:
		pEffect->vColor = MyVec3(131.f, 149.f, 250.f);
		pEffect->vAngle = MyVec3(-30.f, 0.f, 0.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.4f, 0.6f);
		pEffect->fSpeed = Engine::GetRandomFloat(2.f, 5.f);
		pEffect->fAngle = 40.f;
		pEffect->fRadius = 20.f;
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(2.f, 4.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.7f;
		pEffect->fAlphaWayDelta = 200.f;

		pEffect->fRollDelta = 80.f;

		pEffect->bAlive = true;
		break;

	case 10:
		pEffect->vColor = MyVec3(131.f, 149.f, 250.f);
		pEffect->vAngle = MyVec3(30.f, 180.f, 0.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.2f, 0.3f);
		pEffect->fSpeed = Engine::GetRandomFloat(20.f, 30.f);
		pEffect->fAngle = 25.f;
		pEffect->fRadius = 1.f;
		pEffect->fRadiusAngle = 30.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		pEffect->vScale = MyVec3(Engine::GetRandomFloat(3.f, 4.f), 8.f, 1.f);
		pEffect->vPos = vPos;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.5f;
		pEffect->fAlphaWayDelta = 180.f;

		pEffect->bAlive = true;
		break;

	case 11:
		pEffect->vColor = MyVec3(131.f, 149.f, 250.f);
		pEffect->vAngle = MyVec3(140.f, 0.f, 0.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.3f, 0.7f);
		pEffect->fSpeed = Engine::GetRandomFloat(1.f, 10.f);
		pEffect->fAngle = 50.f;
		pEffect->fRadius = Engine::GetRandomFloat(10.f, 30.f);
		pEffect->fRadiusAngle = 30.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(3.f, 6.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 3.f;
		pEffect->fSizeWayTime = 0.5f;
		pEffect->fSizeWayDelta = 2.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.5f;
		pEffect->fAlphaWayDelta = 100.f;

		pEffect->fRollDelta = 40.f;

		pEffect->bAlive = true;
		break;

	case 12:
		pEffect->vColor = MyVec3(131.f, 149.f, 250.f);
		pEffect->vAngle = MyVec3(-50.f, 0.f, 0.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.5f, 0.7f);
		pEffect->fSpeed = Engine::GetRandomFloat(20.f, 35.f);
		pEffect->fAngle = 40.f;
		pEffect->fRadius = 20.f;
		pEffect->fRadiusAngle = 30.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(2.f, 4.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.7f;
		pEffect->fAlphaWayDelta = 200.f;

		pEffect->fRollDelta = 80.f;

		pEffect->bAlive = true;
		break;
	}

	pEffect->vAngle = m_vAngle;

	m_EffectList.push_back(pEffect);
}

HRESULT CDust::Ready_Component( void )
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

void CDust::Free( void )
{
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pTextureCom);
	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pShaderCom);

	Engine::CGameObject::Free();
}

CDust* CDust::Create( LPDIRECT3DDEVICE9 pDevice, const _uint& iType, MyMat* pTarget, MyMat* pBone)
{
	CDust* pDust = new CDust(pDevice);

	if (FAILED(pDust->Initialize(iType, pTarget, pBone)))
	{
		MSG_BOX("Dust Created Failed");
		Engine::Safe_Release(pDust);
	}
	return pDust;
}



#include "StdAfx.h"
#include "RuinWave.h"

CRuinWave::CRuinWave( LPDIRECT3DDEVICE9 pDevice)
: CEffect(pDevice)
{
	
}

CRuinWave::~CRuinWave( void )
{

}

HRESULT CRuinWave::Initialize( const _uint& iType, MyMat* pTarget, MyMat* pBone)
{
	m_iType = iType;
	m_pBone = pBone;
	m_pTarget = pTarget;
	m_vOrigin = *(MyVec3*)&(*m_pBone * *m_pTarget).m[3][0];
	m_vAngle = *((Engine::CTransform*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(0)->GetComponent(L"Com_Transform"))->GetInfomation(Engine::CTransform::INFO_ANGLE);
	m_vAngle.y = D3DXToDegree(m_vAngle.y);
	m_fDis = 1.f;

	DefaultSetting();

	if (FAILED(Ready_Component()))
		return E_FAIL;

	for (_uint i = 0; i < m_iCount; ++i)
		AddParticle();

	return S_OK;
}

_int CRuinWave::Update( const _float& fDelta )
{
	if (isDead() == true && m_fDurationAcc >= m_fDuration)
	{
		m_bDestroy = true;
		return 0;
	}

	m_fDis += fDelta;

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

void CRuinWave::Render( void )
{
	LPD3DXEFFECT pEffect = m_pShaderCom->GetEffect();
	if (pEffect == NULL)
		return;

	SetUpShader(pEffect);
}

void CRuinWave::SetUpShader( LPD3DXEFFECT pEffect )
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
	MyMat matRot;

	for ( ; iter != iter_end; ++iter)
	{
		if ((*iter)->bAlive == true)
		{
			_float	fSizeDelta = GetSizeDelta(*iter);
			_float	fRollDelta = GetRollDelta(*iter);
			_uint	iAlphaDelta = GetAlphaDelta(*iter);
			MyVec3	vScaleDelta = GetScaleDelta(*iter);
			MyVec3	vColorDelta = GetColorDelta(*iter);
			D3DXMatrixScaling(&matScale, (*iter)->vScale.x + fSizeDelta + vScaleDelta.x, (*iter)->vScale.y + fSizeDelta + vScaleDelta.y, (*iter)->vScale.z);
			D3DXMatrixRotationX(&matRotX, D3DXToRadian((*iter)->vAngle.x));
			D3DXMatrixRotationY(&matRotY, D3DXToRadian((*iter)->vAngle.y));
			D3DXMatrixRotationZ(&matRotZ, D3DXToRadian((*iter)->vAngle.z + fRollDelta));
			D3DXMatrixTranslation(&matTrans, (*iter)->vPos.x, (*iter)->vPos.y, (*iter)->vPos.z);
			matWorld = matScale * matRotZ * matRotX * matRotY * matTrans;
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

void CRuinWave::DefaultSetting( void )
{
	switch (m_iType)
	{
	case 0: // ¿Ã¶ó°¡´ÂºÒ1
		m_pEffectKey = L"Component_Texture_YellowFlare";
		m_iCount = 1;
		m_iIndex = 3;

		m_fInterval = 0.1f;
		m_fDuration = 3.f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 1: // ¿Ã¶ó°¡´ÂºÒ2
		m_pEffectKey = L"Component_Texture_IceLight";
		m_iCount = 1;
		m_iIndex = 2;

		m_fInterval = 0.15f;
		m_fDuration = 3.f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 2: // ¿Ã¶ó°¡´ÂºÒ3
		m_pEffectKey = L"Component_Texture_IceLight";
		m_iCount = 1;
		m_iIndex = 1;

		m_fInterval = 0.18f;
		m_fDuration = 3.f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 3: // ¹Ù´Úºû1
		break;
	case 4: // ºû1
		break;
	case 5: // ¹ßµ¿ºû
		break;
	case 6: // ¹ßµ¿ºû2
		break;
	case 7: // ¹ßµ¿ºû2
		break;

	case 10: // ¸µ1
		m_pEffectKey = L"Component_Texture_Ring_Outer_Wind";
		m_iCount = 5;
		m_iIndex = 2;

		m_fInterval = 0.3f;
		m_fDuration = 2.f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 11: // »ÏÁ·1
		m_pEffectKey = L"Component_Texture_Blunt";
		m_iCount = 15;
		m_iIndex = 0;

		m_fInterval = 0.3f;
		m_fDuration = 2.f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 12: // »ÏÁ·2
		m_pEffectKey = L"Component_Texture_PartiSpray";
		m_iCount = 3;
		m_iIndex = 0;

		m_fInterval = 0.3f;
		m_fDuration = 2.f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 13: // »ÏÁ·3
		m_pEffectKey = L"Component_Texture_IceLight";
		m_iCount = 2;
		m_iIndex = 1;

		m_fInterval = 0.1f;
		m_fDuration = 2.f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 14: // »ÏÁ·4
		m_pEffectKey = L"Component_Texture_Blunt";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.3f;
		m_fDuration = 2.f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;
	}
}

void CRuinWave::AddParticle( void )
{
	EFFECT* pEffect= new EFFECT;
	ZeroMemory(pEffect, sizeof(EFFECT));

	pEffect->vScale = MyVec3(1.f, 1.f, 1.f);
	D3DXVECTOR3 vMin;
	D3DXVECTOR3 vMax;

	MyMat matCompute = *m_pBone * *m_pTarget;
	MyVec3 vPos = *(MyVec3*)&matCompute.m[3][0];
	MyVec3 vLook = *D3DXVec3Normalize(&vLook, (MyVec3*)&m_pTarget->m[2][0]);

	_float fSize = 0.f;

	switch (m_iType)
	{
	case 0:
		pEffect->vAngle = MyVec3(-90.f, 0.f, 35.f + Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 160.f, 90.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.2f, 0.4f);
		pEffect->fSpeed = Engine::GetRandomFloat(23.f, 28.f);
		pEffect->fAngle = 10.f;
		pEffect->fRadius = Engine::GetRandomFloat(5.f, 15.f);
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = Engine::GetRandomFloat(5.f, 15.f);
		pEffect->vDir.x = pEffect->fRadius * cosf(pEffect->fAngle);
		pEffect->vDir.y = pEffect->fRadius * sinf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(3.f, 5.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = -2.f;
		pEffect->fSizeWayTime = 0.4f;
		pEffect->fSizeWayDelta = 4.f;
		
		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.5f;
		pEffect->fAlphaWayDelta = 200.f;

		pEffect->fRollDelta = 40.f;

		pEffect->bAlive = true;
		break;

	case 1:
		pEffect->vAngle = MyVec3(-90.f, 0.f, 10.f + Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 160.f, 90.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.5f, 0.7f);
		pEffect->fSpeed = Engine::GetRandomFloat(14.f, 17.f);
		pEffect->fAngle = 5.f;
		pEffect->fRadius = Engine::GetRandomFloat(5.f, 15.f);
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = Engine::GetRandomFloat(5.f, 15.f);
		pEffect->vDir.x = pEffect->fRadius * cosf(pEffect->fAngle);
		pEffect->vDir.y = pEffect->fRadius * sinf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(3.f, 5.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = -1.f;
		pEffect->fSizeWayTime = 0.2f;
		pEffect->fSizeWayDelta = 3.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.001f;
		pEffect->fAlphaWayDelta = 250.f;

		pEffect->fRollDelta = 40.f;

		pEffect->bAlive = true;
		break;

	case 2:
		pEffect->vAngle = MyVec3(-90.f, 0.f, 10.f + Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 160.f, 90.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.4f, 0.6f);
		pEffect->fSpeed = Engine::GetRandomFloat(12.f, 18.f);
		pEffect->fAngle = 15.f;
		pEffect->fRadius = Engine::GetRandomFloat(5.f, 15.f);
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = Engine::GetRandomFloat(5.f, 15.f);
		pEffect->vDir.x = pEffect->fRadius * cosf(pEffect->fAngle);
		pEffect->vDir.y = pEffect->fRadius * sinf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(7.f, 10.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = -4.f;
		pEffect->fSizeWayTime = 0.2f;
		pEffect->fSizeWayDelta = 3.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.2f;
		pEffect->fAlphaWayDelta = 250.f;

		pEffect->fRollDelta = 40.f;

		pEffect->bAlive = true;
		break;

	case 10:
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(80.f, 90.f), 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(235.f, 165.f, 125.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.15f, 0.2f);
		pEffect->fSpeed = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fAngle = 20.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = 0.f;
		pEffect->vDir.x = pEffect->fRadius * cosf(pEffect->fAngle);
		pEffect->vDir.y = pEffect->fRadius * sinf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(15.f, 25.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		
		pEffect->fSizeDelta = 65.f;
		pEffect->vPos = vPos;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.4f;
		pEffect->fAlphaWayDelta = 220.f;

		pEffect->fRollDelta = 40.f;

		pEffect->bAlive = true;
		break;

	case 11:
		pEffect->vAngle = MyVec3(0.f, Engine::GetRandomFloat(-360.f, 360.f), 0.f);
		pEffect->vColor = MyVec3(235.f, 165.f, 85.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.15f, 0.2f);
		pEffect->fSpeed = Engine::GetRandomFloat(25.f, 40.f);
		pEffect->fAngle = 30.f;
		pEffect->fRadius = Engine::GetRandomFloat(40.f, 45.f);
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = 0.f;
		pEffect->vDir.x = pEffect->fRadius * cosf(pEffect->fAngle);
		pEffect->vDir.y = pEffect->fRadius * sinf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		pEffect->vScale = MyVec3(Engine::GetRandomFloat(10.f, 15.f), Engine::GetRandomFloat(15.f, 25.f), 1.f);
		pEffect->vPos = vPos + MyVec3(0.f, -10.f, 0.f);

		pEffect->vScaleDelta = MyVec3(70.f, 95.f, 0.f);

		pEffect->fAlphaDelta = 250.f;
		pEffect->fAlphaWayTime = 0.4f;
		pEffect->fAlphaWayDelta = 150.f;

		pEffect->bAlive = true;
		break;

	case 12:
		pEffect->vAngle = MyVec3(0.f, Engine::GetRandomFloat(-360.f, 360.f), 180.f);
		pEffect->vColor = MyVec3(235.f, 165.f, 125.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.2f, 0.25f);
		pEffect->fSpeed = Engine::GetRandomFloat(15.f, 20.f);
		pEffect->fAngle = 45.f;
		pEffect->fRadius = Engine::GetRandomFloat(35.f, 45.f);
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = 0.f;
		pEffect->vDir.x = pEffect->fRadius * cosf(pEffect->fAngle);
		pEffect->vDir.y = pEffect->fRadius * sinf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		pEffect->vScale = MyVec3(Engine::GetRandomFloat(44.f, 48.f), Engine::GetRandomFloat(15.f, 17.f), 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 5.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.7f;
		pEffect->fAlphaWayDelta = 255.f;

		pEffect->bAlive = true;
		break;

	case 13:
		pEffect->vAngle = MyVec3(0.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(235.f, 165.f, 85.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.3f, 0.35f);
		pEffect->fSpeed = Engine::GetRandomFloat(15.f, 20.f);
		pEffect->fAngle = 15.f;
		pEffect->fRadius = Engine::GetRandomFloat(35.f, 45.f);
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = 0.f;
		pEffect->vDir.x = pEffect->fRadius * cosf(pEffect->fAngle);
		pEffect->vDir.y = pEffect->fRadius * sinf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		pEffect->vScale = MyVec3(Engine::GetRandomFloat(40.f, 44.f), Engine::GetRandomFloat(35.f, 47.f), 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 5.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.7f;
		pEffect->fAlphaWayDelta = 230.f;

		pEffect->bAlive = true;
		break;

	case 14:
		pEffect->vAngle = MyVec3(0.f, Engine::GetRandomFloat(-360.f, 360.f), 0.f);
		pEffect->vColor = MyVec3(235.f, 165.f, 75.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.15f, 0.2f);
		pEffect->fSpeed = Engine::GetRandomFloat(30.f, 50.f);
		pEffect->fAngle = Engine::GetRandomFloat(0.f, 360.f);
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 10.f);
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = 0.f;
		pEffect->vDir.x = pEffect->fRadius * cosf(pEffect->fAngle);
		pEffect->vDir.y = pEffect->fRadius * sinf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		pEffect->vScale = MyVec3(Engine::GetRandomFloat(10.f, 15.f), Engine::GetRandomFloat(15.f, 25.f), 1.f);
		pEffect->vPos = vPos + MyVec3(0.f, -10.f, 0.f);

		pEffect->vScaleDelta = MyVec3(70.f, 95.f, 0.f);

		pEffect->fAlphaDelta = 250.f;
		pEffect->fAlphaWayTime = 0.6f;
		pEffect->fAlphaWayDelta = 150.f;

		pEffect->bAlive = true;
		break;
	}

	pEffect->vPos += vLook * (_int(m_fDis / 0.3f) * 10);
	pEffect->vAngle += m_vAngle;
	m_EffectList.push_back(pEffect);
}


HRESULT CRuinWave::Ready_Component( void )
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

void CRuinWave::Free( void )
{
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pTextureCom);
	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pShaderCom);

	Engine::CGameObject::Free();
}

CRuinWave* CRuinWave::Create( LPDIRECT3DDEVICE9 pDevice, const _uint& iType, MyMat* pTarget, MyMat* pBone)
{
	CRuinWave* pRuinWave = new CRuinWave(pDevice);

	if (FAILED(pRuinWave->Initialize(iType, pTarget, pBone)))
	{
		MSG_BOX("RuinWave Created Failed");
		Engine::Safe_Release(pRuinWave);
	}
	return pRuinWave;
}



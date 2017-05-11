#include "StdAfx.h"
#include "JumpSmash.h"
#include "DefaultCamera.h"
#include "Player.h"

CJumpSmash::CJumpSmash( LPDIRECT3DDEVICE9 pDevice)
: CEffect(pDevice)
, m_pPlayer(NULL)
, m_bColl(true)
{
	
}

CJumpSmash::~CJumpSmash( void )
{

}

HRESULT CJumpSmash::Initialize( const _uint& iType)
{
	m_iType = iType;
	m_pTarget = ((Engine::CTransform*)Engine::Get_Management()->GetLayer(L"Layer_Boss")->GetObject(0)->GetComponent(L"Com_Transform"))->GetWorldMatrix();
	
	m_vOrigin = *(MyVec3*)&m_pTarget->m[3][0];
	m_vLook = *D3DXVec3Normalize(&m_vLook, (MyVec3*)&m_pTarget->m[2][0]);

	if (m_iType == 0 || m_iType == 4)
	{
		m_bColl = false;
		m_pPlayer = ((Engine::CTransform*)Engine::Get_Management()->GetLayer(L"Layer_Boss")->GetObject(0)->GetComponent(L"Com_Transform"))->GetWorldMatrix();
	}

	DefaultSetting();

	if (FAILED(Ready_Component()))
		return E_FAIL;

	for (_uint i = 0; i < m_iCount; ++i)
		AddParticle(i);

	return S_OK;
}

_int CJumpSmash::Update( const _float& fDelta )
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
				if (m_iType == 0)
				{
					MyVec3 vDif = *(MyVec3*)&m_pPlayer->m[3][0] - (*iter)->vPos;
					_float fDis = fabs(D3DXVec3Length(&vDif));
					_float fRadius = ((*iter)->vScale.x + (*iter)->vScale.y) / 3.f;

					if (fDis <= fRadius)
					{
						((CPlayer*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(0))->SetDamage(80.f);
						m_bColl = true;
					}
				}
				if (m_iType == 4)
				{
					/*MyVec3 vDif = *(MyVec3*)&m_pPlayer->m[3][0] - (*iter)->vPos;
					_float fDis = fabs(D3DXVec3Length(&vDif));
					_float fRadius = ((*iter)->vScale.x + (*iter)->vScale.y) / 3.f;

					if (fDis <= fRadius)
					{
						((CPlayer*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(0))->SetDamage(0.5f);
					}*/
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

void CJumpSmash::Render( void )
{
	LPD3DXEFFECT pEffect = m_pShaderCom->GetEffect();
	if (pEffect == NULL)
		return;

	SetUpShader(pEffect);
}

void CJumpSmash::SetUpShader( LPD3DXEFFECT pEffect )
{
	pEffect->AddRef();

	MyMat matView, matProj;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	m_pTextureCom->SetUpShader(pEffect, m_iIndex);
	
	pEffect->Begin(NULL, 0);
	if (m_iType == 6)
		pEffect->BeginPass(4);
	else if (m_iType == 13 || m_iType == 22)
		pEffect->BeginPass(5);
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
			D3DXMatrixScaling(&matScale, (*iter)->vScale.x + vScaleDelta.x + fSizeDelta, (*iter)->vScale.y + vScaleDelta.y + fSizeDelta, (*iter)->vScale.z);
			D3DXMatrixRotationX(&matRotX, D3DXToRadian((*iter)->vAngle.x));
			D3DXMatrixRotationY(&matRotY, D3DXToRadian((*iter)->vAngle.y));
			D3DXMatrixRotationZ(&matRotZ, D3DXToRadian((*iter)->vAngle.z + fRollDelta));
			D3DXMatrixTranslation(&matTrans, (*iter)->vPos.x, (*iter)->vPos.y, (*iter)->vPos.z);
			matWorld = (matScale * matRotZ * matRotX * matBill *  matTrans);
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

void CJumpSmash::DefaultSetting( void )
{
	switch (m_iType)
	{
	// jump smash addition ground 07
	case 0:	// »ÏÁ·¸ÕÁö
		m_pEffectKey = L"Component_Texture_Blunt";
		m_iCount = 4;
		m_iIndex = 0;

		m_fInterval = 0.018f;
		m_fDuration = 0.1f;
		break;

	case 1:	// ¸ð·¡1
		m_pEffectKey = L"Component_Texture_ShapeFire";
		m_iCount = 5;
		m_iIndex = 0;

		m_fInterval = 0.25f;
		m_fDuration = 0.3f;
		break;

	case 2:	// ¹Ù´Ú2
		m_pEffectKey = L"Component_Texture_PartiSpray";
		m_iCount = 5;
		m_iIndex = 0;

		m_fInterval = 0.5f;
		m_fDuration = 4.5f;
		break;

	case 3:	// ¹Ù´Ú3
		m_pEffectKey = L"Component_Texture_IceLight";
		m_iCount = 5;
		m_iIndex = 1;

		m_fInterval = 0.3f;
		m_fDuration = 4.5f;
		break;

	case 4:	// ¹Ù´Ú6
		m_pEffectKey = L"Component_Texture_LightGlow";
		m_iCount = 5;
		m_iIndex = 3;

		m_fInterval = 0.8f;
		m_fDuration = 4.5f;
		break;

	case 5:	// ¸ð·¡ ¿À¶ó 2
		m_pEffectKey = L"Component_Texture_IceLight";
		m_iCount = 5;
		m_iIndex = 1;

		m_fInterval = 0.03f;
		m_fDuration = 0.2f;
		break;
	}
}

void CJumpSmash::AddParticle( const _int& iIndex )
{
	EFFECT* pEffect= new EFFECT;
	ZeroMemory(pEffect, sizeof(EFFECT));

	pEffect->bAlive = true;
	pEffect->vScale = MyVec3(1.f, 1.f, 1.f);
	D3DXVECTOR3 vMin(255.f, 155.f, 115.f);
	D3DXVECTOR3 vMax(255.f, 185.f, 155.f);
	Engine::GetRandomVector(&pEffect->vColor, &vMin, &vMax);

	_float fSize = 0.f;

	switch (m_iType)
	{
	case 0:
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(0.f, 10.f), Engine::GetRandomFloat(-345.f, 345.f), 0.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.3f, 0.3f);
		pEffect->fSpeed = Engine::GetRandomFloat(1.f, 3.f);
		pEffect->fAngle = 5.f;
		pEffect->fRadius = Engine::GetRandomFloat(15.f, 55.f);
		pEffect->fRadiusAngle = 120.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		pEffect->vScale = MyVec3(Engine::GetRandomFloat(10.f, 12.f), Engine::GetRandomFloat(25.f, 35.f), 1.f);
		pEffect->vScaleDelta = MyVec3(-20.f, 25.f, 0.f);
		pEffect->vPos =  m_vOrigin + m_vLook * (10.f + m_fDurationAcc * 200.f) + MyVec3(0.f, -10.f, 0.f);

		pEffect->fSizeDelta = 15.f;
		pEffect->fSizeWayTime = 0.4f;
		pEffect->fSizeWayDelta = 10.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.5f;
		pEffect->fAlphaWayDelta = 220.f;

		break;

	case 1:
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(0.f, 90.f), 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.4f, 0.6f);
		pEffect->fSpeed = Engine::GetRandomFloat(10.f, 15.f);
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(10.f, 35.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(11.f, 15.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = m_vOrigin + m_vLook * (10.f + (_float)iIndex * 6.f);

		pEffect->fSizeDelta = 6.f;
		pEffect->fSizeWayTime = 0.3f;
		pEffect->fSizeWayDelta = 5.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.3f;
		pEffect->fAlphaWayDelta = 250.f;

		pEffect->fRollDelta = 10.f;
		break;

	case 2:
		pEffect->vAngle = MyVec3(90.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(1.f, 1.4f);
		pEffect->fSpeed = Engine::GetRandomFloat(5.f, 15.f);
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(10.f, 40.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(15.f, 21.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = m_vOrigin + m_vLook * (10.f + (_float)iIndex * 6.f);

		pEffect->fSizeDelta = 2.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.3f;
		pEffect->fAlphaWayDelta = 230.f;

		pEffect->fRollDelta = 10.f;
		break;

	case 3:
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(0.f, 10.f), 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.9f, 1.2f);
		pEffect->fSpeed = Engine::GetRandomFloat(5.f, 15.f);
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(10.f, 50.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(12.f, 18.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = m_vOrigin + m_vLook * (10.f + (_float)iIndex * 6.f);

		pEffect->fSizeDelta = -3.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.2f;
		pEffect->fAlphaWayDelta = 200.f;

		pEffect->fRollDelta = 5.f;
		break;

	case 4:
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(80.f, 90.f), 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(1.f, 1.4f);
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = 5.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(15.f, 25.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = m_vOrigin + m_vLook * (10.f + (_float)iIndex * 6.f);

		pEffect->fSizeDelta = 60.f;
		pEffect->fSizeWayTime = 0.01f;
		pEffect->fSizeWayDelta = 50.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.3f;
		pEffect->fAlphaWayDelta = 150.f;

		pEffect->fRollDelta = 5.f;
		break;

	case 5:
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(0.f, 10.f), 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.7f, 0.9f);
		pEffect->fSpeed = Engine::GetRandomFloat(2.f, 7.f);
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(10.f, 40.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(8.f, 12.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = m_vOrigin + m_vLook * (10.f + (_float)iIndex * 6.f);

		pEffect->fSizeDelta = 3.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.1f;
		pEffect->fAlphaWayDelta = 230.f;

		pEffect->fRollDelta = 5.f;
		break;
	}

	D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
	pEffect->fAlphaDelta -= 255.f;
	
	m_EffectList.push_back(pEffect);
}
	
HRESULT CJumpSmash::Ready_Component( void )
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

void CJumpSmash::Free( void )
{
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pTextureCom);
	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pShaderCom);

	Engine::CGameObject::Free();
}

CJumpSmash* CJumpSmash::Create( LPDIRECT3DDEVICE9 pDevice, const _uint& iType)
{
	CJumpSmash* pJumpSmash = new CJumpSmash(pDevice);

	if (FAILED(pJumpSmash->Initialize(iType)))
	{
		MSG_BOX("JumpSmash Created Failed");
		Engine::Safe_Release(pJumpSmash);
	}
	return pJumpSmash;
}



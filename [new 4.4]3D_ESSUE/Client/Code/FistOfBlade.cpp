#include "StdAfx.h"
#include "FistOfBlade.h"

CFistOfBlade::CFistOfBlade( LPDIRECT3DDEVICE9 pDevice)
: CEffect(pDevice)
{
	
}

CFistOfBlade::~CFistOfBlade( void )
{

}

HRESULT CFistOfBlade::Initialize( const _uint& iType)
{
	m_iType = iType;
	m_pBone = ((Engine::CDynamicMesh*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(0)->GetComponent(L"Com_Mesh"))->GetPartsMatrix("ValveBiped_Bip01_R_Toe0");
	m_pTarget = ((Engine::CTransform*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(0)->GetComponent(L"Com_Transform"))->GetWorldMatrix();
	m_vOrigin = *(MyVec3*)&(*m_pBone * *m_pTarget).m[3][0];
	m_vAngle = *((Engine::CTransform*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(0)->GetComponent(L"Com_Transform"))->GetInfomation(Engine::CTransform::INFO_ANGLE);
	m_vAngle.y = D3DXToDegree(m_vAngle.y);

	DefaultSetting();

	if (FAILED(Ready_Component()))
		return E_FAIL;

	for (_uint i = 0; i < m_iCount; ++i)
		AddParticle();

	return S_OK;
}

_int CFistOfBlade::Update( const _float& fDelta )
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

void CFistOfBlade::Render( void )
{
	LPD3DXEFFECT pEffect = m_pShaderCom->GetEffect();
	if (pEffect == NULL)
		return;

	SetUpShader(pEffect);
}

void CFistOfBlade::SetUpShader( LPD3DXEFFECT pEffect )
{
	pEffect->AddRef();

	MyMat matView, matProj;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	m_pTextureCom->SetUpShader(pEffect, m_iIndex);
	
	pEffect->Begin(NULL, 0);
	if (m_iType % 2 == 0)
		pEffect->BeginPass(5);
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
			D3DXMatrixRotationY(&matRotY, D3DXToRadian((*iter)->vAngle.y + fRollDelta));
			D3DXMatrixRotationZ(&matRotZ, D3DXToRadian((*iter)->vAngle.z));
			D3DXMatrixTranslation(&matTrans, (*iter)->vPos.x, (*iter)->vPos.y, (*iter)->vPos.z);
			matWorld = matScale * matRotX * matRotY * matRotZ * matTrans;
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

void CFistOfBlade::DefaultSetting( void )
{
	switch (m_iType)
	{
	case 0: // »ÏÁ·±¼Àý 1
		m_pEffectKey = L"Component_Texture_Ring_Outer_Wind_n";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.03f;
		m_fDuration = 0.1f;
		break;

	case 1: // »ÏÁ·±¼Àý 1
		m_pEffectKey = L"Component_Texture_WindBladeSmall";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.03f;
		m_fDuration = 0.1f;
		break;

	case 2: // »ÏÁ·±¼Àý 1
		m_pEffectKey = L"Component_Texture_Ring_Outer_Wind_n";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.03f;
		m_fDuration = 0.1f;
		break;

	case 3: // »ÏÁ·±¼Àý 1
		m_pEffectKey = L"Component_Texture_WindBladeSmall";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.03f;
		m_fDuration = 0.1f;
		break;

	case 4: // »ÏÁ·±¼Àý 1
		m_pEffectKey = L"Component_Texture_Ring_Outer_Wind_n";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.03f;
		m_fDuration = 0.1f;
		break;

	case 5: // »ÏÁ·±¼Àý 1
		m_pEffectKey = L"Component_Texture_WindBladeSmall";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.03f;
		m_fDuration = 0.1f;
		break;

	case 6: // »ÏÁ·±¼Àý 1
		m_pEffectKey = L"Component_Texture_Ring_Outer_Wind_n";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.03f;
		m_fDuration = 0.1f;
		break;

	case 7: // »ÏÁ·±¼Àý 1
		m_pEffectKey = L"Component_Texture_WindBladeSmall";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.03f;
		m_fDuration = 0.1f;
		break;

	case 8: // »ÏÁ·±¼Àý 1
		m_pEffectKey = L"Component_Texture_Ring_Outer_Wind_n";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.03f;
		m_fDuration = 0.1f;
		break;

	case 9: // »ÏÁ·±¼Àý 1
		m_pEffectKey = L"Component_Texture_WindBladeSmall";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.03f;
		m_fDuration = 0.1f;
		break;

	case 10: // »ÏÁ·±¼Àý 1
		m_pEffectKey = L"Component_Texture_Ring_Outer_Wind_n";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.03f;
		m_fDuration = 0.1f;
		break;

	case 11: // »ÏÁ·±¼Àý 1
		m_pEffectKey = L"Component_Texture_WindBladeSmall";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.03f;
		m_fDuration = 0.1f;
		break;
	}
}

void CFistOfBlade::AddParticle( void )
{
	EFFECT* pEffect= new EFFECT;
	ZeroMemory(pEffect, sizeof(EFFECT));

	pEffect->bAlive = true;
	pEffect->vScale = MyVec3(1.f, 1.f, 1.f);
	D3DXVECTOR3 vMin(135.f, 165.f, 255.f);
	D3DXVECTOR3 vMax(175.f, 195.f, 255.f);
	Engine::GetRandomVector(&pEffect->vColor, &vMin, &vMax);

	MyMat matCompute = *m_pBone * *m_pTarget;
	MyVec3 vPos = *(MyVec3*)&matCompute.m[3][0];

	_float fSize = 0.f;

	switch (m_iType)
	{
	case 0:
		pEffect->vAngle = MyVec3(90.f, 90.f, 0.f);
		pEffect->vColor = MyVec3(117.f, 108.f, 91.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.2f, 0.25f);
		pEffect->fSpeed = 0.1f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = 0.f;
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(30.f, 35.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos + MyVec3(0.f, 4.f, 0.f);

		pEffect->fSizeDelta = 25.f;
		
		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.6f;
		pEffect->fAlphaWayDelta = 255.f;

		pEffect->fRollDelta = -260.f;
		break;

	case 1:
		pEffect->vAngle = MyVec3(90.f, 135.f, 0.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.2f, 0.25f);
		pEffect->fSpeed = 0.1f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = 0.f;
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(30.f, 35.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos + MyVec3(0.f, 4.f, 0.f);

		pEffect->fSizeDelta = 25.f;

		pEffect->fAlphaDelta = 0.f;

		pEffect->fRollDelta = -260.f;
		break;

	case 2:
		pEffect->vAngle = MyVec3(90.f, 285.f, 40.f);
		pEffect->vColor = MyVec3(117.f, 108.f, 91.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.2f, 0.25f);
		pEffect->fSpeed = 0.1f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = 0.f;
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(30.f, 35.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos + MyVec3(0.f, 4.f, 0.f);

		pEffect->fSizeDelta = 25.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.6f;
		pEffect->fAlphaWayDelta = 255.f;

		pEffect->fRollDelta = 260.f;
		break;

	case 3:
		pEffect->vAngle = MyVec3(90.f, 195.f, 40.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.2f, 0.25f);
		pEffect->fSpeed = 0.1f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = 0.f;
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(30.f, 35.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos + MyVec3(0.f, 4.f, 0.f);

		pEffect->fSizeDelta = 25.f;

		pEffect->fAlphaDelta = 0.f;

		pEffect->fRollDelta = 260.f;
		break;

	case 4:
		pEffect->vAngle = MyVec3(90.f, 135.f, -20.f);
		pEffect->vColor = MyVec3(117.f, 108.f, 91.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.2f, 0.25f);
		pEffect->fSpeed = 0.1f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = 0.f;
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(30.f, 35.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos + MyVec3(0.f, 4.f, 0.f);

		pEffect->fSizeDelta = 25.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.6f;
		pEffect->fAlphaWayDelta = 255.f;

		pEffect->fRollDelta = -260.f;
		break;

	case 5:
		pEffect->vAngle = MyVec3(90.f, 90.f, -20.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.2f, 0.25f);
		pEffect->fSpeed = 0.1f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = 0.f;
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(30.f, 35.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos + MyVec3(0.f, 4.f, 0.f);

		pEffect->fSizeDelta = 25.f;

		pEffect->fAlphaDelta = 0.f;

		pEffect->fRollDelta = -260.f;
		break;

	case 6:
		pEffect->vAngle = MyVec3(90.f, -90.f, 10.f);
		pEffect->vColor = MyVec3(117.f, 108.f, 91.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.2f, 0.25f);
		pEffect->fSpeed = 0.1f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = 0.f;
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(30.f, 35.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos + MyVec3(0.f, 4.f, 0.f);

		pEffect->fSizeDelta = 25.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.6f;
		pEffect->fAlphaWayDelta = 255.f;

		pEffect->fRollDelta = 260.f;
		break;

	case 7:
		pEffect->vAngle = MyVec3(90.f, -135.f, 10.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.2f, 0.25f);
		pEffect->fSpeed = 0.1f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = 0.f;
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(30.f, 35.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos + MyVec3(0.f, 4.f, 0.f);

		pEffect->fSizeDelta = 25.f;

		pEffect->fAlphaDelta = 0.f;

		pEffect->fRollDelta = 260.f;
		break;

	case 8:
		pEffect->vAngle = MyVec3(90.f, 270.f, 80.f);
		pEffect->vColor = MyVec3(117.f, 108.f, 91.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.2f, 0.25f);
		pEffect->fSpeed = 0.1f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = 0.f;
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(30.f, 35.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos + MyVec3(0.f, 4.f, 0.f);

		pEffect->fSizeDelta = 25.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.6f;
		pEffect->fAlphaWayDelta = 255.f;

		pEffect->fRollDelta = 260.f;
		break;

	case 9:
		pEffect->vAngle = MyVec3(90.f, 180.f, 80.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.2f, 0.25f);
		pEffect->fSpeed = 0.1f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = 0.f;
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(30.f, 35.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos + MyVec3(0.f, 4.f, 0.f);

		pEffect->fSizeDelta = 25.f;

		pEffect->fAlphaDelta = 0.f;

		pEffect->fRollDelta = 260.f;
		break;

	case 10:
		pEffect->vAngle = MyVec3(90.f, 180.f, 5.f);
		pEffect->vColor = MyVec3(117.f, 108.f, 91.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.2f, 0.25f);
		pEffect->fSpeed = 0.1f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = 0.f;
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(30.f, 35.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos + MyVec3(0.f, 4.f, 0.f);

		pEffect->fSizeDelta = 25.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.6f;
		pEffect->fAlphaWayDelta = 255.f;

		pEffect->fRollDelta = 260.f;
		break;

	case 11:
		pEffect->vAngle = MyVec3(90.f, 135.f, 5.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.2f, 0.25f);
		pEffect->fSpeed = 0.1f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = 0.f;
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(30.f, 35.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos + MyVec3(0.f, 4.f, 0.f);

		pEffect->fSizeDelta = 25.f;

		pEffect->fAlphaDelta = 0.f;

		pEffect->fRollDelta = 260.f;
		break;
	}

	pEffect->vAngle += m_vAngle;
	pEffect->fAlphaDelta -= 255.f;
	m_EffectList.push_back(pEffect);
}


HRESULT CFistOfBlade::Ready_Component( void )
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

void CFistOfBlade::Free( void )
{
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pTextureCom);
	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pShaderCom);

	Engine::CGameObject::Free();
}

CFistOfBlade* CFistOfBlade::Create( LPDIRECT3DDEVICE9 pDevice, const _uint& iType)
{
	CFistOfBlade* pFistOfBlade = new CFistOfBlade(pDevice);

	if (FAILED(pFistOfBlade->Initialize(iType)))
	{
		MSG_BOX("FistOfBlade Created Failed");
		Engine::Safe_Release(pFistOfBlade);
	}
	return pFistOfBlade;
}



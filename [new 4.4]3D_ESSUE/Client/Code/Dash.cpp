#include "StdAfx.h"
#include "Dash.h"
#include "DefaultCamera.h"

CDash::CDash( LPDIRECT3DDEVICE9 pDevice)
: CEffect(pDevice)
{
	
}

CDash::~CDash( void )
{

}

HRESULT CDash::Initialize( const _uint& iType)
{
	m_iType = iType;
	if (m_iType == 4 || m_iType == 5 || m_iType == 6)
		m_pBone = ((Engine::CDynamicMesh*)Engine::Get_Management()->GetLayer(L"Layer_Boss")->GetObject(0)->GetComponent(L"Com_Mesh"))->GetPartsMatrix("ValveBiped_Bip01_R_Toe0");
	else if (m_iType == 10 || m_iType == 11)
		m_pBone = ((Engine::CDynamicMesh*)Engine::Get_Management()->GetLayer(L"Layer_Boss")->GetObject(0)->GetComponent(L"Com_Mesh"))->GetPartsMatrix("ValveBiped_Bip01_R_Foot");
	else
		m_pBone = ((Engine::CDynamicMesh*)Engine::Get_Management()->GetLayer(L"Layer_Boss")->GetObject(0)->GetComponent(L"Com_Mesh"))->GetPartsMatrix("ValveBiped_Bip01");
	m_pTarget = ((Engine::CTransform*)Engine::Get_Management()->GetLayer(L"Layer_Boss")->GetObject(0)->GetComponent(L"Com_Transform"))->GetWorldMatrix();

	m_vOrigin = *(MyVec3*)&(*m_pBone * *m_pTarget).m[3][0];


	DefaultSetting();

	if (FAILED(Ready_Component()))
		return E_FAIL;

	for (_uint i = 0; i < m_iCount; ++i)
		AddParticle();

	return S_OK;
}

_int CDash::Update( const _float& fDelta )
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

void CDash::Render( void )
{
	LPD3DXEFFECT pEffect = m_pShaderCom->GetEffect();
	if (pEffect == NULL)
		return;

	SetUpShader(pEffect);
}

void CDash::SetUpShader( LPD3DXEFFECT pEffect )
{
	pEffect->AddRef();

	MyMat matView, matProj;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	m_pTextureCom->SetUpShader(pEffect, m_iIndex);
	
	pEffect->Begin(NULL, 0);
	pEffect->BeginPass(2);

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

void CDash::DefaultSetting( void )
{
	switch (m_iType)
	{
	case 0: // 뾰족먼지1
		m_pEffectKey = L"Component_Texture_ShapeFire";
		m_iCount = 2;
		m_iIndex = 2;

		m_fInterval = 0.01f;
		m_fDuration = 0.4f;
		break;

	case 1: // 뾰족먼지1
		m_pEffectKey = L"Component_Texture_YellowFlare";
		m_iCount = 1;
		m_iIndex = 4;

		m_fInterval = 0.02f;
		m_fDuration = 0.15f;
		break;

	case 2: // 뾰족먼지2 ??
		m_pEffectKey = L"Component_Texture_DustWhite";
		m_iCount = (_int)Engine::GetRandomFloat(1.f, 2.f);
		m_iIndex = 1;

		m_fInterval = 0.02f;
		m_fDuration = 0.35f;
		break;

	case 3: // 손먼지 ??
		m_pEffectKey = L"Component_Texture_YellowFlare";
		m_iCount = 1;
		m_iIndex = 2;
		break;

	case 4: // 링1
		m_pEffectKey = L"Component_Texture_YellowFlare";
		m_iCount = 3;
		m_iIndex = 2;

		m_fInterval = 0.01f;
		m_fDuration = 0.35f;
		break;

	case 5: // 링2
		m_pEffectKey = L"Component_Texture_Ring";
		m_iCount = 3;
		m_iIndex = 1;

		m_fInterval = 0.01f;
		m_fDuration = 0.08f;
		break;

	case 6: // 링3
		m_pEffectKey = L"Component_Texture_RingFire";
		m_iCount = 3;
		m_iIndex = 1;

		m_fInterval = 0.01f;
		m_fDuration = 0.2f;
		break;

	case 7: // 링굴절
		m_pEffectKey = L"Component_Texture_RingFire";
		m_iCount = 1;
		m_iIndex = 1;

		m_fInterval = 0.01f;
		m_fDuration = 0.2f;
		break;

	case 8: // 뾰족굴절
		m_pEffectKey = L"Component_Texture_RingFire";
		m_iCount = 1;
		m_iIndex = 1;

		m_fInterval = 0.01f;
		m_fDuration = 0.2f;
		break;

	case 10: // 퍼지는먼지
		m_pEffectKey = L"Component_Texture_DustWhite";
		m_iCount = 3;
		m_iIndex = 1;

		m_fInterval = 0.04f;
		m_fDuration = 0.2f;
		break;

	case 11: // 돌가루
		m_pEffectKey = L"Component_Texture_Ice";
		m_iCount = 1;
		m_iIndex = 2;

		m_fInterval = 0.06f;
		m_fDuration = 0.15f;
		break;
	}
}

void CDash::AddParticle( void )
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
		pEffect->vAngle = MyVec3(90.f, 0.f, 0.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.25f, 0.3f);
		pEffect->fSpeed = Engine::GetRandomFloat(9.f, 12.f);
		pEffect->vScale = MyVec3(Engine::GetRandomFloat(2.f, 3.f), Engine::GetRandomFloat(35.f, 45.f), 1.f);
		pEffect->vScaleDelta = MyVec3(0.f, -25.f, 0.f);
		pEffect->vPos = vPos;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.3f;
		pEffect->fAlphaWayDelta = 250.f;
		break;

	case 1:
		pEffect->vAngle = MyVec3(90.f, 0.f, 0.f);
		pEffect->vColor = MyVec3(255.f, 255.f, 255.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.15f, 0.2f);
		pEffect->fSpeed = Engine::GetRandomFloat(9.f, 12.f);
		pEffect->vScale = MyVec3(Engine::GetRandomFloat(2.f, 3.f), Engine::GetRandomFloat(35.f, 45.f), 1.f);
		pEffect->vScaleDelta = MyVec3(0.f, -5.f, 0.f);
		pEffect->vPos = vPos;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.3f;
		pEffect->fAlphaWayDelta = 110.f;
		break;

	case 2:
		pEffect->vAngle = MyVec3(90.f, 0.f, 0.f);
		pEffect->vColor = MyVec3(255.f, 255.f, 255.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.5f, 0.7f);
		pEffect->fSpeed = Engine::GetRandomFloat(9.f, 12.f);
		pEffect->vScale = MyVec3(Engine::GetRandomFloat(3.f, 6.f), Engine::GetRandomFloat(7.f, 15.f), 1.f);
		pEffect->vScaleDelta = MyVec3(0.f, 15.f, 0.f);
		pEffect->vPos = vPos;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.3f;
		pEffect->fAlphaWayDelta = 20.f;
		break;

	case 3:
		break;

	case 4:
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(-10.f, 10.f), Engine::GetRandomFloat(-10.f, 10.f), Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 255.f, 255.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.2f, 0.3f);
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = 25.f;
		pEffect->fRadius = 0.f;
		pEffect->fRadiusAngle = 0.f;
		fSize = Engine::GetRandomFloat(15.f, 25.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos + MyVec3(0.f, 2.f, 0.f) + vLook * 15.f;

		pEffect->fSizeDelta = 10.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.1f;
		pEffect->fAlphaWayDelta = 100.f;

		pEffect->fRollDelta = 190.f;
		break;

	case 5:
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(-10.f, 10.f), Engine::GetRandomFloat(-10.f, 10.f), Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 255.f, 255.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.15f, 0.2f);
		pEffect->fSpeed = Engine::GetRandomFloat(20.f, 40.f);
		pEffect->fAngle = 5.f;
		pEffect->fRadius = 0.f;
		pEffect->fRadiusAngle = 0.f;
		fSize = Engine::GetRandomFloat(7.f, 7.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos + MyVec3(0.f, 2.f, 0.f) + vLook * 15.f;

		pEffect->fSizeDelta = 15.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.01f;
		pEffect->fAlphaWayDelta = 85.f;
		break;

	case 6:
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(-10.f, 10.f), Engine::GetRandomFloat(-10.f, 10.f), Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 255.f, 255.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.2f, 0.3f);
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = 45.f;
		pEffect->fRadius = 0.f;
		pEffect->fRadiusAngle = 0.f;
		fSize = Engine::GetRandomFloat(8.f, 13.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos + MyVec3(0.f, 2.f, 0.f) + vLook * 15.f;

		pEffect->fSizeDelta = 15.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.01f;
		pEffect->fAlphaWayDelta = 100.f;

		pEffect->fRollDelta = 80.f;
		break;

	case 7:
		break;

	case 8:
		break;

	case 10:
		pEffect->vAngle = MyVec3(70.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		//pEffect->vColor = MyVec3(130.f, 150.f, 230.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(1.f, 2.f);
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(40.f, 110.f);
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.x = pEffect->fRadius * cosf(pEffect->fAngle);
		pEffect->vDir.y = Engine::GetRandomFloat(5.f, 10.f);
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(5.f, 8.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 22.f;
		pEffect->fSizeWayTime = 0.1f;
		pEffect->fSizeWayDelta = 15.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.5f;
		pEffect->fAlphaWayDelta = 180.f;

		pEffect->fRollDelta = 60.f;
		pEffect->fRollWayTime = 0.4f;
		pEffect->fRollWayDelta = 40.f;
		break;

	case 11:
		pEffect->vAngle = MyVec3(70.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(255.f, 155.f, 125.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(1.f, 1.3f);
		pEffect->fSpeed = Engine::GetRandomFloat(8.f, 18.f);
		pEffect->fAngle = 30.f;
		pEffect->fRadius = Engine::GetRandomFloat(60.f, 150.f);
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.x = pEffect->fRadius * cosf(pEffect->fAngle);
		pEffect->vDir.y = Engine::GetRandomFloat(5.f, 10.f);
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(2.f, 4.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 3.f;
		pEffect->fSizeWayTime = 0.3f;
		pEffect->fSizeWayDelta = 3.f;

		pEffect->fAlphaDelta = 0.f;

		pEffect->fRollDelta = 265.f;
		break;
	}


	D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
	pEffect->fAlphaDelta -= 255.f;
	
	m_EffectList.push_back(pEffect);
}

HRESULT CDash::Ready_Component( void )
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

void CDash::Free( void )
{
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pTextureCom);
	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pShaderCom);

	Engine::CGameObject::Free();
}

CDash* CDash::Create( LPDIRECT3DDEVICE9 pDevice, const _uint& iType)
{
	CDash* pDash = new CDash(pDevice);

	if (FAILED(pDash->Initialize(iType)))
	{
		MSG_BOX("Dash Created Failed");
		Engine::Safe_Release(pDash);
	}
	return pDash;
}



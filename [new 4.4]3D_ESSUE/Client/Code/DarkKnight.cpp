#include "StdAfx.h"
#include "DarkKnight.h"
#include "DefaultCamera.h"

CDarkKnight::CDarkKnight( LPDIRECT3DDEVICE9 pDevice)
: CEffect(pDevice)
, m_fRadius(0.f)
, m_fRadiusDelta(0.f)
, m_iCountDelta(0)
{
	
}

CDarkKnight::~CDarkKnight( void )
{

}

HRESULT CDarkKnight::Initialize( const _uint& iType)
{
	m_iType = iType;
	m_pBone = ((Engine::CDynamicMesh*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(0)->GetComponent(L"Com_Mesh"))->GetPartsMatrix("ValveBiped_Anim_Attachment_LF");
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

_int CDarkKnight::Update( const _float& fDelta )
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

void CDarkKnight::Render( void )
{
	LPD3DXEFFECT pEffect = m_pShaderCom->GetEffect();
	if (pEffect == NULL)
		return;

	SetUpShader(pEffect);
}

void CDarkKnight::SetUpShader( LPD3DXEFFECT pEffect )
{
	pEffect->AddRef();

	MyMat matView, matProj;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	m_pTextureCom->SetUpShader(pEffect, m_iIndex);
	
	pEffect->Begin(NULL, 0);
	pEffect->BeginPass(6);

	EFFECTLIST::iterator iter = m_EffectList.begin();
	EFFECTLIST::iterator iter_end = m_EffectList.end();
	MyMat matScale, matRotX, matRotY, matRotZ, matTrans, matWorld;

	/*MyMat matBill;
	D3DXMatrixIdentity(&matBill);
	matBill._11 = matView._11;
	matBill._13 = matView._13;
	matBill._31 = matView._31;
	matBill._33 = matView._33;
	D3DXMatrixInverse(&matBill, NULL, &matBill);*/

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

void CDarkKnight::DefaultSetting( void )
{
	switch (m_iType)
	{
	// DarkKnight Floor
	case 0: // 액체1
		m_pEffectKey = L"Component_Texture_Mercury_Sneezn";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.01f;
		m_fDuration = 6.f;
		break;

	case 1: // 액체1
		m_pEffectKey = L"Component_Texture_Mercury_Sneezn";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.01f;
		m_fDuration = 6.f;
		break;

	// LordOfMana Body
	case 5: // 액체1
		m_pEffectKey = L"Component_Texture_Mercury_Sneezn";
		m_iCount = 1;
		m_iIndex = 2;

		m_fInterval = 0.05f;
		m_fDuration = 9.f;
		break;

	case 6: // 액체2
		m_pEffectKey = L"Component_Texture_Mercury_Sneezn";
		m_iCount = 1;
		m_iIndex = 1;

		m_fInterval = 0.05f;
		m_fDuration = 9.f;
		break;

	case 7: // 액체3
		m_pEffectKey = L"Component_Texture_Mercury_Sneezn";
		m_iCount = 1;
		m_iIndex = 1;

		m_fInterval = 0.02f;
		m_fDuration = 9.f;
		break;

	case 8: // 검은연기
		m_pEffectKey = L"Component_Texture_Dust2";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.09f;
		m_fDuration = 9.f;

		m_iCountDelta = -7;
		break;

	// shakebomb
	case 10: // 검은링
		m_pEffectKey = L"Component_Texture_RingFireDark";
		m_iCount = 2;
		m_iIndex = 0;

		m_fInterval = 0.03f;
		m_fDuration = 0.2f;
		break;

	case 11: // 검은가루
		m_pEffectKey = L"Component_Texture_RingFireDark";
		m_iCount = 12;
		m_iIndex = 0;

		m_fInterval = 0.02f;
		m_fDuration = 0.1f;
		break;

	case 12: // 반짝이는빛
		m_pEffectKey = L"Component_Texture_Dust";
		m_iCount = 8;
		m_iIndex = 0;

		m_fInterval = 0.6f;
		m_fDuration = 0.5f;
		break;
	}
}

void CDarkKnight::AddParticle( void )
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
		pEffect->vColor = MyVec3(255.f, 255.f, 255.f);
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.5f, 0.7f);
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = 25.f;
		pEffect->fRadius = Engine::GetRandomFloat(10.f, 30.f);
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle);
		pEffect->vDir.x = pEffect->fRadius * cosf(pEffect->fAngle);
		pEffect->vDir.y = 1.f;
		fSize = Engine::GetRandomFloat(2.f, 4.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = *(MyVec3*)&m_pTarget->m[3][0];

		pEffect->fSizeDelta = -1.f;
		pEffect->fSizeWayTime = 0.5f;
		pEffect->fSizeWayDelta = 1.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.6f;
		pEffect->fAlphaWayDelta = 190.f;

		pEffect->fRollDelta = 100.f;
		pEffect->fRollWayTime = 0.5f;
		pEffect->fRollWayDelta = 80.f;

		pEffect->bAlive = true;
		break;

	case 1:
		pEffect->vColor = MyVec3(255.f, 255.f, 255.f);
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.5f, 0.7f);
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = 25.f;
		pEffect->fRadius = Engine::GetRandomFloat(10.f, 30.f);
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(4.f, 7.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vScaleDelta = MyVec3(1.f, 2.f, 0.f);
		pEffect->vPos = *(MyVec3*)&m_pTarget->m[3][0];

		pEffect->fSizeDelta = -1.f;
		pEffect->fSizeWayTime = 0.5f;
		pEffect->fSizeWayDelta = 1.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.6f;
		pEffect->fAlphaWayDelta = 190.f;

		pEffect->fRollDelta = 20.f;
		pEffect->fRollWayTime = 0.5f;
		pEffect->fRollWayDelta = 10.f;

		pEffect->bAlive = true;
		break;

	case 5:
		pEffect->vColor = MyVec3(255.f, 255.f, 255.f);
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(1.f, 2.f);
		pEffect->fSpeed = 0.f;
		pEffect->fAngle = 360.f;
		pEffect->fRadius = 0.f;
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(1.f, 3.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 1.f;
		pEffect->fSizeWayTime = 0.5f;
		pEffect->fSizeWayDelta = 5.f;

		pEffect->fAlphaDelta = 5.f;
		pEffect->fAlphaWayTime = 0.5f;
		pEffect->fAlphaWayDelta = 155.f;

		pEffect->fRollDelta = 100.f;
		pEffect->fRollWayTime = 0.5f;
		pEffect->fRollWayDelta = 80.f;

		pEffect->bAlive = true;
		break;

	case 6:
		pEffect->vColor = MyVec3(255.f, 255.f, 255.f);
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(2.f, 3.f);
		pEffect->fSpeed = 0.f;
		pEffect->fAngle = 360.f;
		pEffect->fRadius = 0.f;
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(1.f, 3.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 2.f;
		pEffect->fSizeWayTime = 0.5f;
		pEffect->fSizeWayDelta = 6.f;

		pEffect->fAlphaDelta = 5.f;
		pEffect->fAlphaWayTime = 0.5f;
		pEffect->fAlphaWayDelta = 155.f;

		pEffect->fRollDelta = 100.f;
		pEffect->fRollWayTime = 0.5f;
		pEffect->fRollWayDelta = 80.f;

		pEffect->bAlive = true;
		break;

	case 7:
		pEffect->vColor = MyVec3(255.f, 255.f, 255.f);
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.7f, 1.f);
		pEffect->fSpeed = 0.f;
		pEffect->fAngle = 360.f;
		pEffect->fRadius = 0.f;
		pEffect->fRadiusAngle = 30.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(1.f, 2.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = -1.f;
		pEffect->fSizeWayTime = 0.5f;
		pEffect->fSizeWayDelta = 0.f;

		pEffect->fAlphaDelta = 5.f;
		pEffect->fAlphaWayTime = 0.5f;
		pEffect->fAlphaWayDelta = 155.f;

		pEffect->fRollDelta = 200.f;
		pEffect->fRollWayTime = 0.5f;
		pEffect->fRollWayDelta = 210.f;

		pEffect->bAlive = true;
		break;

	case 8:
		pEffect->vColor = MyVec3(255.f, 255.f, 255.f);
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.7f, 1.f);
		pEffect->fSpeed = Engine::GetRandomFloat(3.f, 5.f);
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(7.f, 9.f);
		pEffect->fRadiusAngle = Engine::GetRandomFloat(-360.f, 360.f);
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(1.f, 2.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 1.f;
		pEffect->fSizeWayTime = 0.01f;
		pEffect->fSizeWayDelta = 0.3f;

		pEffect->fAlphaDelta = 10.f;
		pEffect->fAlphaWayTime = 0.6f;
		pEffect->fAlphaWayDelta = 250.f;

		pEffect->fRollDelta = 60.f;
		pEffect->fRollWayTime = 0.5f;
		pEffect->fRollWayDelta = 40.f;

		pEffect->bAlive = true;
		break;

	case 10:
		pEffect->vColor = MyVec3(255.f, 227.f, 175.f);
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.2f, 0.3f);
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = 30.f;
		pEffect->fRadius = 20.f;
		pEffect->fRadiusAngle = 40.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(1.f, 2.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 79.f;
		pEffect->fSizeWayTime = 0.3f;
		pEffect->fSizeWayDelta = 57.f;

		pEffect->fAlphaDelta = 10.f;
		pEffect->fAlphaWayTime = 0.5f;
		pEffect->fAlphaWayDelta = 250.f;

		pEffect->fRollDelta = 100.f;
		pEffect->fRollWayTime = 0.01f;
		pEffect->fRollWayDelta = 50.f;

		pEffect->bAlive = true;
		break;

	case 11:
		pEffect->vColor = MyVec3(255.f, 121.f, 47.f);
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.5f, 0.7f);
		pEffect->fSpeed = 250.f;
		pEffect->fAngle = 92.f;
		pEffect->fRadius = Engine::GetRandomFloat(30.f, 50.f);
		pEffect->fRadiusAngle = Engine::GetRandomFloat(30.f, 50.f);
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(1.f, 2.f);
		pEffect->vScale = MyVec3(Engine::GetRandomFloat(0.5f, 1.f) + fSize, Engine::GetRandomFloat(4.f, 5.f) + fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 2.f;
		pEffect->fSizeWayTime = 0.01f;
		pEffect->fSizeWayDelta = 1.f;

		pEffect->fAlphaDelta = 2.f;
		pEffect->fAlphaWayTime = 0.5f;
		pEffect->fAlphaWayDelta = 250.f;

		pEffect->fRollDelta = 80.f;
		pEffect->fRollWayTime = 0.5f;
		pEffect->fRollWayDelta = 50.f;

		pEffect->bAlive = true;
		break;

	case 12:
		pEffect->vColor = MyVec3(255.f, 255.f, 255.f);
		pEffect->vAngle = MyVec3(Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = 0.5f;
		pEffect->fSpeed = Engine::GetRandomFloat(10.f, 50.f);
		pEffect->fAngle = 20.f;
		pEffect->fRadius = 20.f;
		pEffect->fRadiusAngle = 30.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		fSize = Engine::GetRandomFloat(15.f, 15.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 1.f;
		pEffect->fSizeWayTime = 0.2f;
		pEffect->fSizeWayDelta = 90.f;

		pEffect->fAlphaDelta = 20.f;
		pEffect->fAlphaWayTime = 0.7f;
		pEffect->fAlphaWayDelta = 150.f;

		pEffect->fRollDelta = 150.f;
		pEffect->fRollWayTime = 0.5f;
		pEffect->fRollWayDelta = 100.f;

		pEffect->bAlive = true;
		break;
	}

	D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
	pEffect->fAlphaDelta -= 255.f;
	m_EffectList.push_back(pEffect);
}

HRESULT CDarkKnight::Ready_Component( void )
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

void CDarkKnight::Free( void )
{
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pTextureCom);
	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pShaderCom);

	Engine::CGameObject::Free();
}

CDarkKnight* CDarkKnight::Create( LPDIRECT3DDEVICE9 pDevice, const _uint& iType)
{
	CDarkKnight* pDarkKnight = new CDarkKnight(pDevice);

	if (FAILED(pDarkKnight->Initialize(iType)))
	{
		MSG_BOX("DarkKnight Created Failed");
		Engine::Safe_Release(pDarkKnight);
	}
	return pDarkKnight;
}



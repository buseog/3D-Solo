#include "StdAfx.h"
#include "Teleport.h"
#include "DefaultCamera.h"
#include "LongBlade.h"

CTeleport::CTeleport( LPDIRECT3DDEVICE9 pDevice)
: CEffect(pDevice)
{
	
}

CTeleport::~CTeleport( void )
{

}

HRESULT CTeleport::Initialize( const _uint& iType)
{
	m_iType = iType;
	m_pBone = ((Engine::CDynamicMesh*)Engine::Get_Management()->GetLayer(L"Layer_Boss")->GetObject(0)->GetComponent(L"Com_Mesh"))->GetPartsMatrix("ValveBiped_Bip01");
	m_pTarget = ((Engine::CTransform*)Engine::Get_Management()->GetLayer(L"Layer_Boss")->GetObject(0)->GetComponent(L"Com_Transform"))->GetWorldMatrix();
	
	m_vOrigin = *(MyVec3*)&(*m_pBone * *m_pTarget).m[3][0];


	DefaultSetting();

	if (FAILED(Ready_Component()))
		return E_FAIL;

	for (_uint i = 0; i < m_iCount; ++i)
		AddParticle(i);

	return S_OK;
}

_int CTeleport::Update( const _float& fDelta )
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

void CTeleport::Render( void )
{
	LPD3DXEFFECT pEffect = m_pShaderCom->GetEffect();
	if (pEffect == NULL)
		return;

	SetUpShader(pEffect);
}

void CTeleport::SetUpShader( LPD3DXEFFECT pEffect )
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
			D3DXMatrixScaling(&matScale, (*iter)->vScale.x + fSizeDelta, (*iter)->vScale.y + fSizeDelta, (*iter)->vScale.z);
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

void CTeleport::DefaultSetting( void )
{
	switch (m_iType)
	{
	// Teleport01
	case 0:	// 퍼지는먼지
		m_pEffectKey = L"Component_Texture_Light";
		m_iCount = 2;
		m_iIndex = 0;

		m_fInterval = 0.6f;
		m_fDuration = 0.5f;
		break;

	case 1:	// 퍼지는먼지
		m_pEffectKey = L"Component_Texture_IceLight";
		m_iCount = 1;
		m_iIndex = 1;

		m_fInterval = 0.01f;
		m_fDuration = 0.15f;
		break;

	case 2:	// 퍼지는먼지
		m_pEffectKey = L"Component_Texture_ShapeFire";
		m_iCount = 1;
		m_iIndex = 1;

		m_fInterval = 0.03f;
		m_fDuration = 0.25f;
		break;

	// Teleport02
	case 10:	// 퍼지는먼지
		m_pEffectKey = L"Component_Texture_Light";
		m_iCount = 2;
		m_iIndex = 0;

		m_fInterval = 0.01f;
		m_fDuration = 0.15f;
		break;

	case 11:	// 퍼지는먼지
		m_pEffectKey = L"Component_Texture_ShapeFire";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.02f;
		m_fDuration = 0.3f;
		break;

	case 12:	// 퍼지는먼지
		m_pEffectKey = L"Component_Texture_IceLight";
		m_iCount = 1;
		m_iIndex = 1;

		m_fInterval = 0.01f;
		m_fDuration = 0.15f;
		break;

	case 13:	// 퍼지는먼지
		m_pEffectKey = L"Component_Texture_efxRipple";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.03f;
		m_fDuration = 0.5f;
		break;

	// Teleport03
	case 20:	// 퍼지는먼지
		m_pEffectKey = L"Component_Texture_ShapeFire";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.01f;
		m_fDuration = 0.15f;
		break;

	case 21:	// 퍼지는먼지
		m_pEffectKey = L"Component_Texture_IceLight";
		m_iCount = 1;
		m_iIndex = 1;

		m_fInterval = 0.01f;
		m_fDuration = 0.15f;
		break;

	case 22:	// 퍼지는먼지
		m_pEffectKey = L"Component_Texture_efxRipple";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.03f;
		m_fDuration = 0.5f;
		break;
	}
}

void CTeleport::AddParticle( const _int& iIndex )
{
	EFFECT* pEffect= new EFFECT;
	ZeroMemory(pEffect, sizeof(EFFECT));

	pEffect->bAlive = true;
	pEffect->vScale = MyVec3(1.f, 1.f, 1.f);
	D3DXVECTOR3 vMin(255.f, 155.f, 115.f);
	D3DXVECTOR3 vMax(255.f, 185.f, 155.f);
	Engine::GetRandomVector(&pEffect->vColor, &vMin, &vMax);

	MyMat matCompute = *m_pBone * *m_pTarget;
	MyVec3 vPos = *(MyVec3*)&matCompute.m[3][0];

	_float fSize = 0.f;

	switch (m_iType)
	{
	case 0:
		pEffect->vAngle = MyVec3(0.f, 0.f, Engine::GetRandomFloat(-345.f, 345.f));
		pEffect->vColor = MyVec3(255.f, 185.f, 155.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.7f, 1.f);
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = 0.f;
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(5.f, 10.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 30.f;
		pEffect->fSizeWayTime = 0.01f;
		pEffect->fSizeWayDelta = 5.0f;

		pEffect->fAlphaDelta = 10.f;

		pEffect->fRollDelta = 5.f;
		break;

	case 1:
		pEffect->vAngle = MyVec3(0.f, 0.f, Engine::GetRandomFloat(-345.f, 345.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.7f, 1.f);
		pEffect->fSpeed = Engine::GetRandomFloat(5.f, 15.f);
		pEffect->fAngle = Engine::GetRandomFloat(-360.f, 360.f);
		pEffect->fRadius = Engine::GetRandomFloat(20.f, 50.f);
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = Engine::GetRandomFloat(-50.f, 50.f);
		pEffect->vDir.x = pEffect->fRadius * cosf(pEffect->fAngle);
		pEffect->vDir.y = pEffect->fRadius * sinf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(5.f, 9.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 12.f;

		pEffect->fAlphaDelta = 10.f;

		pEffect->fRollDelta = 5.f;
		break;

	case 2:
		pEffect->vAngle = MyVec3(0.f, 0.f, Engine::GetRandomFloat(-345.f, 345.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.7f, 1.f);
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = Engine::GetRandomFloat(-360.f, 360.f);
		pEffect->fRadius = Engine::GetRandomFloat(10.f, 30.f);
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = Engine::GetRandomFloat(-20.f, 80.f);
		pEffect->vDir.x = pEffect->fRadius * cosf(pEffect->fAngle);
		pEffect->vDir.y = pEffect->fRadius * sinf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(3.f, 9.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 3.f;

		pEffect->fAlphaDelta = 10.f;

		pEffect->fRollDelta = 5.f;
		break;

	case 10:
		pEffect->vAngle = MyVec3(0.f, 0.f, Engine::GetRandomFloat(-345.f, 345.f));
		pEffect->vColor = MyVec3(255.f, 185.f, 155.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.7f, 1.f);
		pEffect->fSpeed = 1.f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = 0.f;
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(5.f, 10.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 10.f;
		pEffect->fSizeWayTime = 0.01f;
		pEffect->fSizeWayDelta = 5.0f;

		pEffect->fAlphaDelta = 10.f;

		pEffect->fRollDelta = 5.f;		
		break;

	case 11:
		pEffect->vAngle = MyVec3(0.f, 0.f, Engine::GetRandomFloat(-345.f, 345.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.7f, 1.f);
		pEffect->fSpeed = Engine::GetRandomFloat(5.f, 15.f);;
		pEffect->fAngle = Engine::GetRandomFloat(-360.f, 360.f);
		pEffect->fRadius = Engine::GetRandomFloat(20.f, 50.f);
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = Engine::GetRandomFloat(-50.f, 50.f);
		pEffect->vDir.x = pEffect->fRadius * cosf(pEffect->fAngle);
		pEffect->vDir.y = pEffect->fRadius * sinf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(6.f, 11.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 1.f;

		pEffect->fAlphaDelta = 0.f;

		pEffect->fRollDelta = 5.f;		
		break;

	case 12:
		pEffect->vAngle = MyVec3(0.f, 0.f, Engine::GetRandomFloat(-345.f, 345.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.7f, 1.f);
		pEffect->fSpeed = Engine::GetRandomFloat(0.f, 1.f);;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(5.f, 10.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * cosf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(5.f, 7.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 3.f;

		pEffect->fAlphaDelta = 0.f;

		pEffect->fRollDelta = 15.f;		
		break;

	case 13:
		pEffect->vAngle = MyVec3(0.f, 0.f, Engine::GetRandomFloat(-345.f, 345.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.7f, 1.f);
		pEffect->fSpeed = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(5.f, 10.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);;
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(5.f, 7.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 3.f;

		pEffect->fAlphaDelta = 0.f;

		pEffect->fRollDelta = 15.f;		
		break;

	case 20:
		pEffect->vAngle = MyVec3(0.f, 0.f, Engine::GetRandomFloat(-345.f, 345.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.7f, 1.f);
		pEffect->fSpeed = Engine::GetRandomFloat(5.f, 15.f);;
		pEffect->fAngle = Engine::GetRandomFloat(-360.f, 360.f);
		pEffect->fRadius = Engine::GetRandomFloat(20.f, 50.f);
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = Engine::GetRandomFloat(-50.f, 50.f);
		pEffect->vDir.x = pEffect->fRadius * cosf(pEffect->fAngle);
		pEffect->vDir.y = pEffect->fRadius * sinf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(6.f, 11.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 1.f;

		pEffect->fAlphaDelta = 0.f;

		pEffect->fRollDelta = 5.f;		
		break;

	case 21:
		pEffect->vAngle = MyVec3(0.f, 0.f, Engine::GetRandomFloat(-345.f, 345.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.7f, 1.f);
		pEffect->fSpeed = Engine::GetRandomFloat(0.f, 1.f);;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(5.f, 10.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);;
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(5.f, 7.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 3.f;

		pEffect->fAlphaDelta = 0.f;

		pEffect->fRollDelta = 15.f;
		break;

	case 22:
		pEffect->vAngle = MyVec3(0.f, 0.f, Engine::GetRandomFloat(-345.f, 345.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.7f, 1.f);
		pEffect->fSpeed = Engine::GetRandomFloat(0.f, 1.f);;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(5.f, 10.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);;
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(5.f, 7.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = 3.f;

		pEffect->fAlphaDelta = 0.f;

		pEffect->fRollDelta = 15.f;
		break;
	}

	D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
	pEffect->fAlphaDelta -= 255.f;
	
	m_EffectList.push_back(pEffect);
}
	
HRESULT CTeleport::Ready_Component( void )
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

void CTeleport::Free( void )
{
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pTextureCom);
	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pShaderCom);

	Engine::CGameObject::Free();
}

CTeleport* CTeleport::Create( LPDIRECT3DDEVICE9 pDevice, const _uint& iType)
{
	CTeleport* pTeleport = new CTeleport(pDevice);

	if (FAILED(pTeleport->Initialize(iType)))
	{
		MSG_BOX("Teleport Created Failed");
		Engine::Safe_Release(pTeleport);
	}
	return pTeleport;
}



#include "StdAfx.h"
#include "BladeEffect.h"
#include "DefaultCamera.h"
#include "LongBlade.h"

CBladeEffect::CBladeEffect( LPDIRECT3DDEVICE9 pDevice)
: CEffect(pDevice)
, m_pMatCompute(NULL)
{
	
}

CBladeEffect::~CBladeEffect( void )
{

}

HRESULT CBladeEffect::Initialize( const _uint& iType)
{
	m_iType = iType;
	m_pBone = ((Engine::CDynamicMesh*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(1)->GetComponent(L"Com_Mesh"))->GetPartsMatrix("ValveBiped_Anim_Attachment_RH");
	m_pTarget = ((Engine::CTransform*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(1)->GetComponent(L"Com_Transform"))->GetWorldMatrix();
	m_pMatCompute = ((CLongBlade*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(1))->GetComputeMatrix();
	m_vAngle = *((CDefaultCamera*)Engine::Get_CameraMgr()->GetCamera(L"Cam_Default"))->GetAngle();

	m_vOrigin = *(MyVec3*)&(*m_pBone * *m_pTarget).m[3][0];


	DefaultSetting();
	InitInstance();

	if (FAILED(Ready_Component()))
		return E_FAIL;

	for (_uint i = 0; i < m_iCount; ++i)
		AddParticle(i);

	return S_OK;
}

_int CBladeEffect::Update( const _float& fDelta )
{
	if (m_EffectList.empty() == true)//isDead() == true)
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

	PARTICLE_INSTANCE* pInst;
	MyMat matScale, matRotX, matRotY, matRotZ, matTrans, matWorld;
	m_pVBInstance->Lock( 0, NULL, ( void** )&pInst, 0 );
	ZeroMemory(pInst, sizeof(PARTICLE_INSTANCE) * 500);	

	MyMat matView, matProj;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	MyMat matBill;
	D3DXMatrixIdentity(&matBill);
	matBill._11 = matView._11;
	matBill._13 = matView._13;
	matBill._31 = matView._31;
	matBill._33 = matView._33;
	D3DXMatrixInverse(&matBill, NULL, &matBill);

	_uint i = 0;

	for ( ; iter != iter_end;)
	{	
		(*iter)->fLifeTime += fDelta;
		(*iter)->fFrame += (*iter)->fFrameSpeed * fDelta;
		if ((*iter)->fFrame >= 25.f)
			(*iter)->fFrame = 0.f;

		(*iter)->vPos += (*iter)->vDir * (*iter)->fSpeed * fDelta;

		_float	fSizeDelta = GetSizeDelta(*iter);
		_float	fRollDelta = GetRollDelta(*iter);
		_uint	iAlphaDelta = GetAlphaDelta(*iter);

		D3DXMatrixScaling(&matScale, (*iter)->vScale.x + fSizeDelta, (*iter)->vScale.y + fSizeDelta, (*iter)->vScale.z);
		D3DXMatrixRotationX(&matRotX, D3DXToRadian((*iter)->vAngle.x));
		D3DXMatrixRotationY(&matRotY, D3DXToRadian((*iter)->vAngle.y));
		D3DXMatrixRotationZ(&matRotZ, D3DXToRadian((*iter)->vAngle.z + fRollDelta));
		D3DXMatrixTranslation(&matTrans, (*iter)->vPos.x, (*iter)->vPos.y, (*iter)->vPos.z);
		matWorld = matScale * matRotZ * matRotX * matBill *  matTrans;

		pInst[i].Color = D3DXCOLOR(D3DCOLOR_ARGB(255 - iAlphaDelta, (_int)(*iter)->vColor.x, (_int)(*iter)->vColor.y, (_int)(*iter)->vColor.z));
		pInst[i].vRight = *(MyVec4*)&matWorld.m[0][0];
		pInst[i].vUp = *(MyVec4*)&matWorld.m[1][0];
		pInst[i].vLook = *(MyVec4*)&matWorld.m[2][0];
		pInst[i].vPos = *(MyVec4*)&matWorld.m[3][0];

		++i;

		if ((*iter)->fLifeTime >= (*iter)->fMaxLife)
		{
			Engine::Safe_Delete(*iter);
			iter = m_EffectList.erase(iter);
		}
		else
			++iter;
	}
	m_pVBInstance->Unlock();

	Engine::CGameObject::Update(fDelta);
	m_pRendererCom->AddRenderList(Engine::CRenderer::RENDER_ALPHA, this);

	return 0;
}

void CBladeEffect::Render( void )
{
	LPD3DXEFFECT pEffect = m_pShaderCom->GetEffect();
	if (pEffect == NULL)
		return;

	SetUpShader(pEffect);
}

void CBladeEffect::SetUpShader( LPD3DXEFFECT pEffect )
{
	pEffect->AddRef();

	MyMat matView, matProj;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	m_pDevice->SetVertexDeclaration(m_pVertexDecl);

	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(PARTICLEVTX));
	m_pDevice->SetStreamSourceFreq(0, D3DSTREAMSOURCE_INDEXEDDATA | 500);

	m_pDevice->SetStreamSource(1, m_pVBInstance, 0, sizeof(PARTICLE_INSTANCE));
	m_pDevice->SetStreamSourceFreq(1, D3DSTREAMSOURCE_INSTANCEDATA| 1);
	m_pDevice->SetIndices(m_pIB);

	m_pTextureCom->SetUpShader(pEffect, m_iIndex);
	
	pEffect->Begin(NULL, 0);
	if (m_iType == 6)
		pEffect->BeginPass(4);
	else
		pEffect->BeginPass(0);

	m_pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2 );
	
	pEffect->EndPass();
	pEffect->End();

	m_pDevice->SetStreamSourceFreq(0, 1);
	m_pDevice->SetStreamSourceFreq(1, 1);

	Engine::Safe_Release(pEffect);
}

void CBladeEffect::DefaultSetting( void )
{
	switch (m_iType)
	{
	// mb_blade
	case 0:	// 검기오라1
		m_pEffectKey = L"Component_Texture_SimpleAura";
		m_iCount = 1;
		m_iIndex = 1;

		m_fInterval = 0.03f;
		m_fDuration = 0.7f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;
	case 1:	// 검기오라2-1
		m_pEffectKey = L"Component_Texture_EfxWisp";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.01f;
		m_fDuration = 0.4f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 2:	// 검기오라3
		m_pEffectKey = L"Component_Texture_EfxWisp";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.01f;
		m_fDuration = 0.4f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 3:	// 검기오라4
		m_pEffectKey = L"Component_Texture_EfxWisp";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.01f;
		m_fDuration = 0.4f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 4:	// 반복검기 2
		m_pEffectKey = L"Component_Texture_EfxWisp";
		m_iCount = 12;
		m_iIndex = 0;

		m_fInterval = 0.03f;
		m_fDuration = 10.f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 5:	// 반복검기 3
		m_pEffectKey = L"Component_Texture_IceLight";
		m_iCount = 14;
		m_iIndex = 1;

		m_fInterval = 0.02f;
		m_fDuration = 10.f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 6:	// 반복검기 4
		m_pEffectKey = L"Component_Texture_Fire_Seed_D_a_s";
		m_iCount = 9;
		m_iIndex = 0;

		m_fInterval = 0.05f;
		m_fDuration = 10.f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;
	}
}

void CBladeEffect::AddParticle( const _int& iIndex )
{
	EFFECT* pEffect= new EFFECT;
	ZeroMemory(pEffect, sizeof(EFFECT));

	pEffect->vScale = MyVec3(1.f, 1.f, 1.f);
	D3DXVECTOR3 vMin(131.f, 149.f, 250.f);
	D3DXVECTOR3 vMax(181.f, 150.f, 250.f);
	Engine::GetRandomVector(&pEffect->vColor, &vMin, &vMax);

	MyMat matCompute = *m_pBone * *m_pTarget;
	MyVec3 vPos = *(MyVec3*)&matCompute.m[3][0];
	MyVec3 vRight = *D3DXVec3Normalize(&vRight, (MyVec3*)&m_pMatCompute->m[0][0]);
	MyVec3 vUp = *D3DXVec3Normalize(&vUp, (MyVec3*)&m_pMatCompute->m[1][0]);

	_float fSize = 0.f;

	switch (m_iType)
	{
	case 0:
		pEffect->vAngle = MyVec3(0.f, 90.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.2f, 0.35f);
		pEffect->fSpeed = Engine::GetRandomFloat(-10.f, -30.f);
		pEffect->fAngle = 85.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 2.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(1.f, 2.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = -1.f;
		pEffect->fSizeWayTime = 0.01f;
		pEffect->fSizeWayDelta = 0.5f;

		pEffect->fAlphaDelta = 0.f;

		pEffect->fRollDelta = 30.f;

		pEffect->bAlive = true;
		break;

	case 1:
		pEffect->vAngle = MyVec3(0.f, 90.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.3f, 0.5f);
		pEffect->fSpeed = Engine::GetRandomFloat(-20.f, -40.f);
		pEffect->fAngle = 55.f;
		pEffect->fRadius = Engine::GetRandomFloat(-10.f, -60.f);
		pEffect->fRadiusAngle = 0.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(1.f, 2.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos;

		pEffect->fSizeDelta = -0.9f;
		pEffect->fSizeWayTime = 0.01f;
		pEffect->fSizeWayDelta = 0.5f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.2f;
		pEffect->fAlphaWayDelta = 155.f;

		pEffect->fRollDelta = 50.f;

		pEffect->bAlive = true;
		break;	

	case 4:
		pEffect->vAngle = MyVec3(0.f, Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.2f, 0.8f);
		pEffect->fSpeed = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(-5.f, -55.f);
		pEffect->fRadiusAngle = Engine::GetRandomFloat(-360.f, 360.f);
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(1.f, 3.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos + vRight * (_float)iIndex * 0.5f + vUp * (_float)iIndex * 0.05f;

		pEffect->fSizeDelta = -0.7f;
		pEffect->fSizeWayTime = 0.01f;
		pEffect->fSizeWayDelta = -0.2f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.4f;
		pEffect->fAlphaWayDelta = 155.f;

		pEffect->fRollDelta = 30.f;

		pEffect->bAlive = true;
		break;


	case 5:
		pEffect->vAngle = MyVec3(0.f, Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.3f, 0.9f);
		pEffect->fSpeed = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(-5.f, -55.f);
		pEffect->fRadiusAngle = Engine::GetRandomFloat(-360.f, 360.f);
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(1.f, 3.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos + vRight * (_float)iIndex * 0.3f + vUp * (_float)iIndex * 0.03f;

		pEffect->fSizeDelta = -0.7f;
		pEffect->fSizeWayTime = 0.01f;
		pEffect->fSizeWayDelta = -0.2f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.4f;
		pEffect->fAlphaWayDelta = 225.f;

		pEffect->fRollDelta = 15.f;

		pEffect->bAlive = true;
		break;

	case 6:
		pEffect->vAngle = MyVec3(0.f, Engine::GetRandomFloat(-360.f, 360.f), Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->fMaxLife = Engine::GetRandomFloat(0.3f, 0.9f);
		pEffect->fSpeed = Engine::GetRandomFloat(0.f, 1.f);
		pEffect->fFrameSpeed = 30.f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(-5.f, -50.f);
		pEffect->fRadiusAngle = Engine::GetRandomFloat(-360.f, 360.f);
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(2.f, 3.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = vPos + vRight * (_float)iIndex * 0.7f + vUp * (_float)iIndex * 0.1f;

		pEffect->fSizeDelta = -0.7f;
		pEffect->fSizeWayTime = 0.01f;
		pEffect->fSizeWayDelta = -0.2f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.4f;
		pEffect->fAlphaWayDelta = 225.f;

		pEffect->fRollDelta = 15.f;

		pEffect->bAlive = true;
		break;
	}

	//pEffect->vAngle = m_vAngle;

	pEffect->fAlphaDelta -= 255.f;
	m_EffectList.push_back(pEffect);
}

HRESULT CBladeEffect::Ready_Component( void )
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

void CBladeEffect::Free( void )
{
	CEffect::Free();
	Engine::CGameObject::Free();
}

CBladeEffect* CBladeEffect::Create( LPDIRECT3DDEVICE9 pDevice, const _uint& iType)
{
	CBladeEffect* pBladeEffect = new CBladeEffect(pDevice);

	if (FAILED(pBladeEffect->Initialize(iType)))
	{
		MSG_BOX("BladeEffect Created Failed");
		Engine::Safe_Release(pBladeEffect);
	}
	return pBladeEffect;
}



#include "StdAfx.h"
#include "DarkNight.h"

CDarkNight::CDarkNight( LPDIRECT3DDEVICE9 pDevice)
: Engine::CGameObject(pDevice)
, m_pRendererCom(NULL)
, m_pTransformCom(NULL)
, m_pTextureCom(NULL)
, m_pBufferCom(NULL)
, m_pShaderCom(NULL)
, m_pTarget(NULL)
, m_iType(0)
, m_iIndex(0)
, m_pDarkNightKey(NULL)
{
	
}

CDarkNight::~CDarkNight( void )
{

}

HRESULT CDarkNight::Initialize( const _uint& iType, MyVec3* pPos)
{
	m_iType = iType;
	m_vOrigin = *pPos;
	m_pTarget = pPos;

	DefaultSetting();

	if (FAILED(Ready_Component()))
		return E_FAIL;

	for (_uint i = 0; i < m_iCount; ++i)
		AddParticle();

	return S_OK;
}

_int CDarkNight::Update( const _float& fDelta )
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

	EFFECTLIST::iterator iter = m_DarkNightList.begin();
	EFFECTLIST::iterator iter_end = m_DarkNightList.end();

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

void CDarkNight::Render( void )
{
	LPD3DXEFFECT pEffect = m_pShaderCom->GetEffect();
	if (pEffect == NULL)
		return;

	SetUpShader(pEffect);
}

void CDarkNight::SetUpShader( LPD3DXEFFECT pEffect )
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

	EFFECTLIST::iterator iter = m_DarkNightList.begin();
	EFFECTLIST::iterator iter_end = m_DarkNightList.end();
	MyMat matScale, matRotX, matRotY, matRotZ, matTrans, matWorld;

	for ( ; iter != iter_end; ++iter)
	{
		_float fSizeDelta = (*iter)->fLifeTime / (*iter)->fMaxLife * (*iter)->fSizeDelta;
		D3DXMatrixScaling(&matScale, (*iter)->vScale.x + fSizeDelta, (*iter)->vScale.y + fSizeDelta, (*iter)->vScale.z);
		D3DXMatrixRotationX(&matRotX, D3DXToRadian((*iter)->vAngle.x));
		D3DXMatrixRotationY(&matRotY, D3DXToRadian((*iter)->vAngle.y));
		D3DXMatrixRotationZ(&matRotZ, D3DXToRadian((*iter)->vAngle.z));
		D3DXMatrixTranslation(&matTrans, (*iter)->vPos.x, (*iter)->vPos.y, (*iter)->vPos.z);
		matWorld = matScale * matRotX * matRotY * matRotZ *  matTrans;
		pEffect->SetMatrix("g_matWorld", &matWorld);
		pEffect->SetVector("g_vColor", &MyVec4(D3DXCOLOR(D3DCOLOR_ARGB(255, (_int)(*iter)->vColor.x, (_int)(*iter)->vColor.y, (_int)(*iter)->vColor.z))));
		pEffect->CommitChanges();
		m_pBufferCom->Render();
	}
	
	pEffect->EndPass();
	pEffect->End();

	Engine::Safe_Release(pEffect);
}

void CDarkNight::DefaultSetting( void )
{
	switch (m_iType)
	{
	case 0: // ½º¸ðÅ©
		m_pDarkNightKey = L"Component_Texture_DarkSmoke";
		m_iCount = 2;
		m_iIndex = 0;

		m_fInterval = 0.01f;
		m_fDuration = 0.2f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 1: // ½º¸ðÅ©
		m_pDarkNightKey = L"Component_Texture_DarkSmoke";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.04f;
		m_fDuration = 0.4f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 2: // ´ÙÅ©¼¿
		m_pDarkNightKey = L"Component_Texture_DarkCell";
		m_iCount = 1;
		m_iIndex = 0;

		m_fInterval = 0.01f;
		m_fDuration = 0.6f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 3: // ´ÙÅ©¼¿
		m_pDarkNightKey = L"Component_Texture_DarkCell";
		m_iCount = 1;
		m_iIndex = 4;

		m_fInterval = 0.02f;
		m_fDuration = 0.4f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;
	}
}

void CDarkNight::AddParticle( void )
{
	EFFECT* pEffect= new EFFECT;
	ZeroMemory(pEffect, sizeof(EFFECT));

	pEffect->vScale = MyVec3(1.f, 1.f, 1.f);
	D3DXVECTOR3 vMin(131.f, 149.f, 250.f);
	D3DXVECTOR3 vMax(181.f, 150.f, 250.f);
	Engine::GetRandomVector(&pEffect->vColor, &vMin, &vMax);

	_float fSize = 0.f;

	switch (m_iType)
	{
	case 0:
		pEffect->vAngle = MyVec3(0.f, 0.f, Engine::GetRandomFloat(0.f, 360.f));
		pEffect->vColor = MyVec3(105.f, 31.f, 31.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.5f, 0.6f);
		pEffect->fSpeed = -5.f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(60.f, 75.f);
		pEffect->fRadiusAngle = 90.f;
		pEffect->vDir.x = pEffect->fRadius * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.z = Engine::GetRandomFloat(3.f, 5.f);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(15.f, 30.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos += m_vOrigin;
		pEffect->fSizeDelta = 3.f;
		pEffect->bAlive = true;
		break;

	case 1:
		pEffect->vAngle = MyVec3(0.f, 0.f, Engine::GetRandomFloat(0.f, 360.f));
		pEffect->vColor = MyVec3(105.f, 31.f, 31.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.3f, 0.8f);
		pEffect->fSpeed = 5.f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(5.f, 10.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(1.f, 3.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos += m_vOrigin;
		pEffect->fSizeDelta = 3.f;
		pEffect->bAlive = true;
		break;

	case 2:
		pEffect->vAngle = MyVec3(0.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(105.f, 31.f, 31.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.1f, 0.15f);
		pEffect->fSpeed = 5.f;
		pEffect->fAngle = 0.f;
		pEffect->fRadius = Engine::GetRandomFloat(3.f, 10.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(1.f, 2.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos += m_vOrigin;
		pEffect->fSizeDelta = -1.f;
		pEffect->bAlive = true;
		break;

	case 3:
		pEffect->vAngle = MyVec3(0.f, 0.f, Engine::GetRandomFloat(-360.f, 360.f));
		pEffect->vColor = MyVec3(50.f, 31.f, 31.f);
		pEffect->fMaxLife = Engine::GetRandomFloat(0.2f, 0.3f);
		pEffect->fSpeed = 5.f;
		pEffect->fAngle = 8.f;
		pEffect->fRadius = Engine::GetRandomFloat(0.f, 20.f);
		pEffect->fRadiusAngle = 360.f;
		pEffect->vDir.z = pEffect->fRadius * sinf(pEffect->fAngle) * cosf(pEffect->fRadiusAngle);
		pEffect->vDir.x = pEffect->fRadius * sinf(pEffect->fAngle) * sinf(pEffect->fRadiusAngle);
		pEffect->vDir.y = pEffect->fRadius * cosf(pEffect->fAngle);
		D3DXVec3Normalize(&pEffect->vDir, &pEffect->vDir);
		fSize = Engine::GetRandomFloat(5.f, 5.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos += m_vOrigin;
		pEffect->fSizeDelta = 10.f;
		pEffect->bAlive = true;
		break;
	}


	m_DarkNightList.push_back(pEffect);
}

bool CDarkNight::isDead( void )
{
	EFFECTLIST::iterator iter = m_DarkNightList.begin();
	EFFECTLIST::iterator iter_end = m_DarkNightList.end();

	if (iter == iter_end)
		return false;

	for ( ; iter != iter_end; ++iter)
	{	
		if ((*iter)->bAlive == true)
			return false;
	}
	return true;
}


HRESULT CDarkNight::Ready_Component( void )
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

	m_pTextureCom = (Engine::CTexture*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, m_pDarkNightKey);
	if (m_pTextureCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(Engine::CGameObject::UPDATE_NONE, L"Com_Texture", m_pTextureCom)))
		return E_FAIL;

	m_pBufferCom = (Engine::CTrailTex*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STATIC, L"Component_Buffer_RcTex");
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

void CDarkNight::Free( void )
{
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pTextureCom);
	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pShaderCom);

	Engine::CGameObject::Free();
}

CDarkNight* CDarkNight::Create( LPDIRECT3DDEVICE9 pDevice, const _uint& iType, MyVec3* pPos)
{
	CDarkNight* pDarkNight = new CDarkNight(pDevice);

	if (FAILED(pDarkNight->Initialize(iType, pPos)))
	{
		MSG_BOX("DarkNight Created Failed");
		Engine::Safe_Release(pDarkNight);
	}
	return pDarkNight;
}



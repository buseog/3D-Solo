#include "StdAfx.h"
#include "Damage.h"
#include "DefaultCamera.h"
#include "LongBlade.h"

CDamage::CDamage( LPDIRECT3DDEVICE9 pDevice)
: CEffect(pDevice)
, m_pMatCompute(NULL)
{
	
}

CDamage::~CDamage( void )
{

}

HRESULT CDamage::Initialize( const _uint& iType, const MyVec3& vPos)
{
	m_iType = iType;
	m_vAngle = *((CDefaultCamera*)Engine::Get_CameraMgr()->GetCamera(L"Cam_Default"))->GetAngle();

	m_vOrigin = vPos;


	DefaultSetting();

	if (FAILED(Ready_Component()))
		return E_FAIL;

	for (_uint i = 0; i < m_iCount; ++i)
		AddParticle(i);

	return S_OK;
}

_int CDamage::Update( const _float& fDelta )
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

			(*iter)->vPos += (*iter)->vDir * (*iter)->fSpeed * fDelta;

			if ((*iter)->fLifeTime >= (*iter)->fMaxLife)
				(*iter)->bAlive = false;
		}
	}

	Engine::CGameObject::Update(fDelta);
	m_pRendererCom->AddRenderList(Engine::CRenderer::RENDER_ALPHA, this);

	return 0;
}

void CDamage::Render( void )
{
	LPD3DXEFFECT pEffect = m_pShaderCom->GetEffect();
	if (pEffect == NULL)
		return;

	SetUpShader(pEffect);
}

void CDamage::SetUpShader( LPD3DXEFFECT pEffect )
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

void CDamage::DefaultSetting( void )
{
	switch (m_iType)
	{
	case 0:	// 베는빛
		m_pEffectKey = L"Component_Texture_SwordSharp";
		m_iCount = 1;
		m_iIndex = 1;

		m_fInterval = 0.6f;
		m_fDuration = 0.5f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;

	case 1:	// 돌아가는 xx
		m_pEffectKey = L"Component_Texture_SwordSharp";
		m_iCount = 1;
		m_iIndex = 2;

		m_fInterval = 0.4f;
		m_fDuration = 1.f;

		m_fDurationAcc = 0.f;
		m_fIntervalAcc = 0.f;
		break;
	
	}
}

void CDamage::AddParticle( const _int& iIndex )
{
	EFFECT* pEffect= new EFFECT;
	ZeroMemory(pEffect, sizeof(EFFECT));

	pEffect->vScale = MyVec3(1.f, 1.f, 1.f);
	D3DXVECTOR3 vMin(220.f, 220.f, 255.f);
	D3DXVECTOR3 vMax(255.f, 255.f, 255.f);
	Engine::GetRandomVector(&pEffect->vColor, &vMin, &vMax);

	_float fSize = 0.f;

	switch (m_iType)
	{
	case 0:
		pEffect->vAngle = MyVec3(0.f, 0.f, Engine::GetRandomFloat(70.f, 110.f));
		pEffect->fMaxLife = 0.25f;

		fSize = Engine::GetRandomFloat(25.f, 37.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = m_vOrigin;

		pEffect->fSizeDelta = 15.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.8f;
		pEffect->fAlphaWayDelta = 255.f;

		pEffect->bAlive = true;
		break;
	
	case 1:
		pEffect->vAngle = MyVec3(0.f, 0.f, Engine::GetRandomFloat(70.f, 110.f));
		pEffect->fMaxLife = 0.3f;

		fSize = Engine::GetRandomFloat(15.f, 22.f);
		pEffect->vScale = MyVec3(fSize, fSize, 1.f);
		pEffect->vPos = m_vOrigin;

		pEffect->fSizeDelta = 15.f;

		pEffect->fAlphaDelta = 0.f;
		pEffect->fAlphaWayTime = 0.8f;
		pEffect->fAlphaWayDelta = 255.f;

		pEffect->bAlive = true;
		break;
	}

	//pEffect->vAngle = m_vAngle;
	pEffect->fAlphaDelta -= 255.f;
	m_EffectList.push_back(pEffect);
}

HRESULT CDamage::Ready_Component( void )
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

void CDamage::Free( void )
{
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pTextureCom);
	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pShaderCom);

	Engine::CGameObject::Free();
}

CDamage* CDamage::Create( LPDIRECT3DDEVICE9 pDevice, const _uint& iType, const MyVec3& vPos)
{
	CDamage* pDamage = new CDamage(pDevice);

	if (FAILED(pDamage->Initialize(iType, vPos)))
	{
		MSG_BOX("Damage Created Failed");
		Engine::Safe_Release(pDamage);
	}
	return pDamage;
}



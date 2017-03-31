#include "StdAfx.h"
#include "SharpenCombo.h"
#include "LongBlade.h"

CSharpenCombo::CSharpenCombo(LPDIRECT3DDEVICE9 pDevice)
: CParticleSystem(pDevice)
, m_pRendererCom(NULL)
, m_pMatTarget(NULL)
, m_pShaderCom(NULL)
{

}

CSharpenCombo::~CSharpenCombo(void)
{
}

HRESULT CSharpenCombo::Initialize(MyMat* pMatTarget, const _uint& iType)
{
	m_iType = iType;
	m_pMatTarget = pMatTarget;
	m_iMaxCount = 0;
	m_vOrigin = *(MyVec3*)&m_pMatTarget->m[3][0] + MyVec3(0.f, 3.f, 0.f);
	m_vOriDir = *(MyVec3*)&m_pMatTarget->m[2][0];

	for (int i = 0; i < 50; ++i)
	{
		AddParticle(i);
	}

	
	D3DXVec3Normalize(&m_vOriDir, &m_vOriDir);

	m_fRefresh = 0.f;
	m_fSize = 50.f;
	m_dwVBSize = 2048;
	m_dwVBOffset = 0;
	m_dwVBBatchSize = 512;

	Engine::CParticleSystem::Initialize();
	
	if (FAILED(ReadyComponent()))
		return E_FAIL;

	return S_OK;
}

void CSharpenCombo::AddParticle( const _uint& iIndex )
{
	Engine::ATTRIBUTE* pAttribute = new Engine::ATTRIBUTE;
	ZeroMemory(pAttribute, sizeof(Engine::ATTRIBUTE));

	ResetParticle(pAttribute, iIndex);

	m_ParticleList.push_back(pAttribute);
}

void CSharpenCombo::ResetParticle(Engine::ATTRIBUTE* pAttribute, const _uint& iIndex)
{
	D3DXVECTOR3 vMin, vMax;
	vMin = D3DXVECTOR3(-2.f, -5.f, -2.f);
	vMax = D3DXVECTOR3(2.f, 2.f, 2.f);
	Engine::GetRandomVector(&pAttribute->vPos, &vMin, &vMax);

	pAttribute->vVelocity = MyVec3(0.f, Engine::GetRandomFloat(0.5f, 1.5f), 0.f);

	pAttribute->vColor = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

	pAttribute->vPos += m_vOrigin;
	pAttribute->bAlive = true;
	pAttribute->fSpeed = 20.f;
	pAttribute->fLifeTime = 0.4f;
}

_int CSharpenCombo::Update(const _float& fDelta)
{
	if (isDead())
		m_bDestroy = true;

	m_fRefresh -= fDelta;
	if (m_fRefresh <= 0.f)
	{
		++m_iMaxCount;
		m_fRefresh = 0.001f;
	}

	list<Engine::ATTRIBUTE*>::iterator iter = m_ParticleList.begin();
	list<Engine::ATTRIBUTE*>::iterator iter_end = m_ParticleList.end();

	for ( ; iter != iter_end; ++iter)
	{
		if ((*iter)->bAlive == true)
		{
			(*iter)->fLifeTime -= fDelta;
			if ((*iter)->fLifeTime <= 0.f)
				(*iter)->bAlive = false;

			(*iter)->vColor.a -= 0.01f;

			(*iter)->vPos += (*iter)->vVelocity * (*iter)->fSpeed * fDelta;
		}	
	}

	m_pRendererCom->AddRenderList(Engine::CRenderer::RENDER_ALPHA, this);
	return 0;
}

void CSharpenCombo::preRender( void )
{
	m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);
	m_pDevice->SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);
	m_pDevice->SetRenderState(D3DRS_POINTSIZE, FToD(m_fSize));
	m_pDevice->SetRenderState(D3DRS_POINTSIZE_MIN, FToD(1.f));
	m_pDevice->SetRenderState(D3DRS_POINTSIZE_MAX, FToD(200.f));

	m_pDevice->SetRenderState(D3DRS_POINTSCALE_A, FToD(0.f));
	m_pDevice->SetRenderState(D3DRS_POINTSCALE_B, FToD(0.f));
	m_pDevice->SetRenderState(D3DRS_POINTSCALE_C, FToD(1.f));

	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
}

void CSharpenCombo::Render(void)
{
	SetUpShader();
}

void CSharpenCombo::SetUpShader( void )
{
	LPD3DXEFFECT pEffect = m_pShaderCom->GetEffect();

	if (pEffect == NULL)
		return;

	pEffect->AddRef();

	if (m_ParticleList.empty() == false)
	{
		preRender();

		MyMat matIden, matView, matProj;
		D3DXMatrixIdentity(&matIden);

		m_pTextureCom->SetUpShader(pEffect, 4);
		m_pDevice->SetTransform(D3DTS_WORLD, &matIden);
		m_pDevice->GetTransform(D3DTS_VIEW, &matView);
		m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
		pEffect->SetMatrix("g_matView", &matView);
		pEffect->SetMatrix("g_matProj", &matProj);
		pEffect->SetMatrix("g_matWorld", &matIden);

		pEffect->Begin(NULL, 0);
		pEffect->BeginPass(4);

		m_pDevice->SetFVF(Engine::PARTICLEFVF);
		m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(Engine::PARTICLE));

		Engine::PARTICLE* pParticle = NULL;

		m_pVB->Lock(m_dwVBOffset * sizeof(Engine::PARTICLE), 
			m_dwVBBatchSize * sizeof(Engine::PARTICLE),
			(void**)&pParticle,
			m_dwVBOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

		DWORD numParticleInBatch = 0;

		list<Engine::ATTRIBUTE*>::iterator iter = m_ParticleList.begin();
		list<Engine::ATTRIBUTE*>::iterator iter_end = m_ParticleList.end();

		for (iter ; iter != iter_end; ++iter)
		{
			if ((*iter)->bAlive == true)
			{
				pParticle->vPos = (*iter)->vPos;
				pParticle->vColor = (D3DCOLOR)(*iter)->vColor;
				++pParticle;
				++numParticleInBatch;
			}
		}

		m_pVB->Unlock();

		if (numParticleInBatch)
			m_pDevice->DrawPrimitive(D3DPT_POINTLIST, m_dwVBOffset, numParticleInBatch);

		postRender();
	}

	pEffect->EndPass();
	pEffect->End();

	Engine::Safe_Release(pEffect);
}

HRESULT CSharpenCombo::ReadyComponent(void)
{
	m_pRendererCom = (Engine::CRenderer*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STATIC, L"Component_Renderer");
	if (m_pRendererCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(CGameObject::UPDATE_NONE, L"Com_Renderer", m_pRendererCom)))
		return E_FAIL;

	m_pTextureCom = (Engine::CTexture*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Texture_YellowFlare");
	if (m_pTextureCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(Engine::CGameObject::UPDATE_NONE, L"Com_Texture", m_pTextureCom)))
		return E_FAIL;

	m_pShaderCom = (Engine::CShader*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Shader_Effect");
	if (m_pShaderCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(Engine::CGameObject::UPDATE_NONE, L"Com_Shader", m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CSharpenCombo::Free(void)
{
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pShaderCom);

	Engine::CParticleSystem::Free();
}

CSharpenCombo* CSharpenCombo::Create(LPDIRECT3DDEVICE9 pDevice, MyMat* pMatTarget, const _uint& iType)
{
	CSharpenCombo* pSharpenCombo = new CSharpenCombo(pDevice);

	if (FAILED(pSharpenCombo->Initialize(pMatTarget, iType)))
	{
		Engine::Safe_Release(pSharpenCombo);
		MSG_BOX("SharpenCombo Created Failed");
	}
	return pSharpenCombo;
}
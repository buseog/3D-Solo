#include "StdAfx.h"
#include "Satellite.h"
#include "LongBlade.h"

CSatellite::CSatellite(LPDIRECT3DDEVICE9 pDevice)
: CParticleSystem(pDevice)
, m_pTargetPos(NULL)
, m_pShaderCom(NULL)
, m_pRendererCom(NULL)
{

}

CSatellite::~CSatellite(void)
{
}

HRESULT CSatellite::Initialize(const _tchar* pTexKey)
{
	for (int i = 0; i < 3; ++i)
		AddParticle(i);
	

	m_fRefreshTime = 0.3f;
	m_fSize = 50.f;
	m_dwVBSize = 2048;
	m_dwVBOffset = 0;
	m_dwVBBatchSize = 512;

	Engine::CParticleSystem::Initialize();
	
	if (FAILED(ReadyComponent(pTexKey)))
		return E_FAIL;

	return S_OK;
}

void CSatellite::AddParticle( const _uint& iIndex )
{
	Engine::ATTRIBUTE* pAttribute = new Engine::ATTRIBUTE;
	ZeroMemory(pAttribute, sizeof(Engine::ATTRIBUTE));

	ResetParticle(pAttribute, iIndex);

	m_ParticleList.push_back(pAttribute);
}

void CSatellite::ResetParticle(Engine::ATTRIBUTE* pAttribute, const _uint& iIndex)
{
	pAttribute->vColor = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

	pAttribute->bAlive = true;
	pAttribute->fSpeed = 10.f;
	pAttribute->fAngle = iIndex * 120.f;
	pAttribute->iType = iIndex;
}

_int CSatellite::Update(const _float& fDelta)
{
	if (m_pTargetPos == NULL)
		m_pTargetPos = ((CLongBlade*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(1))->GetComputeMatrix();
	
	MyVec3 vPos = *(MyVec3*)&m_pTargetPos->m[3][0];

	list<Engine::ATTRIBUTE*>::iterator iter = m_ParticleList.begin();
	list<Engine::ATTRIBUTE*>::iterator iter_end = m_ParticleList.end();

	MyVec3 vRot;

	for ( ; iter != iter_end; ++iter)
	{
		if ((*iter)->iType == 0)
		{
			vRot = MyVec3(1.f, 0.f, 0.f);
			(*iter)->vPos = MyVec3(0.f, 0.5f, 0.f);
		}
		if ((*iter)->iType == 1)
		{
			vRot = MyVec3(0.f, 1.f, 0.f);
			(*iter)->vPos = MyVec3(0.5f, 0.f, 0.f);
		}
		if ((*iter)->iType == 2)
		{
			vRot = MyVec3(0.f, 0.f, 1.f);
			(*iter)->vPos = MyVec3(0.5f, 0.f, 0.f);
		}

		D3DXVec3Normalize(&vRot, &vRot);
		(*iter)->fAngle += 180.f * fDelta;

		MyMat matAxis;
		D3DXMatrixRotationAxis(&matAxis, &vRot, D3DXToRadian((*iter)->fAngle));
		D3DXVec3TransformCoord(&(*iter)->vPos, &(*iter)->vPos, &matAxis);
		(*iter)->vPos += vPos;
	}

	m_pRendererCom->AddRenderList(Engine::CRenderer::RENDER_ALPHA, this);

	return 0;
}

void CSatellite::preRender( void )
{
	m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);
	m_pDevice->SetRenderState(D3DRS_POINTSCALEENABLE, FALSE);
	m_pDevice->SetRenderState(D3DRS_POINTSIZE, FToD(m_fSize));
	m_pDevice->SetRenderState(D3DRS_POINTSIZE_MIN, FToD(1.f));
	m_pDevice->SetRenderState(D3DRS_POINTSIZE_MAX, FToD(40.f));

	m_pDevice->SetRenderState(D3DRS_POINTSCALE_A, FToD(0.f));
	m_pDevice->SetRenderState(D3DRS_POINTSCALE_B, FToD(0.f));
	m_pDevice->SetRenderState(D3DRS_POINTSCALE_C, FToD(1.f));

	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
}

void CSatellite::Render(void)
{
	SetUpShader();
}

void CSatellite::SetUpShader( void )
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

		m_pTextureCom->SetUpShader(pEffect);
		m_pDevice->SetTransform(D3DTS_WORLD, &matIden);
		m_pDevice->GetTransform(D3DTS_VIEW, &matView);
		m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
		pEffect->SetMatrix("g_matView", &matView);
		pEffect->SetMatrix("g_matProj", &matProj);
		pEffect->SetMatrix("g_matWorld", &matIden);

		pEffect->Begin(NULL, 0);
		pEffect->BeginPass(1);

		m_pDevice->SetFVF(Engine::PARTICLEFVF);
		m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(Engine::PARTICLE));
		
		if (m_dwVBOffset >= m_dwVBSize)
			m_dwVBOffset = 0;

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

				if (numParticleInBatch == m_dwVBBatchSize)
				{	
					m_pVB->Unlock();

					m_pDevice->DrawPrimitive(D3DPT_POINTLIST, m_dwVBOffset, m_dwVBBatchSize);

					m_dwVBOffset += m_dwVBBatchSize;

					if (m_dwVBOffset >= m_dwVBSize)
						m_dwVBOffset = 0;

					m_pVB->Lock(m_dwVBOffset * sizeof(Engine::PARTICLE),
						m_dwVBBatchSize * sizeof(Engine::PARTICLE),
						(void**)&pParticle,
						m_dwVBOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

					numParticleInBatch = 0;
				}
			}
		}

		m_pVB->Unlock();

		if (numParticleInBatch)
			m_pDevice->DrawPrimitive(D3DPT_POINTLIST, m_dwVBOffset, numParticleInBatch);

		m_dwVBOffset += m_dwVBBatchSize;
		postRender();
	}

	pEffect->EndPass();
	pEffect->End();

	Engine::Safe_Release(pEffect);
}

HRESULT CSatellite::ReadyComponent(const _tchar* pTexkey)
{
	m_pRendererCom = (Engine::CRenderer*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STATIC, L"Component_Renderer");
	if (m_pRendererCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(CGameObject::UPDATE_NONE, L"Com_Renderer", m_pRendererCom)))
		return E_FAIL;

	m_pTextureCom = (Engine::CTexture*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, pTexkey);
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

void CSatellite::Free(void)
{
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pShaderCom);

	Engine::CParticleSystem::Free();
}

CSatellite* CSatellite::Create(LPDIRECT3DDEVICE9 pDevice, const _tchar* pTexKey)
{
	CSatellite* pSatellite = new CSatellite(pDevice);

	if (FAILED(pSatellite->Initialize(pTexKey)))
	{
		Engine::Safe_Release(pSatellite);
		MSG_BOX("Satellite Created Failed");
	}
	return pSatellite;
}
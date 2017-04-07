#include "StdAfx.h"
#include "Satellite.h"
#include "LongBlade.h"

CSatellite::CSatellite(LPDIRECT3DDEVICE9 pDevice)
: CParticleSystem(pDevice)
, m_pTargetPos(NULL)
, m_pShaderCom(NULL)
{

}

CSatellite::~CSatellite(void)
{
}

HRESULT CSatellite::Initialize(const _tchar* pTexKey)
{
	for (int i = 0; i < 7; ++i)
	{
		AddParticle();
	}

	m_fRefreshTime = 0.3f;
	m_fSize = 5.f;
	m_dwVBSize = 2048;
	m_dwVBOffset = 0;
	m_dwVBBatchSize = 512;

	Engine::CParticleSystem::Initialize();
	
	if (FAILED(ReadyComponent(pTexKey)))
		return E_FAIL;

	return S_OK;
}

void CSatellite::ResetParticle(Engine::ATTRIBUTE* pAttribute)
{
	D3DXVECTOR3 vMin = D3DXVECTOR3(-20.f, 3.f, -20.f);
	D3DXVECTOR3 vMax = D3DXVECTOR3(20.f, 9.f, 20.f);

	Engine::GetRandomVector(&pAttribute->vPos, &vMin, &vMax);
	pAttribute->vColor = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

	pAttribute->bAlive = true;
	pAttribute->fSpeed = 20.f;
}

_int CSatellite::Update(const _float& fDelta)
{
	if (isDead())
		m_bDestroy = true;

	if (m_pTargetPos == NULL)
		m_pTargetPos = ((CLongBlade*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(1))->GetComputeMatrix();

	list<Engine::ATTRIBUTE*>::iterator iter = m_ParticleList.begin();
	list<Engine::ATTRIBUTE*>::iterator iter_end = m_ParticleList.end();

	for ( ; iter != iter_end; ++iter)
	{
		if ((*iter)->bAlive == true)
		{
			(*iter)->fLifeTime -= fDelta;

			if ((*iter)->fLifeTime <= 0.f)
			{
				MyVec3 vMin(-1.f, -1.f, -1.f);
				MyVec3 vMax(1.f, 1.f, 1.f);
				MyVec3 vTemp;
				Engine::GetRandomVector(&vTemp, &vMin, &vMax);

				MyVec3 vRight = *(MyVec3*)&m_pTargetPos->m[0][0];
				D3DXVec3Normalize(&vRight, &vRight);
				int iRand = rand() % 3 + 3;
				(*iter)->vVelocity = *(MyVec3*)&m_pTargetPos->m[3][0] + vRight * iRand + vTemp;
				(*iter)->fLifeTime = 0.4f;
			}

			MyVec3 vDir = (*iter)->vVelocity - (*iter)->vPos;

			if (D3DXVec3Length(&vDir) < 0.3f)
				continue;

			D3DXVec3Normalize(&vDir, &vDir);
			(*iter)->vPos += vDir * (*iter)->fSpeed * fDelta;
		}	
	}

	return 0;
}

void CSatellite::preRender( void )
{
	m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);
	m_pDevice->SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);
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
		pEffect->BeginPass(4);

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
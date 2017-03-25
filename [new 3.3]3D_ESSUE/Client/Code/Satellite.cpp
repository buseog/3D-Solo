#include "StdAfx.h"
#include "Satellite.h"

CSatellite::CSatellite(LPDIRECT3DDEVICE9 pDevice)
: CParticleSystem(pDevice)
, m_pTargetPos(NULL)
{

}

CSatellite::~CSatellite(void)
{
}

HRESULT CSatellite::Initialize(const _tchar* pTexKey, MyVec3* pTarget)
{
	m_pTargetPos = pTarget;

	for (int i = 0; i < 7; ++i)
	{
		AddParticle();
	}

	m_fSize = 10.f;
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
	pAttribute->vPos += *m_pTargetPos;
	pAttribute->vColor = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

	pAttribute->bAlive = true;
	pAttribute->fSpeed = 0.3f;
}

_int CSatellite::Update(const _float& fDelta)
{
	if (isDead())
		m_bDestroy = true;

	list<Engine::ATTRIBUTE*>::iterator iter = m_ParticleList.begin();
	list<Engine::ATTRIBUTE*>::iterator iter_end = m_ParticleList.end();

	for ( ; iter != iter_end; ++iter)
	{
		if ((*iter)->bAlive == true)
		{
			MyVec3 vDir;

			MyVec3 vMin = D3DXVECTOR3(-20.f, 3.f, -20.f);
			MyVec3 vMax = D3DXVECTOR3(20.f, 9.f, 20.f);
			Engine::GetRandomVector(&vDir, &vMin, &vMax);

			vDir +=	*m_pTargetPos - (*iter)->vPos;
			_float fDis = fabs(D3DXVec3Length(&vDir));

			if (fDis > 10.f)
			{
				D3DXVec3Normalize(&vDir, &vDir);
				(*iter)->vPos += vDir * (*iter)->fSpeed;
			}
		}
	}

	return 0;
}

void CSatellite::preRender( void )
{
	// 렌더링에 앞서 지정해야될 렌더링 상태를 세팅함.
	// 시스템마다 다를수 있어서 가상화시킴.
	m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);
	m_pDevice->SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);
	m_pDevice->SetRenderState(D3DRS_POINTSIZE, FToD(m_fSize));
	m_pDevice->SetRenderState(D3DRS_POINTSIZE_MIN, FToD(20.f));
	m_pDevice->SetRenderState(D3DRS_POINTSIZE_MAX, FToD(100.f));

	// 거리에 따른 파티클의 크기를 정해줌
	m_pDevice->SetRenderState(D3DRS_POINTSCALE_A, FToD(0.f));
	m_pDevice->SetRenderState(D3DRS_POINTSCALE_B, FToD(0.f));
	m_pDevice->SetRenderState(D3DRS_POINTSCALE_C, FToD(1.f));

	// 텍스처의 알파를 이용함.

	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	// 알파 블렌드를 활성화해서 투명하게 했음. 이로인해 동그란 모양을 만들 수 있다.

	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

void CSatellite::Render(void)
{
	if (m_ParticleList.empty() == false)
	{
		// 렌더 상태를 지정

		preRender();

		m_pTextureCom->Render();
		m_pDevice->SetFVF(Engine::PARTICLEFVF);
		m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(Engine::PARTICLE));

		// 버텍스 버퍼를 벗어날 경우 처음부터 재시작

		if (m_dwVBOffset >= m_dwVBSize)
			m_dwVBOffset = 0;

		Engine::PARTICLE* pParticle = NULL;

		m_pVB->Lock(m_dwVBOffset * sizeof(Engine::PARTICLE), 
			m_dwVBBatchSize * sizeof(Engine::PARTICLE),
			(void**)&pParticle,
			m_dwVBOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

		DWORD numParticleInBatch = 0;

		MyMat matIden;
		D3DXMatrixIdentity(&matIden);

		m_pDevice->SetTransform(D3DTS_WORLD,&matIden);

		// 모든 파티클이 렌더링될 때까지

		list<Engine::ATTRIBUTE*>::iterator iter = m_ParticleList.begin();
		list<Engine::ATTRIBUTE*>::iterator iter_end = m_ParticleList.end();

		for (iter ; iter != iter_end; ++iter)
		{
			if ((*iter)->bAlive)
			{
				// 한 단계의 생존한 파티클을 다음 버텍스 버퍼로 복사한다.
				pParticle->vPos = (*iter)->vPos;

				pParticle->vColor = (D3DCOLOR)(*iter)->vColor;

				++pParticle;
				++numParticleInBatch;

				// 현재의 단계가 전부 채워져 있는지
				if (numParticleInBatch == m_dwVBBatchSize)
				{	
					// 버텍스 버퍼로 복사된 마지막 단계의 파티클을 그림

					m_pVB->Unlock();

					m_pDevice->DrawPrimitive(D3DPT_POINTLIST, m_dwVBOffset, m_dwVBBatchSize);

					// 단계가 그려지는 동안 다음 6단계를 파티클로 채움

					// 다음 단계의 처음 오프셋으로 이동함
					m_dwVBOffset += m_dwVBBatchSize;

					// 버텍스 버퍼의 경계를 넘는 메모리로 오프셋을 설정하지않음. 경계를 넘을 경우에 처음부터 시작함
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

		// numParticlesInBatch == m_dwVBBatch 가 만족되지 않았을 경우 마지막 단계의 렌더링이 되지 않을수도 있다.
		// 일부만 채워진 단계는 이곳에서 렌더링 시킴

		if (numParticleInBatch)
		{
			m_pDevice->DrawPrimitive(D3DPT_POINTLIST, m_dwVBOffset, numParticleInBatch);
		}

		// 다음블록
		m_dwVBOffset += m_dwVBBatchSize;

		postRender();
	}
}


HRESULT CSatellite::ReadyComponent(const _tchar* pTexkey)
{
	m_pTextureCom = (Engine::CTexture*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, pTexkey);
	if (m_pTextureCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(Engine::CGameObject::UPDATE_NONE, L"Com_Texture", m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

void CSatellite::Free(void)
{
	Engine::CParticleSystem::Free();
}

CSatellite* CSatellite::Create(LPDIRECT3DDEVICE9 pDevice, const _tchar* pTexKey, MyVec3* pTarget)
{
	CSatellite* pSatellite = new CSatellite(pDevice);

	if (FAILED(pSatellite->Initialize(pTexKey, pTarget)))
	{
		Engine::Safe_Release(pSatellite);
		MSG_BOX("Satellite Created Failed");
	}
	return pSatellite;
}
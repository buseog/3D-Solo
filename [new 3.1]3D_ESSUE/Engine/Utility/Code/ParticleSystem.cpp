#include "ParticleSystem.h"

Engine::CParticleSystem::CParticleSystem( LPDIRECT3DDEVICE9 pDevice )
: CGameObject(pDevice)
, m_pVB(NULL)
, m_pTexture(NULL)
, m_pBoundingBox(NULL)
, m_vOrigin(0.f, 0.f, 0.f)
, m_fRate(0.f)
, m_fSize(0.f)
, m_iMaxCount(0)
, m_dwVBSize(0)
, m_dwVBOffset(0)
, m_dwVBBatchSize(0)
{

}

Engine::CParticleSystem::~CParticleSystem(void)
{
	Release();
}

HRESULT Engine::CParticleSystem::Initialize( const wstring& wstrFileName )
{
	// 버텍스를 생성하고 텍스처를 만드는등의 초기화 작업

	HRESULT	hr = NULL;

	hr = m_pDevice->CreateVertexBuffer(m_dwVBSize * sizeof(Engine::PARTICLE),
										D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY, 
										Engine::PARTICLEFVF, 
										D3DPOOL_DEFAULT, &m_pVB, NULL);

	hr = D3DXCreateTextureFromFile(m_pDevice, wstrFileName.c_str(), (LPDIRECT3DTEXTURE9*)&m_pTexture);

	return S_OK;

}

void Engine::CParticleSystem::Reset( void )
{
	// 모든 파티클 초기화

	list<Engine::ATTRIBUTE*>::iterator iter = m_ParticleList.begin();
	list<Engine::ATTRIBUTE*>::iterator iter_end = m_ParticleList.end();

	for (iter ; iter != iter_end; ++iter)
	{
		ResetParticle(*iter);
	}
}

void Engine::CParticleSystem::AddParticle( void )
{
	Engine::ATTRIBUTE* pAttribute = new Engine::ATTRIBUTE;

	ResetParticle(pAttribute);

	m_ParticleList.push_back(pAttribute);
}

void Engine::CParticleSystem::preRender( void )
{
	// 렌더링에 앞서 지정해야될 렌더링 상태를 세팅함.
	// 시스템마다 다를수 있어서 가상화시킴.

	m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);
	m_pDevice->SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);
	m_pDevice->SetRenderState(D3DRS_POINTSIZE, FToD(m_fSize));
	m_pDevice->SetRenderState(D3DRS_POINTSIZE_MIN, FToD(5.f));
	m_pDevice->SetRenderState(D3DRS_POINTSIZE_MAX, FToD(10.f));

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


void Engine::CParticleSystem::Render( void )
{
	if (m_ParticleList.empty() == false)
	{
		// 렌더 상태를 지정

		preRender();

		m_pDevice->SetTexture(0, m_pTexture);
		m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(Engine::PARTICLE));
		m_pDevice->SetFVF(Engine::PARTICLEFVF);
		

		// 버텍스 버퍼를 벗어날 경우 처음부터 재시작

		if (m_dwVBOffset >= m_dwVBSize)
			m_dwVBOffset = 0;

		Engine::PARTICLE* pParticle = NULL;

		m_pVB->Lock(m_dwVBOffset * sizeof(Engine::PARTICLE), 
					m_dwVBBatchSize * sizeof(Engine::PARTICLE),
					(void**)&pParticle,
					m_dwVBOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

		DWORD numParticleInBatch = 0;

		// 모든 파티클이 렌더링될 때까지

		list<Engine::ATTRIBUTE*>::iterator iter = m_ParticleList.begin();
		list<Engine::ATTRIBUTE*>::iterator iter_end = m_ParticleList.end();

		for (iter ; iter != iter_end; ++iter)
		{
			if ((*iter)->bAlive == true)
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
					
					// 단계가 그려지는 동안 다음 단계를 파티클로 채움

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

void Engine::CParticleSystem::postRender( void )
{
	// 파티클 시스템이 지정한 렌더를 복구한다. 가상화시킴.

	//m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
	m_pDevice->SetRenderState(D3DRS_POINTSCALEENABLE, FALSE);
	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

bool Engine::CParticleSystem::isEmpty( void )
{
	return m_ParticleList.empty();
}

bool Engine::CParticleSystem::isDead( void )
{
	list<Engine::ATTRIBUTE*>::iterator iter = m_ParticleList.begin();
	list<Engine::ATTRIBUTE*>::iterator iter_end = m_ParticleList.end();

	for (iter ; iter != iter_end; ++iter)
	{
		if ((*iter)->bAlive)
			return false;
	}

	return true;
}

void Engine::CParticleSystem::RemoveDeadParticle( void )
{
	// 죽은 파티클들을 리스트에서 제거함

	list<Engine::ATTRIBUTE*>::iterator iter = m_ParticleList.begin();
	list<Engine::ATTRIBUTE*>::iterator iter_end = m_ParticleList.end();

	for (iter ; iter != iter_end;)
	{
		if ((*iter)->bAlive == false)
		{
			m_ParticleList.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

DWORD Engine::CParticleSystem::FToD(float f)
{
	return *((DWORD*)&f);
}

void Engine::CParticleSystem::Release(void)
{
	list<Engine::ATTRIBUTE*>::iterator iter = m_ParticleList.begin();
	list<Engine::ATTRIBUTE*>::iterator iter_end = m_ParticleList.end();

	for (iter ; iter != iter_end; ++iter)
	{
		Engine::Safe_Delete(*iter);
	}
	m_ParticleList.clear();

	if (m_pBoundingBox)
		Engine::Safe_Delete(m_pBoundingBox);
}

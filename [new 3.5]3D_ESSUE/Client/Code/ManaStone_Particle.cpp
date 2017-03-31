#include "StdAfx.h"
#include "ManaStone_Particle.h"

CManaStone::CManaStone(LPDIRECT3DDEVICE9 pDevice)
: CParticleSystem(pDevice)
, m_pRendererCom(NULL)
, m_pShaderCom(NULL)
{

}

CManaStone::~CManaStone(void)
{
}

HRESULT CManaStone::Initialize(const _tchar* pTexKey)
{
	for (int i = 0; i < 5; ++i)
	{
		AddParticle();
	}

	m_fSize = 500.f;
	m_dwVBSize = 2048;
	m_dwVBOffset = 0;
	m_dwVBBatchSize = 512;

	Engine::CParticleSystem::Initialize();
	
	if (FAILED(ReadyComponent(pTexKey)))
		return E_FAIL;

	return S_OK;
}

void CManaStone::ResetParticle(Engine::ATTRIBUTE* pAttribute)
{
	pAttribute->vColor = D3DXCOLOR(0.5f, 0.5f, 1.f, 1.f);
	pAttribute->vPos = D3DXVECTOR3(0.f, 0.f, 0.f);

	pAttribute->bAlive = true;
	pAttribute->fSpeed = 20.f;
}

_int CManaStone::Update(const _float& fDelta)
{
	if (isDead())
		m_bDestroy = true;

	list<Engine::ATTRIBUTE*>::iterator iter = m_ParticleList.begin();
	list<Engine::ATTRIBUTE*>::iterator iter_end = m_ParticleList.end();

	for ( ; iter != iter_end; ++iter)
	{
		if ((*iter)->bAlive == true)
		{
			(*iter)->fLifeTime += 10.f * fDelta;

			if ((*iter)->fLifeTime >= 20.f)
				(*iter)->fLifeTime = 0.f;
		}	
	}

	m_pRendererCom->AddRenderList(Engine::CRenderer::RENDER_ALPHA, this);

	return 0;
}

void CManaStone::preRender( void )
{
	// 렌더링에 앞서 지정해야될 렌더링 상태를 세팅함.
	// 시스템마다 다를수 있어서 가상화시킴.
	m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);
	m_pDevice->SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);
	m_pDevice->SetRenderState(D3DRS_POINTSIZE, FToD(m_fSize));
	m_pDevice->SetRenderState(D3DRS_POINTSIZE_MIN, FToD(20.f));
	m_pDevice->SetRenderState(D3DRS_POINTSIZE_MAX, FToD(500.f));

	// 거리에 따른 파티클의 크기를 정해줌
	m_pDevice->SetRenderState(D3DRS_POINTSCALE_A, FToD(0.f));
	m_pDevice->SetRenderState(D3DRS_POINTSCALE_B, FToD(0.f));
	m_pDevice->SetRenderState(D3DRS_POINTSCALE_C, FToD(1.f));

	// 텍스처의 알파를 이용함.

	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	// 알파 블렌드를 활성화해서 투명하게 했음. 이로인해 동그란 모양을 만들 수 있다.
	m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pDevice->SetRenderState(D3DRS_ALPHAREF, 0x8f);
	m_pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

void CManaStone::Render(void)
{
	SetUpShader();
	//if (m_ParticleList.empty() == false)
	//{
	//	// 렌더 상태를 지정
	//	preRender();
	//	m_pTextureCom->Render();
	//	m_pDevice->SetFVF(Engine::PARTICLEFVF);
	//	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(Engine::PARTICLE));

	//	// 버텍스 버퍼를 벗어날 경우 처음부터 재시작

	//	if (m_dwVBOffset >= m_dwVBSize)
	//		m_dwVBOffset = 0;

	//	Engine::PARTICLE* pParticle = NULL;

	//	m_pVB->Lock(m_dwVBOffset * sizeof(Engine::PARTICLE), 
	//		m_dwVBBatchSize * sizeof(Engine::PARTICLE),
	//		(void**)&pParticle,
	//		m_dwVBOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

	//	DWORD numParticleInBatch = 0;

	//	MyMat matIden;
	//	D3DXMatrixIdentity(&matIden);

	//	m_pDevice->SetTransform(D3DTS_WORLD,&matIden);

	//	// 모든 파티클이 렌더링될 때까지

	//	list<Engine::ATTRIBUTE*>::iterator iter = m_ParticleList.begin();
	//	list<Engine::ATTRIBUTE*>::iterator iter_end = m_ParticleList.end();

	//	for (iter ; iter != iter_end; ++iter)
	//	{
	//		if ((*iter)->bAlive)
	//		{
	//			// 한 단계의 생존한 파티클을 다음 버텍스 버퍼로 복사한다.
	//			pParticle->vPos = (*iter)->vPos;

	//			pParticle->vColor = (D3DCOLOR)(*iter)->vColor;

	//			++pParticle;
	//			++numParticleInBatch;

	//			// 현재의 단계가 전부 채워져 있는지
	//			if (numParticleInBatch == m_dwVBBatchSize)
	//			{	
	//				// 버텍스 버퍼로 복사된 마지막 단계의 파티클을 그림

	//				m_pVB->Unlock();

	//				m_pDevice->DrawPrimitive(D3DPT_POINTLIST, m_dwVBOffset, m_dwVBBatchSize);

	//				// 단계가 그려지는 동안 다음 6단계를 파티클로 채움

	//				// 다음 단계의 처음 오프셋으로 이동함
	//				m_dwVBOffset += m_dwVBBatchSize;

	//				// 버텍스 버퍼의 경계를 넘는 메모리로 오프셋을 설정하지않음. 경계를 넘을 경우에 처음부터 시작함
	//				if (m_dwVBOffset >= m_dwVBSize)
	//					m_dwVBOffset = 0;

	//				m_pVB->Lock(m_dwVBOffset * sizeof(Engine::PARTICLE),
	//					m_dwVBBatchSize * sizeof(Engine::PARTICLE),
	//					(void**)&pParticle,
	//					m_dwVBOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

	//				numParticleInBatch = 0;
	//			}
	//		}
	//	}
	//	m_pVB->Unlock();

	//	// numParticlesInBatch == m_dwVBBatch 가 만족되지 않았을 경우 마지막 단계의 렌더링이 되지 않을수도 있다.
	//	// 일부만 채워진 단계는 이곳에서 렌더링 시킴
	//	if (numParticleInBatch)
	//	{
	//		m_pDevice->DrawPrimitive(D3DPT_POINTLIST, m_dwVBOffset, numParticleInBatch);
	//	}
	//	// 다음블록
	//	m_dwVBOffset += m_dwVBBatchSize;

	//	postRender();
	//}
}

void CManaStone::SetUpShader( void )
{
	LPD3DXEFFECT pEffect = m_pShaderCom->GetEffect();

	if (pEffect == NULL)
		return;

	pEffect->AddRef();

	if (m_ParticleList.empty() == false)
	{
		// 렌더 상태를 지정
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
		pEffect->BeginPass(5);

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
				
				_uint fY = (*iter)->fLifeTime / 5;
				_uint fX = (*iter)->fLifeTime - (5 * fY);
				
				pEffect->SetInt("g_fX", fX);
				pEffect->SetInt("g_fY", fY);

				pEffect->CommitChanges();

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
			//pEffect->CommitChanges();
		}
		m_pVB->Unlock();

		if (numParticleInBatch)
			m_pDevice->DrawPrimitive(D3DPT_POINTLIST, m_dwVBOffset, numParticleInBatch);

		// 다음블록
		m_dwVBOffset += m_dwVBBatchSize;
		postRender();
	}

	pEffect->EndPass();
	pEffect->End();

	Engine::Safe_Release(pEffect);
}

HRESULT CManaStone::ReadyComponent(const _tchar* pTexkey)
{
	m_pRendererCom = (Engine::CRenderer*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STATIC, L"Component_Renderer");
	if (m_pRendererCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(Engine::CGameObject::UPDATE_NONE, L"Com_Renderer", m_pRendererCom)))
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

void CManaStone::Free(void)
{
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pShaderCom);

	Engine::CParticleSystem::Free();
}

CManaStone* CManaStone::Create(LPDIRECT3DDEVICE9 pDevice, const _tchar* pTexKey)
{
	CManaStone* pManaStone = new CManaStone(pDevice);

	if (FAILED(pManaStone->Initialize(pTexKey)))
	{
		Engine::Safe_Release(pManaStone);
		MSG_BOX("ManaStone Created Failed");
	}
	return pManaStone;
}
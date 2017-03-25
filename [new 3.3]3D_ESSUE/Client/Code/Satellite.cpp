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
	// �������� �ռ� �����ؾߵ� ������ ���¸� ������.
	// �ý��۸��� �ٸ��� �־ ����ȭ��Ŵ.
	m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);
	m_pDevice->SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);
	m_pDevice->SetRenderState(D3DRS_POINTSIZE, FToD(m_fSize));
	m_pDevice->SetRenderState(D3DRS_POINTSIZE_MIN, FToD(20.f));
	m_pDevice->SetRenderState(D3DRS_POINTSIZE_MAX, FToD(100.f));

	// �Ÿ��� ���� ��ƼŬ�� ũ�⸦ ������
	m_pDevice->SetRenderState(D3DRS_POINTSCALE_A, FToD(0.f));
	m_pDevice->SetRenderState(D3DRS_POINTSCALE_B, FToD(0.f));
	m_pDevice->SetRenderState(D3DRS_POINTSCALE_C, FToD(1.f));

	// �ؽ�ó�� ���ĸ� �̿���.

	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	// ���� ���带 Ȱ��ȭ�ؼ� �����ϰ� ����. �̷����� ���׶� ����� ���� �� �ִ�.

	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

void CSatellite::Render(void)
{
	if (m_ParticleList.empty() == false)
	{
		// ���� ���¸� ����

		preRender();

		m_pTextureCom->Render();
		m_pDevice->SetFVF(Engine::PARTICLEFVF);
		m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(Engine::PARTICLE));

		// ���ؽ� ���۸� ��� ��� ó������ �����

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

		// ��� ��ƼŬ�� �������� ������

		list<Engine::ATTRIBUTE*>::iterator iter = m_ParticleList.begin();
		list<Engine::ATTRIBUTE*>::iterator iter_end = m_ParticleList.end();

		for (iter ; iter != iter_end; ++iter)
		{
			if ((*iter)->bAlive)
			{
				// �� �ܰ��� ������ ��ƼŬ�� ���� ���ؽ� ���۷� �����Ѵ�.
				pParticle->vPos = (*iter)->vPos;

				pParticle->vColor = (D3DCOLOR)(*iter)->vColor;

				++pParticle;
				++numParticleInBatch;

				// ������ �ܰ谡 ���� ä���� �ִ���
				if (numParticleInBatch == m_dwVBBatchSize)
				{	
					// ���ؽ� ���۷� ����� ������ �ܰ��� ��ƼŬ�� �׸�

					m_pVB->Unlock();

					m_pDevice->DrawPrimitive(D3DPT_POINTLIST, m_dwVBOffset, m_dwVBBatchSize);

					// �ܰ谡 �׷����� ���� ���� 6�ܰ踦 ��ƼŬ�� ä��

					// ���� �ܰ��� ó�� ���������� �̵���
					m_dwVBOffset += m_dwVBBatchSize;

					// ���ؽ� ������ ��踦 �Ѵ� �޸𸮷� �������� ������������. ��踦 ���� ��쿡 ó������ ������
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

		// numParticlesInBatch == m_dwVBBatch �� �������� �ʾ��� ��� ������ �ܰ��� �������� ���� �������� �ִ�.
		// �Ϻθ� ä���� �ܰ�� �̰����� ������ ��Ŵ

		if (numParticleInBatch)
		{
			m_pDevice->DrawPrimitive(D3DPT_POINTLIST, m_dwVBOffset, numParticleInBatch);
		}

		// �������
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
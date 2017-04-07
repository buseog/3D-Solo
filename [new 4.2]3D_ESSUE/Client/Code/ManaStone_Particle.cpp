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
	// �������� �ռ� �����ؾߵ� ������ ���¸� ������.
	// �ý��۸��� �ٸ��� �־ ����ȭ��Ŵ.
	m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);
	m_pDevice->SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);
	m_pDevice->SetRenderState(D3DRS_POINTSIZE, FToD(m_fSize));
	m_pDevice->SetRenderState(D3DRS_POINTSIZE_MIN, FToD(20.f));
	m_pDevice->SetRenderState(D3DRS_POINTSIZE_MAX, FToD(500.f));

	// �Ÿ��� ���� ��ƼŬ�� ũ�⸦ ������
	m_pDevice->SetRenderState(D3DRS_POINTSCALE_A, FToD(0.f));
	m_pDevice->SetRenderState(D3DRS_POINTSCALE_B, FToD(0.f));
	m_pDevice->SetRenderState(D3DRS_POINTSCALE_C, FToD(1.f));

	// �ؽ�ó�� ���ĸ� �̿���.

	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	// ���� ���带 Ȱ��ȭ�ؼ� �����ϰ� ����. �̷����� ���׶� ����� ���� �� �ִ�.
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
	//	// ���� ���¸� ����
	//	preRender();
	//	m_pTextureCom->Render();
	//	m_pDevice->SetFVF(Engine::PARTICLEFVF);
	//	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(Engine::PARTICLE));

	//	// ���ؽ� ���۸� ��� ��� ó������ �����

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

	//	// ��� ��ƼŬ�� �������� ������

	//	list<Engine::ATTRIBUTE*>::iterator iter = m_ParticleList.begin();
	//	list<Engine::ATTRIBUTE*>::iterator iter_end = m_ParticleList.end();

	//	for (iter ; iter != iter_end; ++iter)
	//	{
	//		if ((*iter)->bAlive)
	//		{
	//			// �� �ܰ��� ������ ��ƼŬ�� ���� ���ؽ� ���۷� �����Ѵ�.
	//			pParticle->vPos = (*iter)->vPos;

	//			pParticle->vColor = (D3DCOLOR)(*iter)->vColor;

	//			++pParticle;
	//			++numParticleInBatch;

	//			// ������ �ܰ谡 ���� ä���� �ִ���
	//			if (numParticleInBatch == m_dwVBBatchSize)
	//			{	
	//				// ���ؽ� ���۷� ����� ������ �ܰ��� ��ƼŬ�� �׸�

	//				m_pVB->Unlock();

	//				m_pDevice->DrawPrimitive(D3DPT_POINTLIST, m_dwVBOffset, m_dwVBBatchSize);

	//				// �ܰ谡 �׷����� ���� ���� 6�ܰ踦 ��ƼŬ�� ä��

	//				// ���� �ܰ��� ó�� ���������� �̵���
	//				m_dwVBOffset += m_dwVBBatchSize;

	//				// ���ؽ� ������ ��踦 �Ѵ� �޸𸮷� �������� ������������. ��踦 ���� ��쿡 ó������ ������
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

	//	// numParticlesInBatch == m_dwVBBatch �� �������� �ʾ��� ��� ������ �ܰ��� �������� ���� �������� �ִ�.
	//	// �Ϻθ� ä���� �ܰ�� �̰����� ������ ��Ŵ
	//	if (numParticleInBatch)
	//	{
	//		m_pDevice->DrawPrimitive(D3DPT_POINTLIST, m_dwVBOffset, numParticleInBatch);
	//	}
	//	// �������
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
		// ���� ���¸� ����
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

		// ���ؽ� ���۸� ��� ��� ó������ �����

		if (m_dwVBOffset >= m_dwVBSize)
			m_dwVBOffset = 0;

		Engine::PARTICLE* pParticle = NULL;

		m_pVB->Lock(m_dwVBOffset * sizeof(Engine::PARTICLE), 
			m_dwVBBatchSize * sizeof(Engine::PARTICLE),
			(void**)&pParticle,
			m_dwVBOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

		DWORD numParticleInBatch = 0;

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
				
				_uint fY = (*iter)->fLifeTime / 5;
				_uint fX = (*iter)->fLifeTime - (5 * fY);
				
				pEffect->SetInt("g_fX", fX);
				pEffect->SetInt("g_fY", fY);

				pEffect->CommitChanges();

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
			//pEffect->CommitChanges();
		}
		m_pVB->Unlock();

		if (numParticleInBatch)
			m_pDevice->DrawPrimitive(D3DPT_POINTLIST, m_dwVBOffset, numParticleInBatch);

		// �������
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
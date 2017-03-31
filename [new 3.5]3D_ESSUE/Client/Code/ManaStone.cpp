#include "StdAfx.h"
#include "ManaStone.h"
#include "LongBlade.h"

CManaStone::CManaStone(LPDIRECT3DDEVICE9 pDevice)
: CParticleSystem(pDevice)
, m_pRendererCom(NULL)
, m_pShaderCom(NULL)
, m_pMatParent(NULL)
, m_pMatAttach(NULL)
{

}

CManaStone::~CManaStone(void)
{
}

HRESULT CManaStone::Initialize(void)
{
	for (int i = 0; i < 5; ++i)
	{
		AddParticle();
	}

	m_fSize = 40.f;
	m_dwVBSize = 2048;
	m_dwVBOffset = 0;
	m_dwVBBatchSize = 512;

	Engine::CParticleSystem::Initialize();

	if (FAILED(ReadyComponent()))
		return E_FAIL;

	m_pMatParent = ((Engine::CTransform*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(0)->GetComponent(L"Com_Transform"))->GetWorldMatrix();
	m_pMatAttach = ((Engine::CDynamicMesh*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(0)->GetComponent(L"Com_Mesh"))->GetPartsMatrix("ValveBiped_Anim_Attachment_LH");

	return S_OK;
}

void CManaStone::ResetParticle(Engine::ATTRIBUTE* pAttribute)
{
	pAttribute->vColor = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

	pAttribute->bAlive = true;
	pAttribute->fSpeed = 20.f;
	pAttribute->fLifeTime = float(rand() % 20);
}

_int CManaStone::Update(const _float& fDelta)
{
	if (isDead())
		m_bDestroy = true;

	m_matWorld = *m_pMatAttach * *m_pMatParent;


	list<Engine::ATTRIBUTE*>::iterator iter = m_ParticleList.begin();
	list<Engine::ATTRIBUTE*>::iterator iter_end = m_ParticleList.end();

	for ( ; iter != iter_end; ++iter)
	{
		if ((*iter)->bAlive == true)
		{
			(*iter)->fLifeTime += 7 * fDelta;
			(*iter)->vPos = *(MyVec3*)&m_matWorld.m[3][0] + MyVec3(0.f, 0.5f, 0.f);

			if ((*iter)->fLifeTime >= 10.f)
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
	m_pDevice->SetRenderState(D3DRS_POINTSIZE_MIN, FToD(1.f));
	m_pDevice->SetRenderState(D3DRS_POINTSIZE_MAX, FToD(60.f));

	// 거리에 따른 파티클의 크기를 정해줌
	m_pDevice->SetRenderState(D3DRS_POINTSCALE_A, FToD(0.f));
	m_pDevice->SetRenderState(D3DRS_POINTSCALE_B, FToD(0.f));
	m_pDevice->SetRenderState(D3DRS_POINTSCALE_C, FToD(1.f));

	// 텍스처의 알파를 이용함.

	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
}

void CManaStone::Render(void)
{
	SetUpShader();
}

void CManaStone::SetUpShader( void )
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

HRESULT CManaStone::ReadyComponent(void)
{
	m_pRendererCom = (Engine::CRenderer*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STATIC, L"Component_Renderer");
	if (m_pRendererCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(CGameObject::UPDATE_NONE, L"Com_Renderer", m_pRendererCom)))
		return E_FAIL;

	m_pTextureCom = (Engine::CTexture*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Texture_ManaStone");
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

CManaStone* CManaStone::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CManaStone* pManaStone = new CManaStone(pDevice);

	if (FAILED(pManaStone->Initialize()))
	{
		Engine::Safe_Release(pManaStone);
		MSG_BOX("ManaStone Created Failed");
	}
	return pManaStone;
}
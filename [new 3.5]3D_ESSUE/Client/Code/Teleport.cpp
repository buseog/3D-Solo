#include "StdAfx.h"
#include "Teleport.h"
#include "LongBlade.h"

CTeleport::CTeleport(LPDIRECT3DDEVICE9 pDevice)
: CParticleSystem(pDevice)
, m_pRendererCom(NULL)
, m_pMatTarget(NULL)
, m_pShaderCom(NULL)
{

}

CTeleport::~CTeleport(void)
{
}

HRESULT CTeleport::Initialize(MyMat* pMatTarget, const _uint& iType)
{
	m_iType = iType;
	m_pMatTarget = pMatTarget;
	m_iMaxCount = 0;
	m_vOrigin = *(MyVec3*)&m_pMatTarget->m[3][0] + MyVec3(0.f, 3.f, 0.f);
	m_vOriDir = *(MyVec3*)&m_pMatTarget->m[2][0];

	for (int i = 0; i < 10; ++i)
	{
		AddParticle(i);
	}

	
	D3DXVec3Normalize(&m_vOriDir, &m_vOriDir);

	m_fRefresh = 0.f;
	m_fLifeTime = 0.5f;
	m_fSize = 5.f;
	m_dwVBSize = 2048;
	m_dwVBOffset = 0;
	m_dwVBBatchSize = 512;

	Engine::CParticleSystem::Initialize();
	
	if (FAILED(ReadyComponent()))
		return E_FAIL;

	return S_OK;
}

void CTeleport::AddParticle( const _uint& iIndex )
{
	Engine::ATTRIBUTE* pAttribute = new Engine::ATTRIBUTE;
	ZeroMemory(pAttribute, sizeof(Engine::ATTRIBUTE));

	ResetParticle(pAttribute, iIndex);

	m_ParticleList.push_back(pAttribute);
}

void CTeleport::ResetParticle(Engine::ATTRIBUTE* pAttribute, const _uint& iIndex)
{
	pAttribute->vColor = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

	pAttribute->vPos = m_vOrigin;
	pAttribute->vVelocity = pAttribute->vPos;
	pAttribute->bAlive = true;
	pAttribute->fSpeed = 10.f;
	pAttribute->fLifeTime = 2.f;
	pAttribute->fAge -= D3DXToRadian(iIndex);
}

_int CTeleport::Update(const _float& fDelta)
{
	if (isDead())
		m_bDestroy = true;

	m_fLifeTime -= fDelta;
	if (m_fLifeTime <= 0.f)
		m_bDestroy = true;

	m_fRefresh -= fDelta;
	if (m_fRefresh <= 0.f)
	{
		++m_iMaxCount;
		m_fRefresh = 0.001f;
	}

	list<Engine::ATTRIBUTE*>::iterator iter = m_ParticleList.begin();
	list<Engine::ATTRIBUTE*>::iterator iter_end = m_ParticleList.end();

	MyVec3 vRight;
	D3DXVec3Cross(&vRight, &MyVec3(0.f, 1.f, 0.f), &MyVec3(m_vOriDir));
	//vRight /= 2.f;
	if (m_iType == 1)
		vRight *= -1.f;

	MyMat matAxis;

	_uint iCnt = 0;

	for ( ; iter != iter_end; ++iter)
	{
		if ((*iter)->bAlive == true)
		{
			if (iCnt > m_iMaxCount)
				break;
			++iCnt;

			(*iter)->fLifeTime -= fDelta;
			if ((*iter)->fLifeTime <= 0.f)
				(*iter)->bAlive = false;

			(*iter)->fAge += 0.2f;
			(*iter)->vVelocity += m_vOriDir * (*iter)->fSpeed * fDelta;

			D3DXMatrixRotationAxis(&matAxis, &m_vOriDir, (*iter)->fAge);
			D3DXVec3TransformCoord(&(*iter)->vPos, &vRight, &matAxis);

			(*iter)->vPos += (*iter)->vVelocity;
		}	
	}

	m_pRendererCom->AddRenderList(Engine::CRenderer::RENDER_ALPHA, this);
	return 0;
}

void CTeleport::preRender( void )
{
	m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);
	m_pDevice->SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);
	m_pDevice->SetRenderState(D3DRS_POINTSIZE, FToD(m_fSize));
	m_pDevice->SetRenderState(D3DRS_POINTSIZE_MIN, FToD(1.f));
	m_pDevice->SetRenderState(D3DRS_POINTSIZE_MAX, FToD(20.f));

	m_pDevice->SetRenderState(D3DRS_POINTSCALE_A, FToD(0.f));
	m_pDevice->SetRenderState(D3DRS_POINTSCALE_B, FToD(0.f));
	m_pDevice->SetRenderState(D3DRS_POINTSCALE_C, FToD(1.f));

	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
}

void CTeleport::Render(void)
{
	SetUpShader();
}

void CTeleport::SetUpShader( void )
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

HRESULT CTeleport::ReadyComponent(void)
{
	m_pRendererCom = (Engine::CRenderer*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STATIC, L"Component_Renderer");
	if (m_pRendererCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(CGameObject::UPDATE_NONE, L"Com_Renderer", m_pRendererCom)))
		return E_FAIL;

	m_pTextureCom = (Engine::CTexture*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Texture_Bomb");
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

void CTeleport::Free(void)
{
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pShaderCom);

	Engine::CParticleSystem::Free();
}

CTeleport* CTeleport::Create(LPDIRECT3DDEVICE9 pDevice, MyMat* pMatTarget, const _uint& iType)
{
	CTeleport* pTeleport = new CTeleport(pDevice);

	if (FAILED(pTeleport->Initialize(pMatTarget, iType)))
	{
		Engine::Safe_Release(pTeleport);
		MSG_BOX("Teleport Created Failed");
	}
	return pTeleport;
}
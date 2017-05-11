#include "stdafx.h"
#include "ParticleBuffer.h"

CParticleBuffer::CParticleBuffer(LPDIRECT3DDEVICE9 pGraphicDev)
: CGameObject(pGraphicDev)
, m_pRendererCom(NULL)
, m_pTransformCom(NULL)
, m_pTextureCom(NULL)
, m_pShaderCom(NULL)
, m_pTarget(NULL)
, m_pVB(NULL)
, m_pIB(NULL)
, m_pVBInstance(NULL)
, m_pVertexDecl(NULL)
{

}

CParticleBuffer::~CParticleBuffer(void)
{

}

HRESULT CParticleBuffer::Initialize(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;


	D3DVERTEXELEMENT9 g_VertexElemHardware[] =
	{
		{ 0, 0,	D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 1, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		{ 1, 4, D3DDECLTYPE_FLOAT4,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
		{ 1, 20, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 },
		{ 1, 36, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3 },
		{ 1, 52, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 4 },
		D3DDECL_END()
	};

	m_fTime = 2.f;

	m_pTarget = ((Engine::CTransform*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(0)->GetComponent(L"Com_Transform"))->GetWorldMatrix();

	m_pDevice->CreateVertexDeclaration(g_VertexElemHardware, &m_pVertexDecl);

	m_pDevice->CreateVertexBuffer(4 * sizeof(PARTICLEVTX), 0, 0, D3DPOOL_MANAGED, &m_pVB, 0);

	m_pDevice->CreateIndexBuffer(2 * sizeof( Engine::INDEX16 ), 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, 0);
	
	PARTICLEVTX* pVerts;

	m_pVB->Lock( 0, NULL, ( void** )&pVerts, 0 );

	pVerts[0].vPos = D3DXVECTOR3(-1.f, 1.f, 0.f);
	pVerts[0].vTexUV = D3DXVECTOR2(0.f, 0.f);

	pVerts[1].vPos = D3DXVECTOR3(1.f, 1.f, 0.f);
	pVerts[1].vTexUV = D3DXVECTOR2(1.f, 0.f);

	pVerts[2].vPos = D3DXVECTOR3(1.f, -1.f, 0.f);
	pVerts[2].vTexUV = D3DXVECTOR2(1.f, 1.f);

	pVerts[3].vPos = D3DXVECTOR3(-1.f, -1.f, 0.f);
	pVerts[3].vTexUV = D3DXVECTOR2(0.f, 1.f);

	m_pVB->Unlock();

	Engine::INDEX16* pIndices;
	m_pIB->Lock( 0, NULL, ( void** )&pIndices, 0 );

	pIndices[0]._0 = 0;
	pIndices[0]._1 = 1;
	pIndices[0]._2 = 2;

	pIndices[1]._0 = 0;
	pIndices[1]._1 = 2;
	pIndices[1]._2 = 3;

	m_pIB->Unlock();

	m_pDevice->CreateVertexBuffer(500 * sizeof(PARTICLE_INSTANCE), 0, 0, D3DPOOL_MANAGED, &m_pVBInstance, 0);

	PARTICLE_INSTANCE* pInst;

	MyMat matScale, matRotX, matRotY, matRotZ, matTrans, matWorld;

	m_pVBInstance->Lock( 0, NULL, ( void** )&pInst, 0 );

	for (int i = 0; i < 500; ++i)
	{
		/*int iRand = rand() % 3 + 3;
		D3DXMatrixScaling(&matScale, iRand, iRand, iRand);
		D3DXMatrixRotationX(&matRotX, D3DXToRadian(rand() % 360));
		D3DXMatrixRotationY(&matRotY, D3DXToRadian(rand() % 360));
		D3DXMatrixRotationZ(&matRotZ, D3DXToRadian(rand() % 360));
		MyVec3 vPos = *(MyVec3*)&m_pTarget->m[3][0] + MyVec3(Engine::GetRandomFloat(-30.f, 30.f), Engine::GetRandomFloat(2.f, 15.f), Engine::GetRandomFloat(-30.f, 30.f));
		D3DXMatrixTranslation(&matTrans, vPos.x, vPos.y, vPos.z);

		matWorld = matScale * matRotX * matRotY * matRotZ * matTrans;

		pInst[i].Color = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
		pInst[i].vRight = *(MyVec4*)&matWorld.m[0][0];
		pInst[i].vUp = *(MyVec4*)&matWorld.m[1][0];
		pInst[i].vLook = *(MyVec4*)&matWorld.m[2][0];
		pInst[i].vPos = *(MyVec4*)&matWorld.m[3][0];*/
	}

	m_pVBInstance->Unlock();
	
	return S_OK;
}

_int CParticleBuffer::Update( const _float& fDelta )
{
	m_fTime -= fDelta;
	if (m_fTime <= 0.f)
		m_bDestroy = true;

	/*m_pTransformCom->SetInfomation(Engine::CTransform::INFO_POS, (MyVec3*)&m_pTarget->m[3][0]);*/

	PARTICLE_INSTANCE* pInst;

	MyMat matScale, matRotX, matRotY, matRotZ, matTrans, matWorld;

	m_pVBInstance->Lock( 0, NULL, ( void** )&pInst, 0 );

	for (int i = 0; i < 500; ++i)
	{
		/*int iRand = 5.f;
		D3DXMatrixScaling(&matScale, iRand, iRand, iRand);
		D3DXMatrixRotationX(&matRotX, D3DXToRadian(rand() % 360));
		D3DXMatrixRotationY(&matRotY, D3DXToRadian(rand() % 360));
		D3DXMatrixRotationZ(&matRotZ, D3DXToRadian(rand() % 360));
		MyVec3 vPos = *(MyVec3*)&m_pTarget->m[3][0] + MyVec3(Engine::GetRandomFloat(-30.f, 30.f), Engine::GetRandomFloat(2.f, 15.f), Engine::GetRandomFloat(-30.f, 30.f));
		D3DXMatrixTranslation(&matTrans, vPos.x, vPos.y, vPos.z);

		matWorld = matScale * matRotX * matRotY * matRotZ * matTrans;

		pInst[i].Color = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
		pInst[i].vRight = *(MyVec4*)&matWorld.m[0][0];
		pInst[i].vUp = *(MyVec4*)&matWorld.m[1][0];
		pInst[i].vLook = *(MyVec4*)&matWorld.m[2][0];
		pInst[i].vPos = *(MyVec4*)&matWorld.m[3][0];*/
	}

	m_pVBInstance->Unlock();

	Engine::CGameObject::Update(fDelta);

	m_pRendererCom->AddRenderList(Engine::CRenderer::RENDER_ALPHA, this);

	return 0;
}

void CParticleBuffer::Render(void)
{	
	LPD3DXEFFECT pEffect = m_pShaderCom->GetEffect();
	if (pEffect == NULL)
		return;
	pEffect->AddRef();

	MyMat matView, matProj;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);


	m_pDevice->SetVertexDeclaration(m_pVertexDecl);

	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(PARTICLEVTX));
	m_pDevice->SetStreamSourceFreq(0, D3DSTREAMSOURCE_INDEXEDDATA | 500);

	m_pDevice->SetStreamSource(1, m_pVBInstance, 0, sizeof(PARTICLE_INSTANCE));
	m_pDevice->SetStreamSourceFreq(1, D3DSTREAMSOURCE_INSTANCEDATA| 1);
	m_pDevice->SetIndices(m_pIB);

	//m_pTransformCom->SetUpShader(pEffect);
	m_pTextureCom->SetUpShader(pEffect);
	
	pEffect->Begin(NULL, 0);
	pEffect->BeginPass(0);

	m_pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2 );

	pEffect->EndPass();
	pEffect->End();

	m_pDevice->SetStreamSourceFreq(0, 1);
	m_pDevice->SetStreamSourceFreq(1, 1);

	Engine::Safe_Release(pEffect);
}

HRESULT CParticleBuffer::Ready_Component( void )
{
	m_pRendererCom = (Engine::CRenderer*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STATIC, L"Component_Renderer");
	if (m_pRendererCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(Engine::CGameObject::UPDATE_NONE, L"Com_Renderer", m_pRendererCom)))
		return E_FAIL;

	m_pTransformCom = (Engine::CTransform*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STATIC, L"Component_Transform");
	if (m_pTransformCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(Engine::CGameObject::UPDATE_DO, L"Com_Transform", m_pTransformCom)))
		return E_FAIL;

	m_pTextureCom = (Engine::CTexture*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Texture_Ring_Outer_Wind");
	if (m_pTextureCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(Engine::CGameObject::UPDATE_NONE, L"Com_Texture", m_pTextureCom)))
		return E_FAIL;

	m_pShaderCom = (Engine::CShader*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Shader_Particle");
	if (m_pShaderCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(Engine::CGameObject::UPDATE_NONE, L"Com_Shader", m_pShaderCom)))

		return E_FAIL;
	return S_OK;
}

void CParticleBuffer::Free(void)
{
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pTextureCom);
	Engine::Safe_Release(m_pShaderCom);

	Engine::Safe_Release(m_pVertexDecl);
	Engine::Safe_Release(m_pVBInstance);
	Engine::Safe_Release(m_pVB);
	Engine::Safe_Release(m_pIB);

	Engine::CGameObject::Free();
}

CParticleBuffer* CParticleBuffer::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CParticleBuffer* pParticleBuffer = new CParticleBuffer(pDevice);

	if (FAILED(pParticleBuffer->Initialize()))
	{
		MSG_BOX("ParticleBuffer Created Failed");
		Engine::Safe_Release(pParticleBuffer);
	}

	return pParticleBuffer;
}
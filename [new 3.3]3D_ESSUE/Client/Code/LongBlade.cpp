#include "StdAfx.h"
#include "LongBlade.h"
#include "Player.h"

CLongBlade::CLongBlade( LPDIRECT3DDEVICE9 pDevice )
: Engine::CGameObject(pDevice)
, m_pRendererCom(NULL)
, m_pTransformCom(NULL)
, m_pMtrCom(NULL)
, m_pMeshCom(NULL)
, m_pCollisionCom(NULL)
, m_pShaderCom(NULL)
, m_pMatParent(NULL)
, m_pMatAttachment(NULL)
, m_pTargetCollision(NULL)

, m_pBlurShaderCom(NULL)
, m_pShaderBufferCom(NULL)
{

}

CLongBlade::~CLongBlade( void )
{

}

HRESULT CLongBlade::Initialize( void )
{
	if (FAILED(ReadyComponent()))
		return E_FAIL;

	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_POS, &MyVec3(-10.f, 0.f, 0.f));
	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_ANGLE, &MyVec3(0.f, D3DXToRadian(-90.f), 0.f));

	m_fTime = 0.6f;

	D3DVIEWPORT9	ViewPort;
	m_pDevice->GetViewport(&ViewPort);

	if(FAILED(D3DXCreateTexture(m_pDevice, ViewPort.Width, ViewPort.Height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pTargetTexture[0])))
		return E_FAIL;
	m_pTargetTexture[0]->GetSurfaceLevel(0, &m_pTargetSurface[0]);

	if(FAILED(D3DXCreateTexture(m_pDevice, ViewPort.Width, ViewPort.Height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pTargetTexture[1])))
		return E_FAIL;
	m_pTargetTexture[1]->GetSurfaceLevel(0, &m_pTargetSurface[1]);

	return S_OK;
}

_int CLongBlade::Update( const _float& fDelta )
{
	if (m_pMatParent == NULL)
	{
		m_pMatParent = ((Engine::CTransform*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(0)->GetComponent(L"Com_Transform"))->GetWorldMatrix();
		m_pMatAttachment = ((Engine::CDynamicMesh*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(0)->GetComponent(L"Com_Mesh"))->GetPartsMatrix("ValveBiped_Anim_Attachment_RH");
		m_pTransformCom->SetParentMatrix(&m_MatCompute);

		//m_pTargetCollision = (Engine::CCollisionBox*)Engine::Get_Management()->GetLayer(L"Layer_Monster")->GetObject(0)->GetComponent(L"Com_CollisionBox");
	}
	m_fTime -= fDelta;

	m_MatCompute = *m_pMatAttachment * *m_pMatParent;

	Engine::CGameObject::Update(fDelta);

	m_pRendererCom->AddRenderList(Engine::CRenderer::RENDER_NONEALPHA, this);

	//m_pCollisionCom->CollisionOBB(m_pTargetCollision);

	return 0;
}

void CLongBlade::Render( void )
{
	/*PDIRECT3DSURFACE9			pBackBuffer = NULL;

	m_pDevice->GetRenderTarget(0, &pBackBuffer);

	m_pDevice->SetRenderTarget(0, m_pTargetSurface[0]);
	m_pDevice->SetRenderTarget(1, m_pTargetSurface[1]);

	if (m_fTime < 0)
	{
		m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DXCOLOR(0.f, 0.f, 0.f, 0.f), 1.f, 0);
		m_fTime = 0.4f;
	}*/

	////
	LPD3DXEFFECT	pEffect = m_pShaderCom->GetEffect();
	if (pEffect == NULL)
		return;
	SetUpShader(pEffect);
	////

	/*m_pDevice->SetRenderTarget(0, pBackBuffer);

	LPD3DXEFFECT	pEffect2 = m_pBlurShaderCom->GetEffect();
	if(NULL == pEffect2)
		return;

	pEffect2->AddRef();

	pEffect2->SetTexture("g_BaseTexture", m_pTargetTexture[0]);
	pEffect2->SetTexture("g_BlurTexture", m_pTargetTexture[1]);  

	pEffect2->Begin(0, NULL);
	pEffect2->BeginPass(0);

	m_pShaderBufferCom->Render();

	pEffect2->EndPass();
	pEffect2->End();

	
	Engine::Safe_Release(pEffect2);*/
	m_pCollisionCom->Render(Engine::CCollisionBox::COLL_OBB, m_pTransformCom->GetWorldMatrix());
}

void CLongBlade::SetUpShader( LPD3DXEFFECT pEffect )
{
	pEffect->AddRef();

	D3DLIGHT9	LightInfo;
	ZeroMemory(&LightInfo, sizeof(D3DLIGHT9));
	m_pDevice->GetLight(0, &LightInfo);

	pEffect->SetVector("g_vLightDir", (MyVec4*)&LightInfo.Direction);
	pEffect->SetVector("g_vLightDiffuse", (MyVec4*)&LightInfo.Diffuse);
	pEffect->SetVector("g_vLightAmbient", (MyVec4*)&LightInfo.Ambient);
	pEffect->SetVector("g_vLightSpecular", (MyVec4*)&LightInfo.Specular);
	pEffect->SetVector("g_vCamPos", (MyVec4*)Engine::Get_CameraMgr()->GetCameraEye());

	MyMat matView, matProj;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	m_pTransformCom->SetUpShader(pEffect);
	m_pMtrCom->SetUpShader(pEffect);
	m_pMeshCom->SetUpShader(pEffect);

	Engine::Safe_Release(pEffect);
}

HRESULT CLongBlade::ReadyComponent( void )
{
	m_pRendererCom = (Engine::CRenderer*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STATIC, L"Component_Renderer");
	if (m_pRendererCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(CGameObject::UPDATE_NONE, L"Com_Renderer", m_pRendererCom)))
		return E_FAIL;

	m_pTransformCom = (Engine::CTransform*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STATIC, L"Component_Transform");
	if (m_pTransformCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(CGameObject::UPDATE_DO, L"Com_Transform", m_pTransformCom)))
		return E_FAIL;	

	m_pMtrCom = (Engine::CMaterial*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STATIC, L"Component_Material");
	if (m_pMtrCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(CGameObject::UPDATE_NONE, L"Com_Material", m_pMtrCom)))
		return E_FAIL;

	m_pMeshCom = (Engine::CStaticMesh*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Mesh_LongBlade");
	if (m_pMeshCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(CGameObject::UPDATE_NONE, L"Com_Mesh", m_pMeshCom)))
		return E_FAIL;

	m_pCollisionCom = (Engine::CCollisionBox*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STATIC, L"Component_CollisionBox");
	if (m_pCollisionCom == NULL)
		return E_FAIL;
	if (FAILED(m_pCollisionCom->ComputeBox(m_pMeshCom)))
		return E_FAIL;
	if (FAILED(AddComponent(CGameObject::UPDATE_NONE, L"Com_CollisionBox", m_pCollisionCom)))
		return E_FAIL;

	m_pShaderCom = (Engine::CShader*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Shader_Mesh");
	if (m_pShaderCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(Engine::CGameObject::UPDATE_NONE, L"Com_Shader", m_pShaderCom)))
		return E_FAIL;

	m_pBlurShaderCom = (Engine::CShader*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Shader_Blur");
	if (m_pBlurShaderCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(Engine::CGameObject::UPDATE_NONE, L"Com_BlurShader", m_pBlurShaderCom)))
		return E_FAIL;

	m_pShaderBufferCom = (Engine::CShaderBuffer*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Buffer_Shader");
	if (m_pShaderBufferCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(Engine::CGameObject::UPDATE_NONE, L"Com_ShaderBuffer", m_pShaderBufferCom)))
		return E_FAIL;

	return S_OK;
}

void CLongBlade::Free( void )
{
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pMtrCom);
	Engine::Safe_Release(m_pMeshCom);
	Engine::Safe_Release(m_pCollisionCom);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pBlurShaderCom);
	Engine::Safe_Release(m_pShaderBufferCom);

	for (_uint i = 0; i < 2; ++i)
	{
		Engine::Safe_Release(m_pTargetTexture[i]);
		Engine::Safe_Release(m_pTargetSurface[i]);
	}
	

	Engine::CGameObject::Free();
}

CLongBlade* CLongBlade::Create( LPDIRECT3DDEVICE9 pDevice )
{
	CLongBlade* pBlade = new CLongBlade(pDevice);

	if (FAILED(pBlade->Initialize()))
	{
		MSG_BOX("LongBlade Created Failed");
		Engine::Safe_Release(pBlade);
	}
	return pBlade;
}
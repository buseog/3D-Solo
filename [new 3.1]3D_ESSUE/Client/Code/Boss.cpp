#include "StdAfx.h"
#include "Boss.h"

CBoss::CBoss( LPDIRECT3DDEVICE9 pDevice )
: Engine::CGameObject(pDevice)
, m_pRendererCom(NULL)
, m_pTransformCom(NULL)
, m_pMtrCom(NULL)
, m_pMeshCom(NULL)
, m_pCollisionCom(NULL)
, m_iStateID(Idle)
, m_pMatPlayer(NULL)
{

}

CBoss::~CBoss( void )
{

}

HRESULT CBoss::Initialize( void )
{
	if (FAILED(ReadyComponent()))
		return E_FAIL;

	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_SCALE, &Engine::MyVec3(0.1f, 0.1f, 0.1f));
	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_POS, &Engine::MyVec3(0.f, 5.f, 0.f));
	m_pMeshCom->SetAnimation(Idle);

	m_fSpeed = 15.f;
	m_fPatternTime = 5.f;

	return S_OK;
}

_int CBoss::Update( const _float& fDelta )
{
	if (m_pMatPlayer == NULL)
		m_pMatPlayer = ((Engine::CTransform*)Engine::Get_Management()->GetLayer(L"Layer_Player")->GetObject(0)->GetComponent(L"Com_Transform"))->GetWorldMatrix();

	/*m_fPatternTime -= fDelta;
	if (m_fPatternTime <= 0)
	{
		m_fPatternTime = _float(rand() % 7 + 5);
		m_iStateID = rand() % 5 + 1;
		m_pMeshCom->SetAnimation(m_iStateID);
	}*/

	if (Engine::Get_KeyMgr()->KeyDown('8'))
	{
		m_iStateID = Dash;
		m_pMeshCom->SetAnimation(m_iStateID);
	}
	if (Engine::Get_KeyMgr()->KeyDown('9'))
	{
		m_iStateID = ShockWave;
		m_pMeshCom->SetAnimation(m_iStateID);
	}
	if (Engine::Get_KeyMgr()->KeyDown('0'))
	{
		m_iStateID = TripleAttack;
		m_pMeshCom->SetAnimation(m_iStateID);
	}

	Pattern(fDelta);

	Engine::CGameObject::Update(fDelta);

	m_pMeshCom->PlayAnimation(fDelta);
	AnimationCheck();
	
	
	m_pRendererCom->AddRenderList(Engine::CRenderer::RENDER_NONEALPHA, this);

	return 0;
}

_int CBoss::LastUpdate( const _float& fDelta )
{
	
	return 0;	
}

void CBoss::Render( void )
{
	LPD3DXEFFECT	pEffect = m_pShaderCom->GetEffect();
	if (pEffect == NULL)
		return;
	SetUpShader(pEffect);
	m_pCollisionCom->Render(Engine::CCollisionBox::COLL_OBB, m_pTransformCom->GetWorldMatrix());
}
void CBoss::SetUpShader( LPD3DXEFFECT pEffect )
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
	m_pMeshCom->SetUpShader(pEffect, 3);

	Engine::Safe_Release(pEffect);
}

void CBoss::AnimationCheck( void )
{
	if (m_pMeshCom->CheckAnimation() == true)
	{
		m_iStateID = Idle;
		m_pMeshCom->SetAnimation(m_iStateID);
	}
}

void CBoss::Pattern( const _float& fDelta )
{
	switch (m_iStateID)
	{
	case StepFront:
		StepPattern(fDelta);
		break;
	case Run:
		RunPattern(fDelta);
		break;
	case Dash:
		DashPattern(fDelta);
		break;
	case ShockWave:
		ShockWavePattern(fDelta);
		break;
	case TripleAttack:
		TripleAttackPattern(fDelta);
		break;

	}
}

void CBoss::StepPattern( const _float& fDelta )
{
	/*MyVec3 vTarget = *(MyVec3*)&m_pMatPlayer->m[3][0];
	MyVec3 vPos = *(MyVec3*)&m_pTransformCom->GetWorldMatrix()->m[3][0];
	MyVec3 vMove = vTarget - vPos;
	vPos += *D3DXVec3Normalize(&vMove, &vMove) * m_fSpeed * fDelta;

	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_POS, &vPos);*/
}

void CBoss::RunPattern( const _float& fDelta )
{

}

void CBoss::DashPattern( const _float& fDelta )
{
	MyVec3 vLook = *(MyVec3*)&m_pTransformCom->GetWorldMatrix()->m[2][0];
	D3DXVec3Normalize(&vLook, &vLook);

	if (m_pMeshCom->ProgressAnimation() > 5.3 && m_pMeshCom->ProgressAnimation() < 5.5)
		m_pTransformCom->Straight((Engine::MyVec3*)&vLook, m_fSpeed * fDelta * 7.f);

	else if (m_pMeshCom->ProgressAnimation() > 5.5)
		m_pTransformCom->Straight((Engine::MyVec3*)&vLook, m_fSpeed * fDelta * 5.f);
}

void CBoss::ShockWavePattern( const _float& fDelta )
{
	MyVec3 vLook = *(MyVec3*)&m_pTransformCom->GetWorldMatrix()->m[2][0];
	D3DXVec3Normalize(&vLook, &vLook);

	if (m_pMeshCom->ProgressAnimation() > 0.2 && m_pMeshCom->ProgressAnimation() < 3.0)
		m_pTransformCom->Straight((Engine::MyVec3*)&vLook, -m_fSpeed * fDelta * 0.7f);

	else if (m_pMeshCom->ProgressAnimation() > 3.0 && m_pMeshCom->ProgressAnimation() < 3.5)
		m_pTransformCom->Straight((Engine::MyVec3*)&vLook, -m_fSpeed * fDelta * 1.2f);

	else if (m_pMeshCom->ProgressAnimation() > 3.5 && m_pMeshCom->ProgressAnimation() < 4.0)
		m_pTransformCom->Straight((Engine::MyVec3*)&vLook, -m_fSpeed * fDelta * 0.3f);

	else if (m_pMeshCom->ProgressAnimation() > 4.0 && m_pMeshCom->ProgressAnimation() < 5.3)
		m_pTransformCom->Straight((Engine::MyVec3*)&vLook, -m_fSpeed * fDelta * 0.35f);
}

void CBoss::TripleAttackPattern( const _float& fDelta )
{
	MyVec3 vLook = *(MyVec3*)&m_pTransformCom->GetWorldMatrix()->m[2][0];
	D3DXVec3Normalize(&vLook, &vLook);

	if (m_pMeshCom->ProgressAnimation() > 0.7 && m_pMeshCom->ProgressAnimation() < 2.0)
		m_pTransformCom->Straight((Engine::MyVec3*)&vLook, m_fSpeed * fDelta * 1.3f);

	else if (m_pMeshCom->ProgressAnimation() > 2.0 && m_pMeshCom->ProgressAnimation() < 3.6)
		m_pTransformCom->Straight((Engine::MyVec3*)&vLook, m_fSpeed * fDelta * 1.7f);

	else if (m_pMeshCom->ProgressAnimation() > 3.7 && m_pMeshCom->ProgressAnimation() < 4.7)
		m_pTransformCom->Straight((Engine::MyVec3*)&vLook, m_fSpeed * fDelta * 1.5f);

	else if (m_pMeshCom->ProgressAnimation() > 8.7 && m_pMeshCom->ProgressAnimation() < 10.0)
		m_pTransformCom->Straight((Engine::MyVec3*)&vLook, m_fSpeed * fDelta * 0.3f);
}


HRESULT CBoss::ReadyComponent( void )
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

	m_pMeshCom = (Engine::CDynamicMesh*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Mesh_Boss");
	if (m_pMeshCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(CGameObject::UPDATE_NONE, L"Com_Mesh", m_pMeshCom)))
		return E_FAIL;

	m_pCollisionCom = (Engine::CCollisionBox*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STATIC, L"Component_CollisionBox");
	if (m_pCollisionCom == NULL)
		return E_FAIL;
	if (FAILED(m_pCollisionCom->ComputeBox(m_pMeshCom, 0)))
		return E_FAIL;
	if (FAILED(AddComponent(CGameObject::UPDATE_NONE, L"Com_CollisionBox", m_pCollisionCom)))
		return E_FAIL;

	m_pShaderCom = (Engine::CShader*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Shader_Mesh");
	if (m_pShaderCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(Engine::CGameObject::UPDATE_NONE, L"Com_Shader", m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CBoss::Free( void )
{
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pMtrCom);
	Engine::Safe_Release(m_pMeshCom);
	Engine::Safe_Release(m_pCollisionCom);
	Engine::Safe_Release(m_pShaderCom);

	Engine::CGameObject::Free();
}

CBoss* CBoss::Create( LPDIRECT3DDEVICE9 pDevice )
{
	CBoss* pPlayer = new CBoss(pDevice);

	if (FAILED(pPlayer->Initialize()))
	{
		MSG_BOX("Player Created Failed");
		Engine::Safe_Release(pPlayer);
	}
	return pPlayer;
}

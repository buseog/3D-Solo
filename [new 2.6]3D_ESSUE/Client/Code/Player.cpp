#include "StdAfx.h"
#include "Player.h"


CPlayer::CPlayer( LPDIRECT3DDEVICE9 pDevice )
: Engine::CGameObject(pDevice)
, m_pKeyMgr(Engine::Get_KeyMgr())
, m_pRendererCom(NULL)
, m_pTransformCom(NULL)
, m_pMtrCom(NULL)
, m_pMeshCom(NULL)
, m_pCollisionCom(NULL)
, m_pShaderCom(NULL)
, m_iStateID(BattleIdle)
, m_iAttackCnt(Attack01)
{

}

CPlayer::~CPlayer( void )
{

}

HRESULT CPlayer::Initialize( void )
{
	if (FAILED(ReadyComponent()))
		return E_FAIL;

	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_SCALE, &Engine::MyVec3(0.1f, 0.1f, 0.1f));
	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_POS, &Engine::MyVec3(0.f, 5.f, 0.f));
	m_pMeshCom->SetAnimation(0);

	m_fSpeed = 20.f;

	return S_OK;
}

_int CPlayer::Update( const _float& fDelta )
{
	KeyCheck(fDelta);

	Engine::CGameObject::Update(fDelta);

	m_pMeshCom->PlayAnimation(fDelta);
	AnimationCheck();

	m_pRendererCom->AddRenderList(Engine::CRenderer::RENDER_NONEALPHA, this);

	return 0;
}

void CPlayer::Render( void )
{
	LPD3DXEFFECT	pEffect = m_pShaderCom->GetEffect();
	if (pEffect == NULL)
		return;

	SetUpShader(pEffect);

	m_pCollisionCom->Render(Engine::CCollisionBox::COLL_OBB, m_pTransformCom->GetWorldMatrix());
}

void CPlayer::SetUpShader( LPD3DXEFFECT pEffect )
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

void CPlayer::KeyCheck( const _float& fDelta )
{
	if (fDelta == 0.f)
		return;

	if (m_pKeyMgr->KeyDown('1') && m_iStateID <= Attack01)
	{
		m_iStateID = LordOfMana;
		m_pMeshCom->SetAnimation(m_iStateID);
	}

	if (m_pKeyMgr->KeyDown('2') && m_iStateID <= SharpenCombo)
	{
		m_iStateID = SharpenCombo;
		m_pMeshCom->SetAnimation(m_iStateID);
		m_vPrevPos = *(MyVec3*)&m_pTransformCom->GetWorldMatrix()->m[3][0];
	}
	if (m_iStateID == SharpenCombo)
	{
		MyVec3 vMove = *(MyVec3*)&m_pMeshCom->GetPartsMatrix("ValveBiped_Anim_Attachment_GRAPPLING")->m[3][0];
		MyMat matScale, matRot, matTrans;
		D3DXMatrixScaling(&matScale, 0.1f, 0.1f, 0.1f);
		D3DXMatrixRotationY(&matRot, m_pTransformCom->GetInfomation(Engine::CTransform::INFO_ANGLE)->y);
		D3DXVec3TransformCoord(&vMove, &vMove, &(matScale * matRot));
		vMove.x *= -1.f;
		vMove.z *= -1.f;

		m_pTransformCom->SetInfomation(Engine::CTransform::INFO_POS, &(m_vPrevPos + vMove));
	}

	if (m_pKeyMgr->KeyDown('3') && m_iStateID <= Attack01)
	{
		m_iStateID = DarkNight;
		m_pMeshCom->SetAnimation(m_iStateID);
		m_vPrevPos = *(MyVec3*)&m_pTransformCom->GetWorldMatrix()->m[3][0];
		Engine::Get_CameraMgr()->SetCamera(L"Cam_Action");
	}

	if (m_pKeyMgr->KeyDown(VK_SPACE))
	{
		m_iStateID = Teleport;
		m_pMeshCom->SetAnimation(m_iStateID, 0.7f, 1.1);
	}
	if (m_iStateID == Teleport)
	{
		MyVec3 vPos = *(MyVec3*)&m_pTransformCom->GetWorldMatrix()->m[3][0];
		MyVec3 vDir = *(MyVec3*)&m_pTransformCom->GetWorldMatrix()->m[2][0];
		m_pTransformCom->SetInfomation(Engine::CTransform::INFO_POS, &(vPos + vDir * m_fSpeed * fDelta * 50.f));
	}

	if (m_pKeyMgr->StayKeyDown('W') && m_iStateID < Teleport)
	{
		m_iStateID = RunLower;
		m_pMeshCom->SetAnimation(m_iStateID);
		m_pTransformCom->Move(m_fSpeed * fDelta);		
	}
	else if (m_pKeyMgr->StayKeyDown('S') && m_iStateID < Teleport)
	{
		m_iStateID = RunLower;
		m_pMeshCom->SetAnimation(m_iStateID);
		m_pTransformCom->Move(-m_fSpeed * fDelta);
	}
	else if (m_pKeyMgr->StayKeyDown('A') && m_iStateID < Teleport)
	{
		m_iStateID = RunLower;
		m_pMeshCom->SetAnimation(m_iStateID);
		m_pTransformCom->SideMove(-m_fSpeed * fDelta);
	}
	else if (m_pKeyMgr->StayKeyDown('D') && m_iStateID < Teleport)
	{
		m_iStateID = RunLower;
		m_pMeshCom->SetAnimation(m_iStateID);
		m_pTransformCom->SideMove(m_fSpeed * fDelta);
	}

	else if (m_iStateID == RunLower)
	{
		m_iStateID = BattleIdle;
		m_pMeshCom->SetAnimation(m_iStateID);
	}

	if (m_pKeyMgr->KeyDown(VK_LBUTTON))
	{
		if (m_iStateID != BattleIdle &&m_pMeshCom->ProgressAnimation() < 0.3)
			return;

		if ((m_iStateID >= Attack01 && m_iStateID <= Attack05) == false)
			m_iStateID = Attack01;

		m_pMeshCom->SetAnimation(m_iStateID);
		++m_iStateID;

		if (m_iStateID > Attack05)
			m_iStateID = BattleIdle;
	}
}

void CPlayer::AnimationCheck( void )
{
	if (m_pMeshCom->CheckAnimation() == true)
	{
		if (m_iStateID == RunLower)
			return;

		m_iStateID = BattleIdle;
		m_pMeshCom->SetAnimation(BattleIdle);
	}
}

HRESULT CPlayer::ReadyComponent( void )
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

	m_pMeshCom = (Engine::CDynamicMesh*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Mesh_Player");
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

void CPlayer::Free( void )
{
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pMtrCom);
	Engine::Safe_Release(m_pMeshCom);
	Engine::Safe_Release(m_pCollisionCom);
	Engine::Safe_Release(m_pShaderCom);

	Engine::CGameObject::Free();
}

CPlayer* CPlayer::Create( LPDIRECT3DDEVICE9 pDevice )
{
	CPlayer* pPlayer = new CPlayer(pDevice);

	if (FAILED(pPlayer->Initialize()))
	{
		MSG_BOX("Player Created Failed");
		Engine::Safe_Release(pPlayer);
	}
	return pPlayer;
}
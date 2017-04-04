#include "StdAfx.h"
#include "ActionCamera.h"
#include "Player.h"

CActionCamera::CActionCamera( LPDIRECT3DDEVICE9 pDevice )
: CCamera(pDevice)
, m_pPlayer(NULL)
, m_pBoneEye(NULL)
, m_pBoneAt(NULL)
{

}

CActionCamera::~CActionCamera( void )
{

}

HRESULT CActionCamera::Initialize(CPlayer* pPlayer, const MyVec3* pUp, _float fFovy, _float fAspect, _float fNear, _float fFar )
{
	m_pPlayer = pPlayer;
	m_pBoneEye = ((Engine::CDynamicMesh*)m_pPlayer->GetComponent(L"Com_Mesh"))->GetPartsMatrix("ValveBiped_Anim_Attachment_CAM_Origin");
	m_pBoneAt = ((Engine::CDynamicMesh*)m_pPlayer->GetComponent(L"Com_Mesh"))->GetPartsMatrix("ValveBiped_Anim_Attachment_CAM");


	m_vUp = MyVec3(0.f, 1.f, 0.f);

	m_fFovy = fFovy;
	m_fAspect = fAspect;
	m_fNear = fNear;
	m_fFar = fFar;

	if(FAILED(Engine::CCamera::Initialize()))
		return E_FAIL;

	return S_OK;
}

_int CActionCamera::Update( const _float& fDelta )
{
	Engine::CTransform* pTrans = (Engine::CTransform*)m_pPlayer->GetComponent(L"Com_Transform");

	MyMat matRot;
	D3DXMatrixRotationY(&matRot, D3DXToRadian(-90.f));
	

	D3DXVec3TransformCoord(&m_vEye, (Engine::MyVec3*)&m_pBoneEye->m[3][0], &(matRot * *pTrans->GetWorldMatrix()));
	D3DXVec3TransformCoord(&m_vAt, (Engine::MyVec3*)&m_pBoneAt->m[3][0], pTrans->GetWorldMatrix());

	_int iExit = Engine::CCamera::Update(fDelta);

	return 0;
}

void CActionCamera::Free( void )
{
	Engine::CCamera::Free();
}

CActionCamera* CActionCamera::Create( LPDIRECT3DDEVICE9 pDevice, CPlayer* pPlayer, const MyVec3* pUp, _float fFovy /*= D3DXToRadian(60.0f)*/, _float fAspect /*= _float(WINCX) / WINCY*/, _float fNear /*= 0.1f*/, _float fFar /*= 1000.f*/ )
{
	CActionCamera* pCamera = new CActionCamera(pDevice);

	if (FAILED(pCamera->Initialize(pPlayer, pUp, fFovy, fAspect, fNear, fFar)))
	{
		MSG_BOX("DynamicCamera Created Failed");
		::Safe_Release(pCamera);
	}

	return pCamera;
}
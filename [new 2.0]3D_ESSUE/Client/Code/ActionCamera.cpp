#include "StdAfx.h"
#include "ActionCamera.h"
#include "Player.h"

CActionCamera::CActionCamera( LPDIRECT3DDEVICE9 pDevice )
: CCamera(pDevice)
, m_pPlayer(NULL)
, m_pBoneEye(NULL)
, m_pBoneAt(NULL)
, m_pBoneOrigin(NULL)
{

}

CActionCamera::~CActionCamera( void )
{

}

HRESULT CActionCamera::Initialize(CPlayer* pPlayer, Engine::CDynamicMesh* pCamMesh, const MyVec3* pUp, _float fFovy, _float fAspect, _float fNear, _float fFar )
{
	m_pPlayer = pPlayer;
	m_pCamMesh = pCamMesh;
	m_pBoneEye = m_pCamMesh->GetPartsMatrix("ValveBiped_Anim_Attachment_CAM");
	m_pBoneAt = m_pCamMesh->GetPartsMatrix("ValveBiped_Anim_Attachment_EyeTarget");

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
	MyMat matParent = *pTrans->GetWorldMatrix();

	memcpy(&m_vEye, &m_pBoneEye->m[3][0], sizeof(MyVec3));
	memcpy(&m_vAt, &m_pBoneAt->m[3][0], sizeof(MyVec3));

	MyMat matTrans, matWorld;
	D3DXMatrixTranslation(&matTrans, 0.f, 0.f, -150.f);

	matWorld = matTrans * matParent;

	D3DXVec3TransformCoord(&m_vEye, &m_vEye, &matWorld);
	D3DXVec3TransformCoord(&m_vAt, &m_vAt, &matParent);

	_int iExit = Engine::CCamera::Update(fDelta);

	return iExit;
}

void CActionCamera::Free( void )
{
	Engine::CCamera::Free();
}

CActionCamera* CActionCamera::Create( LPDIRECT3DDEVICE9 pDevice, CPlayer* pPlayer, Engine::CDynamicMesh* pCamMesh, const MyVec3* pUp, _float fFovy /*= D3DXToRadian(60.0f)*/, _float fAspect /*= _float(WINCX) / WINCY*/, _float fNear /*= 0.1f*/, _float fFar /*= 1000.f*/ )
{
	CActionCamera* pCamera = new CActionCamera(pDevice);

	if (FAILED(pCamera->Initialize(pPlayer, pCamMesh, pUp, fFovy, fAspect, fNear, fFar)))
	{
		MSG_BOX("DynamicCamera Created Failed");
		::Safe_Release(pCamera);
	}

	return pCamera;
}
#include "StdAfx.h"
#include "IntroCamera.h"
#include "Boss.h"

CIntroCamera::CIntroCamera( LPDIRECT3DDEVICE9 pDevice )
: CCamera(pDevice)
, m_pBoss(NULL)
, m_pBoneEye(NULL)
, m_pMesh(NULL)
{

}

CIntroCamera::~CIntroCamera( void )
{

}

HRESULT CIntroCamera::Initialize(CBoss* pBoss, const MyVec3* pUp, _float fFovy, _float fAspect, _float fNear, _float fFar )
{
	m_pMesh = (Engine::CDynamicMesh*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STAGE, L"Component_Mesh_Intro");
	m_pBoneEye = m_pMesh->GetPartsMatrix("attach_cat");
	m_pMesh->SetAnimation(0); 

	m_pBoss = pBoss;

	m_vUp = MyVec3(0.f, 1.f, 0.f);

	m_fFovy = fFovy;
	m_fAspect = fAspect;
	m_fNear = fNear;
	m_fFar = fFar;

	if(FAILED(Engine::CCamera::Initialize()))
		return E_FAIL;

	return S_OK;
}

_int CIntroCamera::Update( const _float& fDelta )
{
	MyMat matScale, matTrans;
	D3DXMatrixScaling(&matScale, 0.1f, 0.1f, 0.1f);
	MyMat matWorld = *m_pBoneEye * matScale;
	MyVec3 vRight = *(Engine::MyVec3*)&matWorld.m[0][0];
	MyMat matAxis;
	D3DXMatrixRotationAxis(&matAxis, D3DXVec3Normalize(&vRight, &vRight), D3DXToRadian(-90.f));

	m_vEye = *(Engine::MyVec3*)&matWorld.m[3][0];
	m_vAt = m_vEye + *D3DXVec3TransformNormal((Engine::MyVec3*)&matWorld.m[2][0], (Engine::MyVec3*)&matWorld.m[2][0], &matAxis);

	_int iExit = Engine::CCamera::Update(fDelta);
	m_pMesh->PlayAnimation(fDelta);

	return 0;
}

void CIntroCamera::Free( void )
{
	Engine::CCamera::Free();

	Engine::Safe_Release(m_pMesh);
}

CIntroCamera* CIntroCamera::Create( LPDIRECT3DDEVICE9 pDevice, CBoss* pBoss, const MyVec3* pUp, _float fFovy /*= D3DXToRadian(60.0f)*/, _float fAspect /*= _float(WINCX) / WINCY*/, _float fNear /*= 0.1f*/, _float fFar /*= 1000.f*/ )
{
	CIntroCamera* pCamera = new CIntroCamera(pDevice);

	if (FAILED(pCamera->Initialize(pBoss, pUp, fFovy, fAspect, fNear, fFar)))
	{
		MSG_BOX("DynamicCamera Created Failed");
		::Safe_Release(pCamera);
	}

	return pCamera;
}
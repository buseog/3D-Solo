#include "StdAfx.h"
#include "StopCamera.h"

CStopCamera::CStopCamera( LPDIRECT3DDEVICE9 pDevice )
: CCamera(pDevice)
{

}

CStopCamera::~CStopCamera( void )
{

}

HRESULT CStopCamera::Initialize( const MyVec3* pEye, const MyVec3* pAt, const MyVec3* pUp, _float fFovy, _float fAspect, _float fNear, _float fFar )
{
	m_vEye = *pEye;
	m_vAt = *pAt;
	m_vUp = *pUp;

	m_fFovy = fFovy;
	m_fAspect = fAspect;
	m_fNear = fNear;
	m_fFar = fFar;
	m_fSpeed = 20.f;

	if(FAILED(Engine::CCamera::Initialize()))
		return E_FAIL;

	return S_OK;
}

_int CStopCamera::Update( const _float& fDelta )
{
	POINT			ptCenter = { WINCX >> 1, WINCY >> 1 };

	ClientToScreen(g_hWnd, &ptCenter);

	SetCursorPos(ptCenter.x, ptCenter.y);

	KeyCheck(fDelta);

	_int iExit = Engine::CCamera::Update(fDelta);

	return iExit;
}

void CStopCamera::KeyCheck( const _float& fDelta )
{
	MyMat			matWorld;
	D3DXMatrixInverse(&matWorld, NULL, &m_matView);


	if (Engine::Get_KeyMgr()->StayKeyDown('W'))
	{
		MyVec3 vLook;
		memcpy(&vLook, &matWorld.m[2][0], sizeof(MyVec3));
		D3DXVec3Normalize(&vLook, &vLook);

		m_vEye += vLook * m_fSpeed * fDelta; 
		m_vAt += vLook * m_fSpeed * fDelta;
	}

	if (Engine::Get_KeyMgr()->StayKeyDown('S'))
	{
		MyVec3 vLook;
		memcpy(&vLook, &matWorld.m[2][0], sizeof(MyVec3));
		D3DXVec3Normalize(&vLook, &vLook);

		m_vEye -= vLook * m_fSpeed * fDelta;
		m_vAt -= vLook * m_fSpeed * fDelta;
	}

	if (Engine::Get_KeyMgr()->StayKeyDown('A'))
	{
		MyVec3 vRight;
		memcpy(&vRight, &matWorld.m[0][0], sizeof(MyVec3));
		D3DXVec3Normalize(&vRight, &vRight);

		m_vEye -= vRight * m_fSpeed * fDelta;
		m_vAt -= vRight * m_fSpeed * fDelta;
	}

	if (Engine::Get_KeyMgr()->StayKeyDown('D'))
	{
		MyVec3 vRight;
		memcpy(&vRight, &matWorld.m[0][0], sizeof(MyVec3));
		D3DXVec3Normalize(&vRight, &vRight);

		m_vEye += vRight * m_fSpeed * fDelta;
		m_vAt += vRight * m_fSpeed * fDelta;
	}

	_long	dwMouseMove = 0;

	if(dwMouseMove = Engine::GetDIMouseMove(Engine::CInputDevice::DIMS_Y))
	{
		MyVec3			vRight;
		memcpy(&vRight, &matWorld.m[0][0], sizeof(MyVec3));

		MyVec3			vLook = m_vAt - m_vEye;	


		MyMat			matRot;
		D3DXMatrixRotationAxis(&matRot, &vRight, D3DXToRadian(dwMouseMove / 10.0f));		

		D3DXVec3TransformNormal(&vLook, &vLook, &matRot);

		m_vAt = m_vEye + vLook;
	}

	if(dwMouseMove = Engine::GetDIMouseMove(Engine::CInputDevice::DIMS_X))
	{
		MyVec3			vAxis = MyVec3(0.f, 1.f, 0.f);

		MyVec3			vLook = m_vAt - m_vEye;

		MyMat			matRot;

		D3DXMatrixRotationAxis(&matRot, &vAxis, D3DXToRadian(dwMouseMove / 10.0f));		

		D3DXVec3TransformNormal(&vLook, &vLook, &matRot);

		m_vAt = m_vEye + vLook;
	}
}

void CStopCamera::Free( void )
{
	Engine::CCamera::Free();
}

CStopCamera* CStopCamera::Create( LPDIRECT3DDEVICE9 pDevice, const MyVec3* pEye, const MyVec3* pAt, const MyVec3* pUp, _float fFovy /*= D3DXToRadian(60.0f)*/, _float fAspect /*= _float(WINCX) / WINCY*/, _float fNear /*= 0.1f*/, _float fFar /*= 1000.f*/ )
{
	CStopCamera* pCamera = new CStopCamera(pDevice);

	if (FAILED(pCamera->Initialize(pEye, pAt, pUp, fFovy, fAspect, fNear, fFar)))
	{
		MSG_BOX("DynamicCamera Created Failed");
		::Safe_Release(pCamera);
	}

	return pCamera;
}

void CStopCamera::SetCameraSetting( void )
{
	Engine::CCamera* pDefaultCam = Engine::Get_CameraMgr()->GetCamera(L"Cam_Default");

	m_vEye = *pDefaultCam->GetEye();
	m_vAt = *pDefaultCam->GetAt();
	m_vUp = *pDefaultCam->GetUp();
}

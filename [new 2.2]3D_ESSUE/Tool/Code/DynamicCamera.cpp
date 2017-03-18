#include "StdAfx.h"
#include "DynamicCamera.h"

CDynamicCamera::CDynamicCamera( LPDIRECT3DDEVICE9 pDevice )
: CCamera(pDevice)
, m_iTick(0)
, m_bHold(true)
{

}

CDynamicCamera::~CDynamicCamera( void )
{

}

HRESULT CDynamicCamera::Initialize( const MyVec3* pEye, const MyVec3* pAt, const MyVec3* pUp, _float fFovy, _float fAspect, _float fNear, _float fFar )
{
	m_vEye = *pEye;
	m_vAt = *pAt;
	m_vUp = *pUp;

	m_fFovy = fFovy;
	m_fAspect = fAspect;
	m_fNear = fNear;
	m_fFar = fFar;

	if(FAILED(Engine::CCamera::Initialize()))
		return E_FAIL;

	return S_OK;
}

_int CDynamicCamera::Update(const _uint& iMessage, const _float& fDelta )
{
	KeyCheck(iMessage, fDelta);

	_int iExit = Engine::CCamera::Update(fDelta);

	return iExit;
}

void CDynamicCamera::KeyCheck(const _uint& iMessage, const _float& fDelta )
{
	MyMat			matWorld;
	D3DXMatrixInverse(&matWorld, NULL, &m_matView);

	if (Engine::Get_KeyMgr()->StayKeyDown(VK_SPACE))
	{
		if (m_bHold == true)
			m_bHold = false;
		else
			m_bHold = true;
	}


	if (iMessage == 0)
	{
		if (Engine::Get_KeyMgr()->StayKeyDown('W'))
		{
			MyVec3 vLook;
			memcpy(&vLook, &matWorld.m[2][0], sizeof(MyVec3));
			D3DXVec3Normalize(&vLook, &vLook);

			m_vEye += vLook * 20 * fDelta; 
			m_vAt += vLook * 20 * fDelta;
		}

		if (Engine::Get_KeyMgr()->StayKeyDown('S'))
		{
			MyVec3 vLook;
			memcpy(&vLook, &matWorld.m[2][0], sizeof(MyVec3));
			D3DXVec3Normalize(&vLook, &vLook);

			m_vEye -= vLook * 20 * fDelta;
			m_vAt -= vLook * 20 * fDelta;
		}

		if (Engine::Get_KeyMgr()->StayKeyDown('A'))
		{
			MyVec3 vRight;
			memcpy(&vRight, &matWorld.m[0][0], sizeof(MyVec3));
			D3DXVec3Normalize(&vRight, &vRight);

			m_vEye -= vRight * 20 * fDelta;
			m_vAt -= vRight * 20 * fDelta;
		}

		if (Engine::Get_KeyMgr()->StayKeyDown('D'))
		{
			MyVec3 vRight;
			memcpy(&vRight, &matWorld.m[0][0], sizeof(MyVec3));
			D3DXVec3Normalize(&vRight, &vRight);

			m_vEye += vRight * 20 * fDelta;
			m_vAt += vRight * 20 * fDelta;
		}
	}
	else if (iMessage == 1 && m_bHold == true)
	{
		_long	dwMouseMove = 0;
		++m_iTick;

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

		if (m_iTick > 5)
		{
			m_iTick = 0;

			POINT			ptCenter = { WINCX >> 1, WINCY >> 1 };
			ClientToScreen(g_hWnd, &ptCenter);
			SetCursorPos(ptCenter.x, ptCenter.y);
		}
	}
}

void CDynamicCamera::Free( void )
{
	Engine::CCamera::Free();
}

CDynamicCamera* CDynamicCamera::Create( LPDIRECT3DDEVICE9 pDevice, const MyVec3* pEye, const MyVec3* pAt, const MyVec3* pUp, _float fFovy /*= D3DXToRadian(60.0f)*/, _float fAspect /*= _float(WINCX) / WINCY*/, _float fNear /*= 0.1f*/, _float fFar /*= 1000.f*/ )
{
	CDynamicCamera* pCamera = new CDynamicCamera(pDevice);

	if (FAILED(pCamera->Initialize(pEye, pAt, pUp, fFovy, fAspect, fNear, fFar)))
	{
		MSG_BOX("DynamicCamera Created Failed");
		Engine::Safe_Release(pCamera);
	}

	return pCamera;
}

void CDynamicCamera::SetCameraSetting( void )
{
	Engine::CCamera* pDefaultCam = Engine::Get_CameraMgr()->GetCamera(L"Cam_Default");

	m_vEye = *pDefaultCam->GetEye();
	m_vAt = *pDefaultCam->GetAt();
	m_vUp = *pDefaultCam->GetUp();
}

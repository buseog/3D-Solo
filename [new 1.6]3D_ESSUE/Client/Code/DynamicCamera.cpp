#include "StdAfx.h"
#include "DynamicCamera.h"

#include "Export_Engine.h"

CDynamicCamera::CDynamicCamera( LPDIRECT3DDEVICE9 pDevice )
: CCamera(pDevice)
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

_int CDynamicCamera::Update( const _float& fDelta )
{
	POINT			ptCenter = { WINCX >> 1, WINCY >> 1 };

	ClientToScreen(g_hWnd, &ptCenter);

	SetCursorPos(ptCenter.x, ptCenter.y);

	KeyCheck(fDelta);

	_int iExit = Engine::CCamera::Update(fDelta);

	return iExit;
}

void CDynamicCamera::KeyCheck( const _float& fDelta )
{
	MyMat			matWorld;
	D3DXMatrixInverse(&matWorld, NULL, &m_matView);

	if(Engine::GetDIKeyState(DIK_W) & 0x80)
	{
		MyVec3			vLook;
		memcpy(&vLook, &matWorld.m[2][0], sizeof(MyVec3));

		MyVec3			vLength = *D3DXVec3Normalize(&vLook, &vLook) * 5.0f * fDelta;

		m_vEye += vLength;
		m_vAt += vLength;
	}

	if(Engine::GetDIKeyState(DIK_S) & 0x80)
	{
		MyVec3			vLook;
		memcpy(&vLook, &matWorld.m[2][0], sizeof(MyVec3));

		MyVec3			vLength = *D3DXVec3Normalize(&vLook, &vLook) * 5.0f * fDelta;

		m_vEye -= vLength;
		m_vAt -= vLength;
	}

	if(Engine::GetDIKeyState(DIK_A) & 0x80)
	{
		MyVec3			vLook;
		memcpy(&vLook, &matWorld.m[0][0], sizeof(MyVec3));

		MyVec3			vLength = *D3DXVec3Normalize(&vLook, &vLook) * 5.0f * fDelta;

		m_vEye -= vLength;
		m_vAt -= vLength;
	}

	if(Engine::GetDIKeyState(DIK_D) & 0x80)
	{
		MyVec3			vLook;
		memcpy(&vLook, &matWorld.m[0][0], sizeof(MyVec3));

		MyVec3			vLength = *D3DXVec3Normalize(&vLook, &vLook) * 5.0f * fDelta;

		m_vEye += vLength;
		m_vAt += vLength;
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
		::Safe_Release(pCamera);
	}

	return pCamera;
}
